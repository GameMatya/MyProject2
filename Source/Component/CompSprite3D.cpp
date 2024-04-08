#include "CompSprite3D.h"
#include "System/Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Graphics/Sprite3DRenderer.h"

#include "imgui.h"

CompSprite3D::CompSprite3D(const char* filepath, Sprite3DRenderer* renderer) :CompSpriteBase(filepath), renderer(renderer)
{
}

void CompSprite3D::Start()
{
  renderer->Register(this);

  Update(0);
}

void CompSprite3D::Update(const float& elapsedTime)
{
  // スクリーン座標は0
  CalcAffineTransform({});
}
