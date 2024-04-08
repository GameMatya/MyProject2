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

  // �`��
  for (auto& font : fonts) {
    ObjectTransform& transform = font->GetGameObject()->transform;

    // �X�P�[����0��������`�悵�Ȃ�
    if (transform.scale.x <= FLT_MIN || transform.scale.y <= FLT_MIN)
    {
      return;
    }

    // �u�����h�X�e�[�g�A���X�^���C�U�X�e�[�g��ݒ�
    shader->SetUpStates(dc, font->GetBsMode(), font->GetDsMode(), font->GetRsMode());

    // �`��
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
