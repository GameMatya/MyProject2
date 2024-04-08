#pragma once

#include <d3d11.h>
#include <imgui.h>
#include <memory>

#include "Graphics/ModelRenderer.h"
#include "Graphics/FontRenderer.h"
#include "Graphics/Sprite2DRenderer.h"
#include "Graphics/Sprite3DRenderer.h"
#include "Graphics/post_process/PostShader.h"
#include "Graphics/RenderContext/PostProcessContext.h"
#include "Graphics/Light.h"

#include "GameObject/GameObjectManager.h"
#include "Audio/Audio.h"
#include "Audio/AudioSource.h"

class Scene
{
private:
  // �R�s�[�R���X�g���N�^�h�~
  Scene(const Scene&) = delete;
  Scene(Scene&&)  noexcept = delete;
  Scene& operator=(Scene const&) = delete;
  Scene& operator=(Scene&&) noexcept = delete;

public:
  Scene();
  virtual ~Scene() {}

  // ������
  virtual void InitializeGameObjects() = 0;
  virtual void Initialize() = 0;

  // �I����
  virtual void Finalize();

  // �X�V����
  virtual void Update(const float& elapsedTime) = 0;

  // �`�揈��
  void Render();

  // ���������ݒ� (�}���`�X���b�h)
  void SetReady() { isReady = true; }

  // �����������Ă��邩 (�}���`�X���b�h)
  bool IsReady() const { return isReady; }

  // �Q�b�^�[
  std::vector<PostShader*>& GetPostShaders() { return postShaders; }
  PostProcessContext& GetPostContext() { return postContext; }
  ModelRenderer& GetModelRenderer() { return modelRenderer; }
  Sprite2DRenderer& GetSprite2DRenderer() { return sprite2DRenderer; }
  Sprite3DRenderer& GetSprite3DRenderer() { return sprite3DRenderer; }
  GameObjectManager& GetGameObjectManager() { return objectManager; }

protected:
#ifdef _DEBUG
  virtual void ImGuiRender() = 0;

  void SaveTextureFile();

#endif // _DEBUG

protected:
  GameObjectManager objectManager;
  ModelRenderer     modelRenderer;
  FontRenderer      fontRenderer;
  Sprite2DRenderer  sprite2DRenderer;
  Sprite3DRenderer  sprite3DRenderer;

  // �|�X�g�v���Z�X�̃V�F�[�_�[�R���e�i
  std::vector<PostShader*>  postShaders;
  PostProcessContext        postContext;

private:
  // �}���`�X���b�h�̊����t���O
  bool isReady = false;

};
