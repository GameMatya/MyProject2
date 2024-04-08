#include "ModelAnimator.h"
#include "CompModel.h"
#include "imgui.h"
#include <queue>

void ModelAnimator::UpdateAnimations(const float& elapsedTime)
{
  // �S�g�̃A�j���[�V����
  UpdateAnimation(elapsedTime, ANIM_AREA::BOTTOM);

  // �����g2�A�j���[�V����
  UpdateAnimationBottomSub(elapsedTime);

  // �Đ����Ԃ̍X�V
  UpdateAnimationSecond(elapsedTime, ANIM_AREA::BOTTOM);

  // �㔼�g�ɃA�j���[�V�����ԍ������蓖�Ă��Ă�����
  if (currentAnimeIndex[ANIM_AREA::ARM_RIGHT] >= 0 || currentAnimeIndex[ANIM_AREA::ARM_LEFT] >= 0) {
    // �㔼�g�̃A�j���[�V����
    UpdateAnimation(elapsedTime, ANIM_AREA::ARM_RIGHT);
    UpdateAnimation(elapsedTime, ANIM_AREA::ARM_LEFT);

    // �Đ����Ԃ̍X�V
    UpdateAnimationSecond(elapsedTime, ANIM_AREA::ARM_RIGHT);
    UpdateAnimationSecond(elapsedTime, ANIM_AREA::ARM_LEFT);
  }
}

void ModelAnimator::DrawImGui()
{
  if (ImGui::TreeNode("Index")) {
    ImGui::Text((std::string("AnimationID Bottom : ") + std::to_string(currentAnimeIndex[ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("AnimationID BottomSub : ") + std::to_string(currentAnimeIndex[ANIM_AREA::BOTTOM_SUB])).c_str());
    ImGui::Text((std::string("AnimationID Upper : ") + std::to_string(currentAnimeIndex[ANIM_AREA::ARM_RIGHT])).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Speed")) {
    ImGui::Text((std::string("Speed Bottom : ") + std::to_string(animeSpeed[ModelAnimator::ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("Speed Upper : ") + std::to_string(animeSpeed[ModelAnimator::ANIM_AREA::ARM_RIGHT])).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Second")) {
    ImGui::Text((std::string("CurrentSecond Bottom : ") + std::to_string(currentAnimeSeconds[ModelAnimator::ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("CurrentSecond Upper : ") + std::to_string(currentAnimeSeconds[ModelAnimator::ANIM_AREA::ARM_RIGHT])).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Frame")) {
    int currentFrameB = static_cast<int>(currentAnimeSeconds[ModelAnimator::ANIM_AREA::BOTTOM] * 60.0f);
    int currentFrameU = static_cast<int>(currentAnimeSeconds[ModelAnimator::ANIM_AREA::ARM_RIGHT] * 60.0f);

    ImGui::Text((std::string("CurrentFrame Bottom : ") + std::to_string(currentFrameB)).c_str());
    ImGui::Text((std::string("CurrentFrame Upper : ") + std::to_string(currentFrameU)).c_str());

    ImGui::TreePop();
  }
  if (ImGui::TreeNode("Blend")) {
    ImGui::Text((std::string("BlendTime Bottom : ") + std::to_string(animeBlendTimer[ModelAnimator::ANIM_AREA::BOTTOM])).c_str());
    ImGui::Text((std::string("BlendTime Upper : ") + std::to_string(animeBlendTimer[ModelAnimator::ANIM_AREA::ARM_RIGHT])).c_str());

    ImGui::TreePop();
  }

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
  // ������Ȃ���Β�~
  assert(!"Animation Not Found");
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
  if (bodyArea != ANIM_AREA::BOTTOM) {
    startIndex = splitNodeId[bodyArea - 1].x;
    endIndex = splitNodeId[bodyArea - 1].y;
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
        // �r���A�j���[�V��������������
        if (currentAnimeIndex[ANIM_AREA::ARM_RIGHT] >= 0) // �E�r
        {
          // �m�[�h�̍X�V�X�L�b�v
          if (nodeIndex >= splitNodeId[ANIM_AREA::ARM_RIGHT - 1].x &&
            nodeIndex < splitNodeId[ANIM_AREA::ARM_RIGHT - 1].y)
            continue;
        }
        if (currentAnimeIndex[ANIM_AREA::ARM_LEFT] >= 0) // ���r
        {
          if (nodeIndex >= splitNodeId[ANIM_AREA::ARM_LEFT - 1].x &&
            nodeIndex < splitNodeId[ANIM_AREA::ARM_LEFT - 1].y)
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
      // �r���A�j���[�V��������������
      if (currentAnimeIndex[ANIM_AREA::ARM_RIGHT] >= 0 || currentAnimeIndex[ANIM_AREA::ARM_LEFT] >= 0)
      {
        // �m�[�h�̍X�V�X�L�b�v
        if (nodeIndex >= splitNodeId[ANIM_AREA::ARM_RIGHT - 1].x &&
          nodeIndex < splitNodeId[ANIM_AREA::ARM_RIGHT - 1].y)
          continue;

        else if (nodeIndex >= splitNodeId[ANIM_AREA::ARM_LEFT - 1].x &&
          nodeIndex < splitNodeId[ANIM_AREA::ARM_LEFT - 1].y)
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

bool ModelAnimator::CheckEvent(const ANIM_AREA& area, const ANIMATION_EVENT& animEvent) const
{
  // �Đ�������Ȃ����,�G���A�������g2��������,��~����
  if (IsPlayAnimation(area) == false)return false;
  assert(area != ANIM_AREA::BOTTOM_SUB);

  int currentFrame = static_cast<int>(currentAnimeSeconds[area] * 60.0f);
  const std::vector<ModelResource::Animation::Event>& events = model->resource->GetAnimations().at(currentAnimeIndex[area]).events;

  // �Ή�����C�x���g��T��
  std::queue<ModelResource::Animation::Event> eventQueue;
  for (auto& itr : events) {
    if (itr.type == animEvent)
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
      return;
    }
  }
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

  animeSpeed[area] = 1.0f;
}

void ModelAnimator::PlayAnimationBottomSub(const int& index)
{
  // �A�j���[�V�����ԍ�
  currentAnimeIndex[ANIM_AREA::BOTTOM_SUB] = index;

  // �A�j���[�V�����J�ڕ⊮�ݒ�
  animeBlendTimer[ANIM_AREA::BOTTOM_SUB] = 0.0f;
}

void ModelAnimator::SetSplitID(const ANIM_AREA& armArea, const int& armJoint, const int& armTip)
{
  this->splitNodeId[armArea - 1] = { armJoint ,armTip };
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
