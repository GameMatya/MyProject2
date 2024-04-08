#include "FontRenderer.h"
#include "Texture.h"
#include "System/misc.h"

#include <algorithm>

FontRenderer::FontRenderer()
{
  shader = std::make_unique<SpriteShader>("Shader/SpriteVS.cso", "Shader/FontPS.cso");
}

void FontRenderer::Register(CompFont* sprite)
{
  fonts.emplace_back(sprite);
}

void FontRenderer::Remove(CompFont* sprite)
{
  std::vector<CompFont*>::iterator it = std::find(fonts.begin(), fonts.end(), sprite);
  if (it == fonts.end())
    return;

  fonts.erase(it);
}

void FontRenderer::Render(ID3D11DeviceContext* dc)
{
  shader->Begin(dc, {});

  // 描画
  for (auto& font : fonts) {
    ObjectTransform& transform = font->GetGameObject()->transform;

    // スケールが0だったら描画しない
    if (transform.scale.x <= FLT_MIN || transform.scale.y <= FLT_MIN)
    {
      return;
    }

    // ブレンドステート、ラスタライザステートを設定
    shader->SetUpStates(dc, font->GetBsMode(), font->GetDsMode(), font->GetRsMode());

    // 描画
    font->Render(dc);
  }

  shader->End(dc);
}

void FontRenderer::EnforceColorAlpha(const float& alpha)
{
  for (auto& font : fonts) {
    font->EnforceColorAlpha(alpha);
  }
}
