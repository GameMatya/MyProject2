#include "Graphics/Graphics.h"
#include "EffectManager.h"
#include "Camera/Camera.h"

std::shared_ptr<Effect> EffectManager::LoadEffekseer(const char* filename) 
{
  std::string name = filename;
  EffectMap::const_iterator it = effectMap.find(name);
  // モデル検索
  if (effectMap.find(name) != effectMap.end()) {
    return (*effectMap.find(name)).second;
  }

  // 新規モデルリソースの作成・読み込み
  std::shared_ptr<Effect> effect = std::make_shared<Effect>(filename);

  // マップに登録
  effectMap.emplace(filename, effect);

  return effect;
}

// 初期化
void EffectManager::Initialize()
{
  Graphics& graphics = Graphics::Instance();

  // Effekseerレンダラ生成
  effekseerRenderer = EffekseerRendererDX11::
    Renderer::Create(graphics.GetDevice(), graphics.GetDeviceContext(), 2048);

  // Effekseerマネージャー生成
  effekseerManager = Effekseer::Manager::Create(2048);

  // Effekseerレンダラの各種設定
  effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
  effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
  effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
  effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
  effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
  effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
  effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
  effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

  // Effekseerを左手座標系で計算する
  effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// 更新処理
void EffectManager::Update(float elapsedTime)
{
  // エフェクト更新処理( 引数に経過時間を渡す)
  effekseerManager->Update(elapsedTime * 60.0f);
}

// 描画処理
void EffectManager::Render()
{
  Camera& camera = Camera::Instance();

  // ビュー＆プロジェクション行列をEffekseerレンダラに設定
  effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera.GetView()));
  effekseerRenderer->SetProjectionMatrix
  (*reinterpret_cast<const Effekseer::Matrix44*>(&camera.GetProjection()));

  // Effekseer描画開始
  effekseerRenderer->BeginRendering();

  // Effeseer描画実行
  effekseerManager->Draw();

  // Effekseer描画終了
  effekseerRenderer->EndRendering();

}