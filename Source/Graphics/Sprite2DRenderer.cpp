#include "Sprite2DRenderer.h"
#include "Graphics.h"
#include "Texture.h"
#include "System/misc.h"

#include <algorithm>

void Sprite2DRenderer::Register(CompSprite2D* sprite)
{
  sprites.emplace_back(sprite);

  // シェーダーのIDでソート
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

  // 描画
  for (auto& sprite : sprites) {
    ObjectTransform& transform = sprite->GetGameObject()->transform;

    // スケールが0だったら描画しない
    if (transform.scale.x <= FLT_MIN || transform.scale.y <= FLT_MIN)
    {
      continue;
    }

    // シェーダーの切り替え
    if (sprite->GetShaderID() != currentShaderID) {
      shader->End(dc);

      currentShaderID = sprite->GetShaderID();

      shader = graphics.GetShader(currentShaderID);
      shader->Begin(dc, {});
    }

    // 各シェーダーステートを設定
    shader->SetUpStates(dc, sprite->GetBsMode(), sprite->GetDsMode(), sprite->GetRsMode());

    // 描画
    SpriteShader::VertexResource vertexResource = sprite->GetVertexResource();
    shader->Draw(dc, &vertexResource, sprite->GetTexture()->GetShaderResourceView().GetAddressOf());
  }

  shader->End(dc);
}
