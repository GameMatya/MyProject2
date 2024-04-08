#include "Sprite2DRenderer.h"
#include "Graphics.h"
#include "Texture.h"
#include "System/misc.h"

#include <algorithm>

void Sprite2DRenderer::Register(CompSprite2D* sprite)
{
  sprites.emplace_back(sprite);

  // �V�F�[�_�[��ID�Ń\�[�g
  std::sort(sprites.begin(), sprites.end(), [](CompSprite2D* left, CompSprite2D* right) {return left->GetDrawOrder() < right->GetDrawOrder(); });
}

void Sprite2DRenderer::Remove(CompSprite2D* sprite)
{
  std::vector<CompSprite2D*>::iterator it = std::find(sprites.begin(), sprites.end(), sprite);
  if (it == sprites.end())
    return;

  sprites.erase(it);
}

void Sprite2DRenderer::Render(ID3D11DeviceContext* dc)
{
  Graphics& graphics = Graphics::Instance();

  SHADER_ID_SPRITE currentShaderID = SHADER_ID_SPRITE::DEFAULT;
  SpriteShader* shader = graphics.GetShader(currentShaderID);
  shader->Begin(dc, {});

  // �`��
  for (auto& sprite : sprites) {
    ObjectTransform& transform = sprite->GetGameObject()->transform;

    // �X�P�[����0��������`�悵�Ȃ�
    if (transform.scale.x <= FLT_MIN || transform.scale.y <= FLT_MIN)
    {
      continue;
    }

    // �V�F�[�_�[�̐؂�ւ�
    if (sprite->GetShaderID() != currentShaderID) {
      shader->End(dc);

      currentShaderID = sprite->GetShaderID();

      shader = graphics.GetShader(currentShaderID);
      shader->Begin(dc, {});
    }

    // �e�V�F�[�_�[�X�e�[�g��ݒ�
    shader->SetUpStates(dc, sprite->GetBsMode(), sprite->GetDsMode(), sprite->GetRsMode());

    // �`��
    SpriteShader::VertexResource vertexResource = sprite->GetVertexResource();
    shader->Draw(dc, &vertexResource, sprite->GetTexture()->GetShaderResourceView().GetAddressOf());
  }

  shader->End(dc);
}
