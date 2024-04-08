#include "EnemySupport.h"
#include "GameObject/GameObjectManager.h"

void EnemySupport::Initialize(WeakObject owner)
{
  this->owner = owner;
  this->player = owner.lock()->GetObjectManager()->GetCharacterManager().GetPlayer();
  territoryOrigin = owner.lock()->transform.position;

  // マネージャーに自身を登録
  owner.lock()->GetObjectManager()->GetCharacterManager().RegisterEnemy(owner);
}

bool EnemySupport::SearchPlayer()
{
  // プレイヤーが死亡していたら何もしない
  if (IsAlivePlayer() == false)return false;

  DirectX::XMFLOAT3 vec = GetPlayerVec();
  vec.y = 0;

  // 距離を算出 ( 2乗の値 )
  float distSq = Mathf::Dot(vec);

  if (distSq < searchRange * searchRange)
  {
    // 2つのベクトルの内積値で前後判定
    float dot = Mathf::Dot(Mathf::Normalize(vec), owner.lock()->transform.forward);

    // プレイヤーが自身の前方に居ればTrue
    if (dot > 0.0f)
    {
      return true;
    }
  }

  return false;
}

void EnemySupport::SetRandomTarget()
{
  targetPosition.x = territoryOrigin.x + Mathf::RandomRange(-territoryRange, territoryRange);
  targetPosition.z = territoryOrigin.z + Mathf::RandomRange(-territoryRange, territoryRange);
}

DirectX::XMFLOAT3 EnemySupport::GetPlayerVec()
{
  return player.lock()->transform.position - owner.lock()->transform.position;
}

DirectX::XMFLOAT3 EnemySupport::GetTargetDir()
{
  DirectX::XMFLOAT3 vec = targetPosition - owner.lock()->transform.position;
  float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

  // 正規化ベクトルを返す
  return vec / length;
}

DirectX::XMFLOAT3 EnemySupport::GetTargetDirXZ()
{
  DirectX::XMFLOAT3 vec = targetPosition - owner.lock()->transform.position;
  vec.y = 0;
  float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

  // 正規化ベクトルを返す
  return vec / length;
}

DirectX::XMFLOAT3 EnemySupport::GetTargetVec()
{
  return targetPosition - owner.lock()->transform.position;
}

bool EnemySupport::IsAlivePlayer()
{
  return !owner.lock()->GetObjectManager()->GetCharacterManager().GetPlayer().expired();

}
