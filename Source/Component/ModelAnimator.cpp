#include "ModelAnimator.h"
#include "CompModel.h"
#include "imgui.h"
#include "System/Logger.h"
#include <queue>

void ModelAnimator::UpdateAnimations(const float& elapsedTime)
{
  // �S�g�̃A�j���[�V����
  UpdateAnimation(elapsedTime, ANIM_AREA::BOTTOM);

  // �����g2�A�j���[�V����
  UpdateAnimationBottomSub(elapsedTime);

  // ���[�g���[�V����
  if (CheckRootMotion() == true) {
    ApplyRootMotion();
  }

  // �Đ����Ԃ̍X�V
  UpdateAnimationSecond(elapsedTime, ANIM_AREA::BOTTOM);

  // �㔼�g�ɃA�j���[�V�����ԍ������蓖�Ă��Ă�����
  if (currentAnimeIndex[ANIM_AREA::UPPER] >= 0) {
    // �㔼�g�̃A�j���[�V����
    UpdateAnimation(elapsedTime, ANIM_AREA::UPPER);

    // �Đ����Ԃ̍X�V
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
  // ������Ȃ���Β�~
  assert(!"Animation Not Found");

#endif // _DEBUG

  return -1;
}

// �A�j���[�V�����X�V����
void ModelAnimator::UpdateAnimation(const float& elapsedTime, const ANIM_AREA& bodyArea)
{
  if (!IsPlayAnimation(bodyArea))return;

  // �w��̃A�j���[�V�����f�[�^���擾
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[bodyArea]);

  // �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
  const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
  // �L�[�t���[���̐����擾
  int keyCount = (int)keyframes.size();

  // ��������m�[�h
  int startIndex = 0;
  int endIndex = (int)model->nodes.size();

  // ��������G���A���r�Ȃ�A�r�͈̔͂����A�j���[�V��������
  if (bodyArea == ANIM_AREA::UPPER) {
    startIndex = splitNodeId;
  }

  for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
  {
    // ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
    const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
    const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

    // ���݂̃A�j���[�V�����o�ߎ��Ԃ���̃L�[�t���[���̊Ԃ���Ȃ���Ύ��̃L�[�t���[����
    if (currentAnimeSeconds[bodyArea] < keyframe0.seconds ||
      currentAnimeSeconds[bodyArea] >= keyframe1.seconds)
      continue;

    // �قȂ�A�j���[�V�����̕⊮��	( 0�`1 )
    float blendRate = animeBlendTimer[bodyArea] / animeBlendSeconds[bodyArea];
    animeBlendTimer[bodyArea] += elapsedTime;

    // �L�[�t���[���̎p���ɂȂ�悤�ɕ⊮����
    for (int nodeIndex = startIndex; nodeIndex < endIndex; ++nodeIndex)
    {
      if (bodyArea == ANIM_AREA::BOTTOM) {
        // �㔼�g���A�j���[�V��������������
        if (IsPlayAnimation(ANIM_AREA::UPPER) == true)
        {
          // �m�[�h�̍X�V�X�L�b�v
          if (nodeIndex >= splitNodeId)
            continue;
        }
      }

      // 2�̃L�[�t���[���Ԃ̕⊮�v�Z
      // �L�[�t���[���ɓo�^����Ă���m�[�h�����擾�i�p���f�[�^�j
      const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
      const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

      // �m�[�h�̃R���e�i����m�[�h���擾
      CompModel::Node& node = model->nodes[nodeIndex];

      // �u�����h�⊮���� ( �A�j���[�V�����̐؂�ւ� )
      if (blendRate < 1.0f)
      {
        // ���݂̎p���Ǝ��̃L�[�t���[���̎p���̊Ԃ�⊮����

        // Translate�⊮
        DirectX::XMVECTOR translate0 = DirectX::XMLoadFloat3(&node.translate);
        DirectX::XMVECTOR translate1 = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMStoreFloat3(&node.translate, DirectX::XMVectorLerp(translate0, translate1, blendRate));

        // Scale�⊮
        DirectX::XMVECTOR scale0 = DirectX::XMLoadFloat3(&node.scale);
        DirectX::XMVECTOR scale1 = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMStoreFloat3(&node.scale, DirectX::XMVectorLerp(scale0, scale1, blendRate));

        // Rotate�⊮�@�N�H�[�^�j�I��
        DirectX::XMVECTOR rotate0 = DirectX::XMLoadFloat4(&node.rotate);
        DirectX::XMVECTOR rotate1 = DirectX::XMLoadFloat4(&key0.rotate);

        DirectX::XMStoreFloat4(&node.rotate, DirectX::XMQuaternionSlerp(rotate0, rotate1, blendRate));
      }

      // �ʏ�̌v�Z ( �����A�j���[�V���� ) 
      else
      {
        // �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����@�i 0 �` 1 �ȓ��j
        float rate = (currentAnimeSeconds[bodyArea] - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

        // �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
        // Translate�⊮
        DirectX::XMVECTOR key0translate = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMVECTOR key1translate = DirectX::XMLoadFloat3(&key1.translate);

        DirectX::XMStoreFloat3(&node.translate, DirectX::XMVectorLerp(key0translate, key1translate, rate));

        // Scale�⊮
        DirectX::XMVECTOR key0scale = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMVECTOR key1scale = DirectX::XMLoadFloat3(&key1.scale);
        DirectX::XMStoreFloat3(&node.scale, DirectX::XMVectorLerp(key0scale, key1scale, rate));

        // Rotate�⊮�@�i�N�H�[�^�j�I���j
        DirectX::XMVECTOR key0rotate = DirectX::XMLoadFloat4(&key0.rotate);
        DirectX::XMVECTOR key1rotate = DirectX::XMLoadFloat4(&key1.rotate);

        DirectX::XMStoreFloat4(&node.rotate, DirectX::XMQuaternionSlerp(key0rotate, key1rotate, rate));
      }
    }
    break;
  }

  // �ŏI�t���[������
  if (animeEndFlag[bodyArea])
  {
    animeEndFlag[bodyArea] = false;
    currentAnimeIndex[bodyArea] = -1;
    // �����g�A�j���[�V�������I�������牺���g2���~�߂�
    if (bodyArea == ANIM_AREA::BOTTOM) {
      currentAnimeIndex[ANIM_AREA::BOTTOM_SUB] = -1;
    }
    return;
  }
}

// �����g2�A�j���[�V�����X�V����
void ModelAnimator::UpdateAnimationBottomSub(const float& elapsedTime)
{
  // �Đ����łȂ��Ȃ珈�����Ȃ�
  if (currentAnimeIndex[ANIM_AREA::BOTTOM_SUB] < 0)return;

  // �w��̃A�j���[�V�����f�[�^���擾
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[ANIM_AREA::BOTTOM_SUB]);

  // �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
  const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
  // �L�[�t���[���̐����擾
  int keyCount = (int)keyframes.size();

  // ��������m�[�h�͈̔͂����߂�
  int startIndex = 0;
  int endIndex = (int)model->nodes.size();

  for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
  {
    // ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
    const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
    const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

    // ���݂̃A�j���[�V�����o�ߎ��Ԃ���̃L�[�t���[���̊Ԃ���Ȃ���Ύ��̃L�[�t���[����
    if (currentAnimeSeconds[ANIM_AREA::BOTTOM] < keyframe0.seconds ||
      currentAnimeSeconds[ANIM_AREA::BOTTOM] >= keyframe1.seconds)
      continue;

    // �قȂ�A�j���[�V�����̕⊮��	( 0�`1 )
    float blendRate = animeBlendTimer[ANIM_AREA::BOTTOM_SUB] / animeBlendSeconds[ANIM_AREA::BOTTOM];
    animeBlendTimer[ANIM_AREA::BOTTOM_SUB] += elapsedTime;

    // �L�[�t���[���̎p��
    DirectX::XMFLOAT3 translate = {};
    DirectX::XMFLOAT3 scale = {};
    DirectX::XMFLOAT4 rotate = {};

    // �L�[�t���[���̎p���ɂȂ�悤�ɕ⊮����
    for (int nodeIndex = startIndex; nodeIndex < endIndex; ++nodeIndex)
    {
      // �㔼�g���A�j���[�V��������������
      if (IsPlayAnimation(ANIM_AREA::UPPER) == true)
      {
        // �m�[�h�̍X�V�X�L�b�v
        if (nodeIndex >= splitNodeId)
          continue;
      }

      // 2�̃L�[�t���[���Ԃ̕⊮�v�Z
      // �L�[�t���[���ɓo�^����Ă���m�[�h�����擾�i�p���f�[�^�j
      const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
      const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

      // �m�[�h�̃R���e�i����m�[�h���擾
      CompModel::Node& node = model->nodes[nodeIndex];

      // �u�����h�⊮���� ( �A�j���[�V�����̐؂�ւ� )
      if (blendRate < 1.0f)
      {
        // ���݂̎p���Ǝ��̃L�[�t���[���̎p���̊Ԃ�⊮����

        // Translate�⊮
        DirectX::XMVECTOR translate0 = DirectX::XMLoadFloat3(&node.translate);
        DirectX::XMVECTOR translate1 = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMStoreFloat3(&translate, DirectX::XMVectorLerp(translate0, translate1, blendRate));

        // scale�⊮
        DirectX::XMVECTOR scale0 = DirectX::XMLoadFloat3(&node.scale);
        DirectX::XMVECTOR scale1 = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMStoreFloat3(&scale, DirectX::XMVectorLerp(scale0, scale1, blendRate));

        // Rotate�⊮�@�N�H�[�^�j�I��
        DirectX::XMVECTOR rotate0 = DirectX::XMLoadFloat4(&node.rotate);
        DirectX::XMVECTOR rotate1 = DirectX::XMLoadFloat4(&key0.rotate);

        DirectX::XMStoreFloat4(&rotate, DirectX::XMQuaternionSlerp(rotate0, rotate1, blendRate));
      }

      // �ʏ�̌v�Z ( �����A�j���[�V���� ) 
      else
      {
        // �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����@�i 0 �` 1 �ȓ��j
        float rate = (currentAnimeSeconds[ANIM_AREA::BOTTOM] - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

        // �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
        // Translate�⊮
        DirectX::XMVECTOR key0translate = DirectX::XMLoadFloat3(&key0.translate);
        DirectX::XMVECTOR key1translate = DirectX::XMLoadFloat3(&key1.translate);

        DirectX::XMStoreFloat3(&translate, DirectX::XMVectorLerp(key0translate, key1translate, rate));

        // scale�⊮
        DirectX::XMVECTOR key0scale = DirectX::XMLoadFloat3(&key0.scale);
        DirectX::XMVECTOR key1scale = DirectX::XMLoadFloat3(&key1.scale);
        DirectX::XMStoreFloat3(&scale, DirectX::XMVectorLerp(key0scale, key1scale, rate));

        // Rotate�⊮�@�i�N�H�[�^�j�I���j
        DirectX::XMVECTOR key0rotate = DirectX::XMLoadFloat4(&key0.rotate);
        DirectX::XMVECTOR key1rotate = DirectX::XMLoadFloat4(&key1.rotate);

        DirectX::XMStoreFloat4(&rotate, DirectX::XMQuaternionSlerp(key0rotate, key1rotate, rate));
      }

      // �L�[�t���[���̎p����1�ڂ̃A�j���[�V����( Bottom )�̎p���ƍ���
      node.translate = Mathf::Lerp(node.translate, translate, animeBlendRate);
      node.scale = Mathf::Lerp(node.scale, scale, animeBlendRate);
      node.rotate = Mathf::Slerp(node.rotate, rotate, animeBlendRate);
    }
    break;
  }
}

bool ModelAnimator::CheckEvent(const ANIM_AREA& area, const ANIMATION_EVENT& animEvent, const int& index) const
{
  // �Đ�������Ȃ����,�G���A�������g2��������,��~����
  if (IsPlayAnimation(area) == false)return false;
  assert(area != ANIM_AREA::BOTTOM_SUB);

  int currentFrame = static_cast<int>(currentAnimeSeconds[area] * 60.0f);
  const std::vector<ModelResource::Animation::Event>& events = model->resource->GetAnimations().at(currentAnimeIndex[area]).events;

  // �Ή�����C�x���g��T��
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
  // �Đ����łȂ��Ȃ珈�����Ȃ�
  if (!IsPlayAnimation(area))return;

  // �w��̃A�j���[�V�����f�[�^���擾
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[area]);

  // �O�t���[���̃A�j���[�V�������Ԃ�ۑ� ( ���[�g���[�V�����p )
  if (area == ANIM_AREA::BOTTOM) {
    oldAnimationSecond = currentAnimeSeconds[area];
  }

  // ���Ԍo��
  currentAnimeSeconds[area] += elapsedTime * animeSpeed[area];

  // �Đ����Ԃ��I�[���Ԃ𒴂�����
  if (currentAnimeSeconds[area] >= animation.secondsLength)
  {
    // �A�j���[�V�����̃��[�v�t���O
    if (animeLoopFlag[area])
    {
      // �Đ����Ԃ������߂�
      currentAnimeSeconds[area] -= animation.secondsLength;
    }
    else
    {
      // �A�j���[�V�����̏I��
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

  // ����A�O��A����̃��[�g���[�V�����m�[�h�̎p�����擾
  DirectX::XMFLOAT3 beginPos, oldPos, newPos;
  beginPos = CalculateRootNodePos(0.0f);
  oldPos = CalculateRootNodePos(oldAnimationSecond);
  newPos = CalculateRootNodePos(currentAnimeSeconds[ANIM_AREA::BOTTOM]);

  // ���[�J����Ԃō��������߂�
  DirectX::XMFLOAT3 localTranslation = {};

  // ���݂̃A�j���[�V�������Ԃ��A�O�̃t���[���̃A�j���[�V�������Ԃ�菬�����ꍇ ( ���[�v�A�j���[�V���� )
  if (currentAnimeSeconds[ANIM_AREA::BOTTOM] < oldAnimationSecond)
  {
    // �I�[�̎p�������o��
    DirectX::XMFLOAT3 endPos;
    // float�덷���l�����A0.001f�������炷
    endPos = CalculateRootNodePos(animation.secondsLength - 0.001f);

    // �O��->�I�[�̈ړ������l���Z�o����
    DirectX::XMFLOAT3 old_endTranslation, begin_newTranslation;
    old_endTranslation = endPos - oldPos;

    // ����->����̈ړ������l���Z�o����
    begin_newTranslation = newPos - beginPos;

    // �Z�o�����ړ������l�̍��v�l�����[�J���ړ������l�Ƃ��Ĉ���
    localTranslation = old_endTranslation + begin_newTranslation;
  }
  else
  {
    localTranslation = newPos - oldPos;
  }

  // ���[�g���[�V�����m�[�h������̎p���ɂ���
  CompModel::Node& node = model->nodes[rootNodeId];
  node.translate = beginPos;

  // ���[���h��Ԃł̈ړ��l�����߂�
  ObjectTransform& transform = model->gameObject.lock()->transform;
  DirectX::XMFLOAT3 worldTranslation;
  DirectX::XMVECTOR W = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&localTranslation), transform.GetWorldTransform());
  DirectX::XMStoreFloat3(&worldTranslation, W);

  // ���[�g���[�V�����̉e���x
  float rootMotionScale = DEFAULT_MOTION_SCALE;
  auto motionParam = model->GetResource()->GetCharacterData().rootMotionParams;
  if (motionParam.find(currentAnimeIndex[ANIM_AREA::BOTTOM]) != motionParam.end()) {
    rootMotionScale = motionParam.at(currentAnimeIndex[ANIM_AREA::BOTTOM]).motionScale;
  }

  worldTranslation *= rootMotionScale;

  // �ړ��l�𔽉f
  if (animation.applyAxisUpFlg)
  {
    transform.position += worldTranslation;
  }
  else
  {
    // Y���͖���
    transform.position.x += worldTranslation.x;
    transform.position.z += worldTranslation.z;
  }
}

bool ModelAnimator::CheckRootMotion()
{
  // �A�j���[�V�������Đ����Ă��Ȃ�
  if (!IsPlayAnimation(ANIM_AREA::BOTTOM))return false;
  // ���[�g�m�[�h���ݒ肳��Ă��Ȃ�
  if (rootNodeId < 0)return false;

  // ���[�g���[�V������K�p���Ȃ��A�j���[�V�����̏ꍇ
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[ANIM_AREA::BOTTOM]);
  if (animation.rootMotionFlg == false)return false;

  return true;
}

DirectX::XMFLOAT3 ModelAnimator::CalculateRootNodePos(const float& animationTime)
{
  // �w��̃A�j���[�V�����f�[�^���擾
  const std::vector<ModelResource::Animation>& animations = model->resource->GetAnimations();
  const ModelResource::Animation& animation = animations.at(currentAnimeIndex[ANIM_AREA::BOTTOM]);

  // �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
  const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
  // �L�[�t���[���̐����擾
  int keyCount = (int)keyframes.size();

  // ���[�g�m�[�h�̎p�������߂�
  for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
  {
    // ���Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
    const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
    const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

    // �A�j���[�V�����o�ߎ��Ԃ���̃L�[�t���[���̊Ԃ���Ȃ���Ύ��̃L�[�t���[����
    if (animationTime < keyframe0.seconds ||
      animationTime > keyframe1.seconds)
      continue;

    // �L�[�t���[���̎p���ɂȂ�悤�ɕ⊮����
    {
      // 2�̃L�[�t���[���Ԃ̕⊮�v�Z
      // �L�[�t���[���ɓo�^����Ă���m�[�h�����擾�i�p���f�[�^�j
      const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(rootNodeId);
      const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(rootNodeId);

      // �ʏ�̌v�Z
      // �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����i 0 �` 1 �ȓ��j
      float rate = (animationTime - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

      // �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
      // Translate�⊮
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

// �A�j���[�V�����Đ�
void ModelAnimator::PlayAnimation(const ANIM_AREA& area, const int& index, const bool& isLoop, const float& blendSeconds)
{
  // �A�j���[�V�����ԍ�
  currentAnimeIndex[area] = index;
  currentAnimeSeconds[area] = 0.0f;

  // ���[�v�ݒ�
  animeLoopFlag[area] = isLoop;
  animeEndFlag[area] = false;

  // �A�j���[�V�����J�ڕ⊮�ݒ�
  animeBlendTimer[area] = 0.0f;
  animeBlendSeconds[area] = blendSeconds;

  oldAnimationSecond = 0.0f;

  animeSpeed[area] = 1.0f;
}

void ModelAnimator::PlayAnimationBottomSub(const int& index)
{
  // �A�j���[�V�����ԍ�
  currentAnimeIndex[ANIM_AREA::BOTTOM_SUB] = index;

  // �A�j���[�V�����J�ڕ⊮�ݒ�
  animeBlendTimer[ANIM_AREA::BOTTOM_SUB] = 0.0f;
}

// �A�j���[�V�����Đ�����
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
