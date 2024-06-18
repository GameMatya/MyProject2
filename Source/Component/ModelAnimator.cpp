#include "ModelAnimator.h"
#include "CompModel.h"
#include "imgui.h"
#include "System/Logger.h"
#include <queue>

void ModelAnimator::UpdateAnimations(const float& elapsedTime)
{
  // 全身のアニメーション
  UpdateAnimation(elapsedTime, ANIM_AREA::BOTTOM);

  // 下半身2アニメーション
  UpdateAnimationBottomSub(elapsedTime);

  // ルートモーション
  if (CheckRootMotion() == true) {
    ApplyRootMotion();
  }

  // 再生時間の更新
  UpdateAnimationSecond(elapsedTime, ANIM_AREA::BOTTOM);

  // 上半身にアニメーション番号が割り当てられていたら
  if (currentAnimeIndex[ANIM_AREA::UPPER] >= 0) {
    // 上半身のアニメーション
    UpdateAnimation(elapsedTime, ANIM_AREA::UPPER);

    // 再生時間の更新
    UpdateAnimationSecond(elapsedTime, ANIM_AREA::UPPER);
  }
}

void ModelAnimator::DrawImGui()
{
#ifdef _DEBUG
  if (ImGui::TreeNode("Motion")) {
    auto resource = model->GetResource_DebugOnly();
    auto& characterData = resource->GetCharacterData_DebugOnly();

    if (ImGui::Button("SetNowAnimation")) {
      strcpy_s(animeName, resource->GetAnimations().at(currentAnimeIndex[ANIM_AREA::BOTTOM]).name.c_str());
      serializeSuccess = false;
    }
    if (ImGui::InputText("AnimeName", animeName, 256)) {
      serializeSuccess = false;
    }
    int animeID = FindAnimationId(animeName);

    isSetRoot = true;
    if (animeID < 0) {
      isSetRoot = false;
    }
    else if (characterData.rootMotionParams.find(animeID) == characterData.rootMotionParams.end()) {
      isSetRoot = false;
    }

    if (isSetRoot == true) {
      float scale = characterData.rootMotionParams.at(animeID).motionScale;
      ImGui::DragFloat("Scale", &scale);
      resource->SetRootMotionScale(animeID, scale);

      if (ImGui::Button("Save")) {
        isSerialize = true;
        serializeSuccess = resource->CharacterSerialize((resource->ResourcePath()).c_str());
      }
      if (isSerialize) {
        if (serializeSuccess) {
          ImGui::TextColored(ImVec4(0, 1, 0, 1), "Success");
        }
        else {
          ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed");
        }
      }
    }

    if (isSetRoot == false) {
      ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed");

      if (ImGui::Button("EmplaceNewParam")) {
        characterData.rootMotionParams.emplace(
          animeID,
          ModelResource::CharacterData::RootMotionParam(
            300.0f,
            std::vector<DirectX::XMFLOAT3>()
          )
        );
      }
    }

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Index")) {
    ImGui::Text((std::string("AnimationID Bottom : ") + std::to_string(currentAnimeIndex[ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("AnimationID BottomSub : ") + std::to_string(currentAnimeIndex[ANIM_AREA::BOTTOM_SUB])).c_str());
    ImGui::Text((std::string("AnimationID Upper : ") + std::to_string(currentAnimeIndex[ANIM_AREA::UPPER])).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Speed")) {
    ImGui::Text((std::string("Speed Bottom : ") + std::to_string(animeSpeed[ModelAnimator::ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("Speed Upper : ") + std::to_string(animeSpeed[ModelAnimator::ANIM_AREA::UPPER])).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Second")) {
    ImGui::Text((std::string("CurrentSecond Bottom : ") + std::to_string(currentAnimeSeconds[ModelAnimator::ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("CurrentSecond Upper : ") + std::to_string(currentAnimeSeconds[ModelAnimator::ANIM_AREA::UPPER])).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Frame")) {
    int currentFrameB = static_cast<int>(currentAnimeSeconds[ModelAnimator::ANIM_AREA::BOTTOM] * 60.0f);
    int currentFrameU = static_cast<int>(currentAnimeSeconds[ModelAnimator::ANIM_AREA::UPPER] * 60.0f);

    ImGui::Text((std::string("CurrentFrame Bottom : ") + std::to_string(currentFrameB)).c_str());
    ImGui::Text((std::string("CurrentFrame Upper : ") + std::to_string(currentFrameU)).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Blend")) {
    ImGui::Text((std::string("BlendTime Bottom : ") + std::to_string(animeBlendTimer[ModelAnimator::ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("BlendTime Upper : ") + std::to_string(animeBlendTimer[ModelAnimator::ANIM_AREA::UPPER])).c_str());

    ImGui::TreePop();
  }

#endif // _DEBUG
}

int ModelAnimator::FindAnimationId(const std::string& name)
{
  auto& animations = model->resource->GetAnimations();
  for (int i = 0; i < static_cast<int>(animations.size()); ++i)
  {
    if (animations.at(i).name == name.c_str())
    {
      return i;
    }
  }
#ifdef _DEBUG
  Logger::Print("**Animation Not Found**\n\n");

#else
  // 見つからなければ停止
  assert(!"Animation Not Found");

#endif // _DEBUG

  return -1;
}

// アニメーション更新処理
void ModelAnimator::UpdateAnimation(const float& elapsedTime, const ANIM_AREA& bodyArea)
{
  if (!IsPlayAnimation(bodyArea))return;

  // 指定のアニメーションデータを取得
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[bodyArea]);

  // アニメーションデータからキーフレームデータリストを取得
  const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
  // キーフレームの数を取得
  int keyCount = (int)keyframes.size();

  // 処理するノード
  int startIndex = 0;
  int endIndex = (int)model->nodes.size();

  // 処理するエリアが腕なら、腕の範囲だけアニメーションする
  if (bodyArea == ANIM_AREA::UPPER) {
    startIndex = splitNodeId;
  }

  for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
  {
    // 現在の時間がどのキーフレームの間にいるか判定する
    const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
    const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

    // 現在のアニメーション経過時間が二つのキーフレームの間じゃなければ次のキーフレームへ
    if (currentAnimeSeconds[bodyArea] < keyframe0.seconds ||
      currentAnimeSeconds[bodyArea] >= keyframe1.seconds)
      continue;

    // 異なるアニメーションの補完率	( 0～1 )
    float blendRate = animeBlendTimer[bodyArea] / animeBlendSeconds[bodyArea];
    animeBlendTimer[bodyArea] += elapsedTime;

    // キーフレームの姿勢になるように補完する
    for (int nodeIndex = startIndex; nodeIndex < endIndex; ++nodeIndex)
    {
      if (bodyArea == ANIM_AREA::BOTTOM) {
        // 上半身がアニメーション中だったら
        if (IsPlayAnimation(ANIM_AREA::UPPER) == true)
        {
          // ノードの更新スキップ
          if (nodeIndex >= splitNodeId)
            continue;
        }
      }

      // 2つのキーフレーム間の補完計算
      // キーフレームに登録されているノード情報を取得（姿勢データ）
      const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
      const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

      // ノードのコンテナからノードを取得
      CompModel::Node& node = model->nodes[nodeIndex];

      // ブレンド補完処理 ( アニメーションの切り替え )
      if (blendRate < 1.0f)
      {
        // 現在の姿勢と次のキーフレームの姿勢の間を補完する

        // Translate補完
        DirectX::XMVECTOR translate0 = DirectX::XMLoadFloat3(&node.translate);
        DirectX::XMVECTOR translate1 = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMStoreFloat3(&node.translate, DirectX::XMVectorLerp(translate0, translate1, blendRate));

        // Scale補完
        DirectX::XMVECTOR scale0 = DirectX::XMLoadFloat3(&node.scale);
        DirectX::XMVECTOR scale1 = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMStoreFloat3(&node.scale, DirectX::XMVectorLerp(scale0, scale1, blendRate));

        // Rotate補完　クォータニオン
        DirectX::XMVECTOR rotate0 = DirectX::XMLoadFloat4(&node.rotate);
        DirectX::XMVECTOR rotate1 = DirectX::XMLoadFloat4(&key0.rotate);

        DirectX::XMStoreFloat4(&node.rotate, DirectX::XMQuaternionSlerp(rotate0, rotate1, blendRate));
      }

      // 通常の計算 ( 同じアニメーション ) 
      else
      {
        // 再生時間とキーフレームの時間から補完率を算出する　（ 0 ～ 1 以内）
        float rate = (currentAnimeSeconds[bodyArea] - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

        // 前のキーフレームと次のキーフレームの姿勢を補完
        // Translate補完
        DirectX::XMVECTOR key0translate = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMVECTOR key1translate = DirectX::XMLoadFloat3(&key1.translate);

        DirectX::XMStoreFloat3(&node.translate, DirectX::XMVectorLerp(key0translate, key1translate, rate));

        // Scale補完
        DirectX::XMVECTOR key0scale = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMVECTOR key1scale = DirectX::XMLoadFloat3(&key1.scale);
        DirectX::XMStoreFloat3(&node.scale, DirectX::XMVectorLerp(key0scale, key1scale, rate));

        // Rotate補完　（クォータニオン）
        DirectX::XMVECTOR key0rotate = DirectX::XMLoadFloat4(&key0.rotate);
        DirectX::XMVECTOR key1rotate = DirectX::XMLoadFloat4(&key1.rotate);

        DirectX::XMStoreFloat4(&node.rotate, DirectX::XMQuaternionSlerp(key0rotate, key1rotate, rate));
      }
    }
    break;
  }

  // 最終フレーム処理
  if (animeEndFlag[bodyArea])
  {
    animeEndFlag[bodyArea] = false;
    currentAnimeIndex[bodyArea] = -1;
    // 下半身アニメーションが終了したら下半身2も止める
    if (bodyArea == ANIM_AREA::BOTTOM) {
      currentAnimeIndex[ANIM_AREA::BOTTOM_SUB] = -1;
    }
    return;
  }
}

// 下半身2アニメーション更新処理
void ModelAnimator::UpdateAnimationBottomSub(const float& elapsedTime)
{
  // 再生中でないなら処理しない
  if (currentAnimeIndex[ANIM_AREA::BOTTOM_SUB] < 0)return;

  // 指定のアニメーションデータを取得
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[ANIM_AREA::BOTTOM_SUB]);

  // アニメーションデータからキーフレームデータリストを取得
  const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
  // キーフレームの数を取得
  int keyCount = (int)keyframes.size();

  // 処理するノードの範囲を決める
  int startIndex = 0;
  int endIndex = (int)model->nodes.size();

  for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
  {
    // 現在の時間がどのキーフレームの間にいるか判定する
    const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
    const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

    // 現在のアニメーション経過時間が二つのキーフレームの間じゃなければ次のキーフレームへ
    if (currentAnimeSeconds[ANIM_AREA::BOTTOM] < keyframe0.seconds ||
      currentAnimeSeconds[ANIM_AREA::BOTTOM] >= keyframe1.seconds)
      continue;

    // 異なるアニメーションの補完率	( 0～1 )
    float blendRate = animeBlendTimer[ANIM_AREA::BOTTOM_SUB] / animeBlendSeconds[ANIM_AREA::BOTTOM];
    animeBlendTimer[ANIM_AREA::BOTTOM_SUB] += elapsedTime;

    // キーフレームの姿勢
    DirectX::XMFLOAT3 translate = {};
    DirectX::XMFLOAT3 scale = {};
    DirectX::XMFLOAT4 rotate = {};

    // キーフレームの姿勢になるように補完する
    for (int nodeIndex = startIndex; nodeIndex < endIndex; ++nodeIndex)
    {
      // 上半身がアニメーション中だったら
      if (IsPlayAnimation(ANIM_AREA::UPPER) == true)
      {
        // ノードの更新スキップ
        if (nodeIndex >= splitNodeId)
          continue;
      }

      // 2つのキーフレーム間の補完計算
      // キーフレームに登録されているノード情報を取得（姿勢データ）
      const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
      const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

      // ノードのコンテナからノードを取得
      CompModel::Node& node = model->nodes[nodeIndex];

      // ブレンド補完処理 ( アニメーションの切り替え )
      if (blendRate < 1.0f)
      {
        // 現在の姿勢と次のキーフレームの姿勢の間を補完する

        // Translate補完
        DirectX::XMVECTOR translate0 = DirectX::XMLoadFloat3(&node.translate);
        DirectX::XMVECTOR translate1 = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMStoreFloat3(&translate, DirectX::XMVectorLerp(translate0, translate1, blendRate));

        // scale補完
        DirectX::XMVECTOR scale0 = DirectX::XMLoadFloat3(&node.scale);
        DirectX::XMVECTOR scale1 = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMStoreFloat3(&scale, DirectX::XMVectorLerp(scale0, scale1, blendRate));

        // Rotate補完　クォータニオン
        DirectX::XMVECTOR rotate0 = DirectX::XMLoadFloat4(&node.rotate);
        DirectX::XMVECTOR rotate1 = DirectX::XMLoadFloat4(&key0.rotate);

        DirectX::XMStoreFloat4(&rotate, DirectX::XMQuaternionSlerp(rotate0, rotate1, blendRate));
      }

      // 通常の計算 ( 同じアニメーション ) 
      else
      {
        // 再生時間とキーフレームの時間から補完率を算出する　（ 0 ～ 1 以内）
        float rate = (currentAnimeSeconds[ANIM_AREA::BOTTOM] - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

        // 前のキーフレームと次のキーフレームの姿勢を補完
        // Translate補完
        DirectX::XMVECTOR key0translate = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMVECTOR key1translate = DirectX::XMLoadFloat3(&key1.translate);

        DirectX::XMStoreFloat3(&translate, DirectX::XMVectorLerp(key0translate, key1translate, rate));

        // scale補完
        DirectX::XMVECTOR key0scale = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMVECTOR key1scale = DirectX::XMLoadFloat3(&key1.scale);
        DirectX::XMStoreFloat3(&scale, DirectX::XMVectorLerp(key0scale, key1scale, rate));

        // Rotate補完　（クォータニオン）
        DirectX::XMVECTOR key0rotate = DirectX::XMLoadFloat4(&key0.rotate);
        DirectX::XMVECTOR key1rotate = DirectX::XMLoadFloat4(&key1.rotate);

        DirectX::XMStoreFloat4(&rotate, DirectX::XMQuaternionSlerp(key0rotate, key1rotate, rate));
      }

      // キーフレームの姿勢を1個目のアニメーション( Bottom )の姿勢と合成
      node.translate = Mathf::Lerp(node.translate, translate, animeBlendRate);
      node.scale = Mathf::Lerp(node.scale, scale, animeBlendRate);
      node.rotate = Mathf::Slerp(node.rotate, rotate, animeBlendRate);
    }
    break;
  }
}

bool ModelAnimator::CheckEvent(const ANIM_AREA& area, const ANIMATION_EVENT& animEvent, const int& index) const
{
  // 再生中じゃなければ,エリアが下半身2だったら,停止する
  if (IsPlayAnimation(area) == false)return false;
  assert(area != ANIM_AREA::BOTTOM_SUB);

  int currentFrame = static_cast<int>(currentAnimeSeconds[area] * 60.0f);
  const std::vector<ModelResource::Animation::Event>& events = model->resource->GetAnimations().at(currentAnimeIndex[area]).events;

  // 対応するイベントを探す
  std::queue<ModelResource::Animation::Event> eventQueue;
  for (auto& itr : events) {
    if (itr.type != animEvent)continue;
    if (itr.index != index)continue;
    eventQueue.push(itr);
  }

  while (eventQueue.empty() == false) {
    const ModelResource::Animation::Event& itr = std::move(eventQueue.front());
    eventQueue.pop();

    if (itr.frames.x > currentFrame) continue;
    if (itr.frames.y < currentFrame) continue;

    return true;
  }

  return false;
}

void ModelAnimator::UpdateAnimationSecond(const float& elapsedTime, const ANIM_AREA& area)
{
  // 再生中でないなら処理しない
  if (!IsPlayAnimation(area))return;

  // 指定のアニメーションデータを取得
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[area]);

  // 前フレームのアニメーション時間を保存 ( ルートモーション用 )
  if (area == ANIM_AREA::BOTTOM) {
    oldAnimationSecond = currentAnimeSeconds[area];
  }

  // 時間経過
  currentAnimeSeconds[area] += elapsedTime * animeSpeed[area];

  // 再生時間が終端時間を超えたら
  if (currentAnimeSeconds[area] >= animation.secondsLength)
  {
    // アニメーションのループフラグ
    if (animeLoopFlag[area])
    {
      // 再生時間を巻き戻す
      currentAnimeSeconds[area] -= animation.secondsLength;
    }
    else
    {
      // アニメーションの終了
      animeEndFlag[area] = true;
      oldAnimationSecond = 0.0f;

      return;
    }
  }
}

void ModelAnimator::ApplyRootMotion()
{
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[ANIM_AREA::BOTTOM]);

  // 初回、前回、今回のルートモーションノードの姿勢を取得
  DirectX::XMFLOAT3 beginPos, oldPos, newPos;
  beginPos = CalculateRootNodePos(0.0f);
  oldPos = CalculateRootNodePos(oldAnimationSecond);
  newPos = CalculateRootNodePos(currentAnimeSeconds[ANIM_AREA::BOTTOM]);

  // ローカル空間で差分を求める
  DirectX::XMFLOAT3 localTranslation = {};

  // 現在のアニメーション時間が、前のフレームのアニメーション時間より小さい場合 ( ループアニメーション )
  if (currentAnimeSeconds[ANIM_AREA::BOTTOM] < oldAnimationSecond)
  {
    // 終端の姿勢を取り出す
    DirectX::XMFLOAT3 endPos;
    // float誤差を考慮し、0.001fだけ減らす
    endPos = CalculateRootNodePos(animation.secondsLength - 0.001f);

    // 前回->終端の移動差分値を算出する
    DirectX::XMFLOAT3 old_endTranslation, begin_newTranslation;
    old_endTranslation = endPos - oldPos;

    // 初回->今回の移動差分値を算出する
    begin_newTranslation = newPos - beginPos;

    // 算出した移動差分値の合計値をローカル移動差分値として扱う
    localTranslation = old_endTranslation + begin_newTranslation;
  }
  else
  {
    localTranslation = newPos - oldPos;
  }

  // ルートモーションノードを初回の姿勢にする
  CompModel::Node& node = model->nodes[rootNodeId];
  node.translate = beginPos;

  // ワールド空間での移動値を求める
  ObjectTransform& transform = model->gameObject.lock()->transform;
  DirectX::XMFLOAT3 worldTranslation;
  DirectX::XMVECTOR W = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&localTranslation), transform.GetWorldTransform());
  DirectX::XMStoreFloat3(&worldTranslation, W);

  // ルートモーションの影響度
  float rootMotionScale = DEFAULT_MOTION_SCALE;
  auto motionParam = model->GetResource()->GetCharacterData().rootMotionParams;
  if (motionParam.find(currentAnimeIndex[ANIM_AREA::BOTTOM]) != motionParam.end()) {
    rootMotionScale = motionParam.at(currentAnimeIndex[ANIM_AREA::BOTTOM]).motionScale;
  }

  worldTranslation *= rootMotionScale;

  // 移動値を反映
  if (animation.applyAxisUpFlg)
  {
    transform.position += worldTranslation;
  }
  else
  {
    // Y軸は無視
    transform.position.x += worldTranslation.x;
    transform.position.z += worldTranslation.z;
  }
}

bool ModelAnimator::CheckRootMotion()
{
  // アニメーションを再生していない
  if (!IsPlayAnimation(ANIM_AREA::BOTTOM))return false;
  // ルートノードが設定されていない
  if (rootNodeId < 0)return false;

  // ルートモーションを適用しないアニメーションの場合
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[ANIM_AREA::BOTTOM]);
  if (animation.rootMotionFlg == false)return false;

  return true;
}

DirectX::XMFLOAT3 ModelAnimator::CalculateRootNodePos(const float& animationTime)
{
  // 指定のアニメーションデータを取得
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[ANIM_AREA::BOTTOM]);

  // アニメーションデータからキーフレームデータリストを取得
  const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
  // キーフレームの数を取得
  int keyCount = (int)keyframes.size();

  // ルートノードの姿勢を求める
  for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
  {
    // 時間がどのキーフレームの間にいるか判定する
    const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
    const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

    // アニメーション経過時間が二つのキーフレームの間じゃなければ次のキーフレームへ
    if (animationTime < keyframe0.seconds ||
      animationTime > keyframe1.seconds)
      continue;

    // キーフレームの姿勢になるように補完する
    {
      // 2つのキーフレーム間の補完計算
      // キーフレームに登録されているノード情報を取得（姿勢データ）
      const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(rootNodeId);
      const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(rootNodeId);

      // 通常の計算
      // 再生時間とキーフレームの時間から補完率を算出する（ 0 ～ 1 以内）
      float rate = (animationTime - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

      // 前のキーフレームと次のキーフレームの姿勢を補完
      // Translate補完
      DirectX::XMVECTOR key0translate = DirectX::XMLoadFloat3(&key0.translate);
      DirectX::XMVECTOR key1translate = DirectX::XMLoadFloat3(&key1.translate);

      DirectX::XMFLOAT3 rootNodeTranslate = {};
      DirectX::XMStoreFloat3(&rootNodeTranslate, DirectX::XMVectorLerp(key0translate, key1translate, rate));

      return rootNodeTranslate;
    }
  }

  assert(!"RootMotionError");
  return DirectX::XMFLOAT3(0, 0, 0);
}

// アニメーション再生
void ModelAnimator::PlayAnimation(const ANIM_AREA& area, const int& index, const bool& isLoop, const float& blendSeconds)
{
  // アニメーション番号
  currentAnimeIndex[area] = index;
  currentAnimeSeconds[area] = 0.0f;

  // ループ設定
  animeLoopFlag[area] = isLoop;
  animeEndFlag[area] = false;

  // アニメーション遷移補完設定
  animeBlendTimer[area] = 0.0f;
  animeBlendSeconds[area] = blendSeconds;

  oldAnimationSecond = 0.0f;

  animeSpeed[area] = 1.0f;
}

void ModelAnimator::PlayAnimationBottomSub(const int& index)
{
  // アニメーション番号
  currentAnimeIndex[ANIM_AREA::BOTTOM_SUB] = index;

  // アニメーション遷移補完設定
  animeBlendTimer[ANIM_AREA::BOTTOM_SUB] = 0.0f;
}

// アニメーション再生中か
bool ModelAnimator::IsPlayAnimation(const ANIM_AREA& armArea)const
{
  if (currentAnimeIndex[armArea] < 0)return false;

  return true;
}

float ModelAnimator::GetAnimationRate(const ANIM_AREA& bodyArea) const
{
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();

  return currentAnimeSeconds[bodyArea] / animations.at(currentAnimeIndex[bodyArea]).secondsLength;
}
