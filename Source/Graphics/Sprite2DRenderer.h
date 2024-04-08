#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <DirectXMath.h>

#include "Component/CompSprite2D.h"

class Sprite2DRenderer
{
private:
  Sprite2DRenderer(const Sprite2DRenderer&)                     = delete;
  Sprite2DRenderer(Sprite2DRenderer&&)                 noexcept = delete;
  Sprite2DRenderer& operator=(Sprite2DRenderer const&)          = delete;
  Sprite2DRenderer& operator=(Sprite2DRenderer&&)      noexcept = delete;

public:
  Sprite2DRenderer() {}
  ~Sprite2DRenderer() { Clear(); }

  void Register(CompSprite2D* sprite);
  void Remove(CompSprite2D* sprite);
  void Clear() { sprites.clear(); }

  void Render(ID3D11DeviceContext* dc);

private:
  std::vector<CompSprite2D*> sprites;

};
