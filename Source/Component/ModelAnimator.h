#pragma once
#include "Math/OriginMath.h"
#include "Graphics/ModelResource.h"

#include <array>
#include <string>

class CompModel;

class ModelAnimator
{
public:
  enum ANIM_AREA
  {
    BOTTOM,     // �����g
    UPPER,      // �㔼�g

    BOTTOM_SUB, // �����g2

    MAX
  };

public:
  ModelAnimator(CompModel* model) :model(model) {}

  // �A�j���[�V�����̍X�V
  void UpdateAnimations(const float& elapsedTime);

  void DrawImGui();

  // �w�肵���A�j���[�V�����̔ԍ���T��
  int FindAnimationId(const std::string& name);

  // �A�j���[�V�����Đ�
  void PlayAnimation(const ANIM_AREA& bodyArea, const int& index, const bool& isLoop, const float& blendSeconds = 0.2f); // �S�g
  void PlayAnimationBottomSub(const int& index); // �����g2

  // �����g1�Ɖ����g2�̃u�����h��
  void SetBottomBlendRate(const float& rate) { animeBlendRate = rate; }

  // ���̃m�[�h�ԍ�
  void SetSplitID(const int& spineNodeId);

  // �A�j���[�V�����Đ����x�ݒ�
  void SetAnimationSpeed(const ANIM_AREA& bodyArea, const float& speed) { animeSpeed[bodyArea] = (std::max)(speed, 0.0f); }

  // ���[�v�t���O�̐ݒ�
  void SetAnimationLoop(const ANIM_AREA& bodyArea, const bool& isLoop) { animeLoopFlag[bodyArea] = isLoop; }

  // �A�j���[�V�����Đ�����
  bool IsPlayAnimation(const ANIM_AREA& bodyArea) const;

  // ���݂̃A�j���[�V�����Đ����Ԏ擾
  float GetCurrentAnimationSeconds(const ANIM_AREA& bodyArea) const { return currentAnimeSeconds[bodyArea]; }

  // ���݂̃A�j���[�V�����Đ�����
  float GetAnimationRate(const ANIM_AREA& bodyArea) const;

  // ���݂̃A�j���[�V�����ԍ��擾
  int GetCurrentAnimationId(const ANIM_AREA& area)const { return currentAnimeIndex[area]; }

  // ���݂̃A�j���[�V�����Đ����x�擾
  float GetAnimationSpeed(const ANIM_AREA& bodyArea)const { return animeSpeed[bodyArea]; }

  bool CheckEvent(const ANIM_AREA& area, const ANIMATION_EVENT& animEvent)const;

private:
  // �A�j���[�V�����X�V����
  void UpdateAnimation(const float& elapsedTime, const ANIM_AREA& bodyArea);

  // �u�����h�A�j���[�V�����X�V����
  void UpdateAnimationBottomSub(const float& elapsedTime);

  // �A�j���[�V�����Đ����Ԃ̍X�V
  void UpdateAnimationSecond(const float& elapsedTime, const ANIM_AREA& bodyArea);

  // ���[�g���[�V������K�p
  void ApplyRootMotion();

  // ����̃A�j���[�V�������Ԃł̃��[�g�m�[�h�̎p�������߂�
  DirectX::XMFLOAT3 CalculateRootNodePos(const float& animationTime);

private:
  CompModel* model = nullptr;
  int splitNodeId = -1; // ���̃m�[�h�ԍ�
  int rootNodeId = 9;  // ���[�g�m�[�h�̔ԍ�

  std::array<int, ANIM_AREA::MAX>       currentAnimeIndex = { -1,-1,-1 };  // �Đ����̃A�j���[�V�����ԍ�
  std::array<float, ANIM_AREA::MAX - 1> currentAnimeSeconds = {};          // �A�j���[�V�����̍Đ�����
  std::array<float, ANIM_AREA::MAX - 1> animeSpeed = { 1,1 };              // �A�j���[�V�����̍Đ����x

  // ���[�g���[�V�����p
  float oldAnimationSecond = 0.0f; // �O�t���[���̃A�j���[�V��������
  bool  bakeTranslationY = false;  // Y���̈ړ��ʂ𔽉f���邩�t���O

  // �����V���b�g�A�j���[�V�����ϐ�
  std::array<bool, ANIM_AREA::MAX - 1>  animeLoopFlag = {};
  std::array<bool, ANIM_AREA::MAX - 1>  animeEndFlag = {};

  // �u�����h�⊮
  float animeBlendRate = 0.5f; // �u�����h�A�j���[�V�����̃u�����h�� ( 0 ~ 1 )
  std::array<float, ANIM_AREA::MAX>     animeBlendTimer = {};   // �⊮���v�Z�p�^�C�}�[
  std::array<float, ANIM_AREA::MAX - 1> animeBlendSeconds = {}; // �⊮���������鎞��

};
