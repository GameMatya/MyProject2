#include "CompCharacter.h"
#include "GameObject/GameObjectManager.h"
#include "CompModel.h"
#include "Graphics/EffectManager.h"

#include "imgui.h"
#include <algorithm>

void CompCharacter::Start()
{
  model = gameObject.lock()->GetComponent<CompModel>().get();

  // ModelResourceからキャラクターデータを読み込む
  SettingCharacterDatas();

  // マネージャーに自身を登録
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RegisterCharacter(gameObject);
}

void CompCharacter::Update(const float& elapsedTime)
{
  // 速力処理更新
  UpdateVelocity(elapsedTime);
}

void CompCharacter::OnDestroy()
{
  gameObject.lock()->GetObjectManager()->GetCharacterManager().RemoveCharacter(gameObject);
}

void CompCharacter::Move(DirectX::XMFLOAT3 direction, float maxSpeed)
{
  // 移動方向ベクトルを設定
  moveVecXZ.x = direction.x;
  moveVecXZ.z = direction.z;

  // 最大速度設定
  maxMoveSpeed = maxSpeed;
}

void CompCharacter::AnimationFromVectors(DirectX::XMFLOAT3 moveVec, int baseAnime, bool isLoop, const float& blendSecond)
{
  ObjectTransform& transform = gameObject.lock()->transform;
  ModelAnimator& animator = model->animator;

  // 前後アニメーション
  int zAnime = 0;

  // 前後判定
  {
    // キャラクターの前方向と入力方向で内積
    float zDot = Mathf::Dot(transform.forward, moveVec);

    // 内積の結果が正なら前方向アニメーション
    zAnime += (zDot < 0);
  }

  // 左右アニメーション ( 前後アニメーションの分 +2 )
  int xAnime = 2;
  // 左右の内積値 ( ブレンド率に使用 )
  float xDot = 0.0f;

  // 左右判定
  {
    // キャラクターの右方向と入力方向で内積
    xDot = Mathf::Dot(transform.right, moveVec);

    // 内積の結果が正なら右方向アニメーション
    xAnime += (xDot < 0);
  }

  // それぞれの判定結果と現在のアニメーションが違えば、判定結果のアニメーションを再生
  if (animator.GetCurrentAnimationId(animator.ANIM_AREA::BOTTOM) != baseAnime + zAnime)
  {
    animator.PlayAnimation(ModelAnimator::ANIM_AREA::BOTTOM, baseAnime + zAnime, isLoop, blendSecond);
  }
  if (animator.GetCurrentAnimationId(animator.ANIM_AREA::BOTTOM_SUB) != baseAnime + xAnime)
  {
    animator.PlayAnimationBottomSub(baseAnime + xAnime);
  }

  // 移動ベクトルが左右方向に近いほど、左右アニメーションのブレンド率を高くする
  animator.SetBottomBlendRate(fabs(xDot));
}

void CompCharacter::Turn(const float& elapsedTime, DirectX::XMFLOAT3 directionXZ, float turnSpeed)
{
  // 1フレームあたりの旋回速度
  float speed = turnSpeed * elapsedTime;

  ObjectTransform& transform = gameObject.lock()->transform;

  DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&directionXZ);
  Direction = DirectX::XMVector3Normalize(Direction);
  DirectX::XMVECTOR Rotation = DirectX::XMLoadFloat4(&transform.rotation);

  DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&gameObject.lock()->transform.forward);

  // 回転する任意軸を作成
  DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Forward, Direction);
  if (DirectX::XMVector3Equal(Axis, DirectX::XMVectorZero()))
  {
    return;
  }

  // ベクトル同士の角度の差を求める
  float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(Forward, Direction));
  // スピードを調整 ( 向きが近くなるほど遅くなる )
  speed = min(1.0f - dot, speed);

  // 回転処理
  DirectX::XMVECTOR Turn = DirectX::XMQuaternionRotationAxis(Axis, speed);
  gameObject.lock()->transform.AddRotation(Turn);
}

void CompCharacter::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
  // 速力に力を加える
  velocity.x += impulse.x;
  velocity.y += impulse.y;
  velocity.z += impulse.z;
}

void CompCharacter::UpdateVelocity(const float& elapsedTime)
{
  // 経過フレーム
  float elapsedFrame = 60.0f * elapsedTime;

  // 垂直速力更新処理
  UpdateVerticalVelocity(elapsedFrame);

  // 水平速力更新処理
  UpdateHorizontalVelocity(elapsedFrame);

  // 垂直移動更新処理
  UpdateVerticalMove(elapsedTime);

  // 水平移動更新更新
  UpdateHorizontalMove(elapsedTime);
}

// ダメージを与える
bool CompCharacter::ApplyDamage(float damage, float invincibleTime)
{
  // 無敵時間
  if (invincibleTimer > 0)return false;

  // ダメージが0の場合は健康状態を更新する必要がない
  if (damage == 0)return false;

  // 死亡している場合は健康状態を変更しない
  if (health <= 0)return false;

  // ダメージ処理
  health -= damage;

  // 無敵時間
  invincibleTimer = invincibleTime;

  // 死亡通知
  if (health <= 0)
  {
    health = 0.0f;
    OnDead();
  }
  // ダメージ通知
  else
  {
    OnDamaged();
  }

  // 健康状態が変更した場合はtrueを返す
  return true;
}

void CompCharacter::DrawImGui()
{
  ImGui::InputFloat("Health", &health);
  ImGui::DragFloat("Move Speed", &moveSpeed);

  ImGui::InputFloat3("Velocity", &velocity.x);
  ImGui::Checkbox("IsGround", &isGround);
}

void CompCharacter::OnDamaged()
{
}

void CompCharacter::UpdateInvincibleTimer(const float& elapsedTime)
{
  if (invincibleTimer > 0.0f)
  {
    invincibleTimer -= elapsedTime;
  }
}

void CompCharacter::SettingCharacterDatas()
{
  const ModelResource::CharacterData& data = model->GetResource()->GetCharacterData();

  // 腰ノードを読み込む
  {
    if (data.spineNodeId > 0)
      model->animator.SetSplitID(data.spineNodeId);
  }

  // 押し出しの強さを設定
  GetGameObject()->SetPushPower(data.pushPower);

  // 腰の高さを設定
  waistHeight = data.waistHeight * GetGameObject()->transform.scale.y;
}

void CompCharacter::UpdateVerticalVelocity(const float& elapsedFrame)
{
  // 重力処理
  velocity.y -= GRAVITY * gravityAffect * elapsedFrame;
  velocity.y = (std::max)(velocity.y, MAX_FALL_SPEED);
}

void CompCharacter::UpdateVerticalMove(const float& elapsedTime)
{
  // 垂直方向の移動量
  float my = velocity.y * elapsedTime;

  // 下り坂でガタガタにならないようする
  if (isGround && slopeRate < 0.5f && velocity.y < 0) {
    my += -slopeRate * 3;
  }

  float stepOffset = 1.0f;
  {
    // レイの開始位置は足元より少し上
    DirectX::XMFLOAT3 start = gameObject.lock()->transform.position;
    start.y += stepOffset;

    // レイの終点位置は移動後の位置
    DirectX::XMFLOAT3 end = gameObject.lock()->transform.position;
    end.y += my;

    // レイキャストによる地面判定
    HitResult hit;
    if (gameObject.lock()->GetObjectManager()->GetStageManager().Collision(start, end, &hit))
    {
      // 地面に接地している
      gameObject.lock()->transform.position = hit.position;

      // 傾斜率の計算
      DirectX::XMVECTOR Horizon = DirectX::XMLoadFloat3(&hit.normal);
      Horizon = DirectX::XMVectorSetY(Horizon, 0);
      float horizon = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Horizon));

      // 値が１に近いほど壁になる
      slopeRate = 1 - (hit.normal.y / (hit.normal.y + horizon));

      // 着地した
      if (!isGround)
      {
        OnLanding();
      }
      isGround = true;
      velocity.y = 0.0f;
    }
    else
    {
      // 空中に浮いている
      gameObject.lock()->transform.position.y += my;
      isGround = false;
    }
  }
}

void CompCharacter::UpdateHorizontalVelocity(const float& elapsedFrame)
{
  // XZ平面の速力を減速する
  float lengthSq = velocity.x * velocity.x + velocity.z * velocity.z;
  if (lengthSq > 0.0f)
  {
    // 摩擦力
    float friction = this->friction * elapsedFrame;
    // 空中にいるときは摩擦力を減らす
    if (!isGround) friction *= airControl;

    // 摩擦による横方向の減速処理
    if (lengthSq > friction * friction)
    {
      float length = sqrtf(lengthSq);
      float vx = velocity.x / length;
      float vz = velocity.z / length;

      velocity.x -= vx * friction;
      velocity.z -= vz * friction;
    }
    // 横方向の速力が摩擦力以下になったので速力を無効化しリターン
    else
    {
      velocity.x = 0.0f;
      velocity.z = 0.0f;
      // 移動ベクトルをリセット
      moveVecXZ = {};
      return;
    }
  }

  lengthSq = velocity.x * velocity.x + velocity.z * velocity.z;
  float moveVecLengthSq = Mathf::Dot(moveVecXZ, moveVecXZ);

  // XZ平面の速力を加速する
  if (lengthSq <= maxMoveSpeed * maxMoveSpeed)
  {
    // 移動ベクトルがゼロベクトルでないなら加速する
    if (moveVecLengthSq > 0.0f)
    {
      // 加速力
      float acceleration = this->accelerate * elapsedFrame;

      // 移動ベクトルによる加速処理
      velocity.x += acceleration * moveVecXZ.x;
      velocity.z += acceleration * moveVecXZ.z;

      // 最大速度制限
      float length = sqrtf(lengthSq);
      if (length > maxMoveSpeed)
      {
        velocity.x = (velocity.x / length) * maxMoveSpeed;
        velocity.z = (velocity.z / length) * maxMoveSpeed;
      }
    }
  }
  // 移動速度が最大移動速度より大きかったら移動入力に基づいて減速
  else
  {
    // 移動ベクトルがゼロベクトルでないなら加速する
    if (moveVecLengthSq > 0.0f)
    {
      // 加速力
      float acceleration = this->accelerate * elapsedFrame;

      // 移動ベクトルによる加速処理
      DirectX::XMFLOAT3 newVelocity = velocity;
      newVelocity.x += acceleration * moveVecXZ.x;
      newVelocity.y = 0;
      newVelocity.z += acceleration * moveVecXZ.z;

      // 加速後の移動ベクトルが元の移動ベクトルより小さければ反映 ( 減速していれば反映 )
      if (Mathf::Dot(newVelocity, newVelocity) < lengthSq) {
        velocity.x = newVelocity.x;
        velocity.z = newVelocity.z;
      }
    }
  }

  // 移動ベクトルをリセット
  moveVecXZ = {};
}

void CompCharacter::UpdateHorizontalMove(const float& elapsedTime)
{
  // 水平速力量計算
  DirectX::XMFLOAT2 Speed = {
    velocity.x,
    velocity.z
  };
  float velocityLengthXZ = DirectX::XMVectorGetX(DirectX::XMVector2Length(XMLoadFloat2(&Speed)));

  if (velocityLengthXZ > 0.0f)
  {
    // 水平移動量
    float mx = velocity.x * elapsedTime;
    float mz = velocity.z * elapsedTime;

    float stepOffset = 0.5f;

    // レイの始点位置と終点位置
    DirectX::XMFLOAT3 start = { gameObject.lock()->transform.position.x,gameObject.lock()->transform.position.y + stepOffset,gameObject.lock()->transform.position.z };
    DirectX::XMFLOAT3 end = { gameObject.lock()->transform.position.x + mx,gameObject.lock()->transform.position.y + stepOffset,gameObject.lock()->transform.position.z + mz };

    // レイキャストによる壁判定
    HitResult hit;
    if (gameObject.lock()->GetObjectManager()->GetStageManager().Collision(start, end, &hit))
    {
      // 移動後の位置から壁までのベクトル
      DirectX::XMVECTOR Start = XMLoadFloat3(&start);
      DirectX::XMVECTOR End = XMLoadFloat3(&end);
      DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Start, End);

      // 壁の法線
      DirectX::XMVECTOR Normal = XMLoadFloat3(&hit.normal);

      // 入射ベクトルを法線ベクトルに射影
      DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Normal);

      // 補正位置の計算
      DirectX::XMFLOAT3 correction;
      DirectX::XMStoreFloat3(&correction, DirectX::XMVectorMultiplyAdd(Normal, Dot, End));

      // 補正後の位置が壁にめり込んでいたら
      HitResult hit2;
      if (!gameObject.lock()->GetObjectManager()->GetStageManager().Collision(start, correction, &hit2))
      {
        gameObject.lock()->transform.position.x = correction.x;
        gameObject.lock()->transform.position.z = correction.z;
      }
    }
    else
    {
      gameObject.lock()->transform.position.x += mx;
      gameObject.lock()->transform.position.z += mz;
    }
  }
}
