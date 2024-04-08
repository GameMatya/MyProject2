#pragma once

#include <map>
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "Effect.h"

// エフェクトマネージャー
class EffectManager
{
private:
  EffectManager() {}
  ~EffectManager() {}

public:
  // 唯一のインスタンス取得
  static EffectManager& Instance()
  {
    static EffectManager instance;
    return instance;
  }

  // エフェクトリソース読み込み
  std::shared_ptr<Effect> LoadEffekseer(const char* filename);

  // 初期化
  void Initialize();

  void Clear() { effectMap.clear(); }

  // 更新処理
  void Update(float elapsedTime);

  // 描画処理
  void Render();

  // Effekseerマネージャーの取得
  Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

private:
  Effekseer::ManagerRef effekseerManager;
  EffekseerRenderer::RendererRef effekseerRenderer;

  // リソースコンテナ
  using EffectMap = std::map<std::string, std::shared_ptr<Effect>>;
  EffectMap effectMap;

};
