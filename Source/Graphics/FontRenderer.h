#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>

#include "Component/CompFont.h"
#include "Graphics/Shaders/SpriteShader.h"

class FontRenderer
{
private:
  FontRenderer(const FontRenderer&) = delete;
  FontRenderer(FontRenderer&&)                 noexcept = delete;
  FontRenderer& operator=(FontRenderer const&) = delete;
  FontRenderer& operator=(FontRenderer&&)      noexcept = delete;

public:
  FontRenderer();
  ~FontRenderer() { Clear(); }

  void Register(CompFont* sprite);
  void Remove(CompFont* sprite);
  void Clear() { fonts.clear(); }

  void Render(ID3D11DeviceContext* dc);

  // 元の透明値はそのままに透明値を強制的に変更
  void EnforceColorAlpha(const float& alpha);

private:
  std::vector<CompFont*> fonts;
  std::unique_ptr<SpriteShader> shader = nullptr;


};
