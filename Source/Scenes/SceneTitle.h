#pragma once

#include "SceneSystem/Scene.h"
#include "GameObject/GameObject.h"

// �^�C�g���V�[��
class SceneTitle :public Scene
{
public:
  SceneTitle() {}
  ~SceneTitle() override {}

  // ������
  void InitializeGameObjects() override;
  void Initialize() override;

  void Finalize()override;

  // �X�V����
  void Update(const float& elapsedTime) override;

#ifdef _DEBUG
  void ImGuiRender() override;

#endif // _DEBUG

private:
  bool pushEffect = false;

  int transitionState = 0;

  // �X�^�[�g�{�^�����������ۂ̉��o�X�v���C�g
  std::weak_ptr<CompBooleanSprite> effectSprite;

  //�I�[�f�B�I
  std::unique_ptr<AudioSource> bgm = nullptr;

};