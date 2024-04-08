#pragma once

#include "SceneSystem/Scene.h"
#include "GameObject/GameObject.h"

// ���U���g�V�[��
class SceneResult :public Scene
{
public:
  SceneResult() {}
  ~SceneResult() override {}

  // ������
  void InitializeGameObjects() override;
  void Initialize() override {};

  void Finalize()override;

  // �X�V����
  void Update(const float& elapsedTime) override;

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  // �A�Ŗh�~�p
  const float FIXED_TIME = 1.0f;
  float fixedTimer = 0.0f;

  //�I�[�f�B�I
  std::unique_ptr<AudioSource> bgm = nullptr;

};