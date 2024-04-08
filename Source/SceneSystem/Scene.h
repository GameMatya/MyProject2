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
  // コピーコンストラクタ防止
  Scene(const Scene&) = delete;
  Scene(Scene&&)  noexcept = delete;
  Scene& operator=(Scene const&) = delete;
  Scene& operator=(Scene&&) noexcept = delete;

public:
  Scene();
  virtual ~Scene() {}

  // 初期化
  virtual void InitializeGameObjects() = 0;
  virtual void Initialize() = 0;

  // 終了化
  virtual void Finalize();

  // 更新処理
  virtual void Update(const float& elapsedTime) = 0;

  // 描画処理
  void Render();

  // 準備完了設定 (マルチスレッド)
  void SetReady() { isReady = true; }

  // 準備完了しているか (マルチスレッド)
  bool IsReady() const { return isReady; }

  // ゲッター
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

  // ポストプロセスのシェーダーコンテナ
  std::vector<PostShader*>  postShaders;
  PostProcessContext        postContext;

private:
  // マルチスレッドの完了フラグ
  bool isReady = false;

};
