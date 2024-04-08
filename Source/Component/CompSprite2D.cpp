#include "CompSprite2D.h"
#include "System/Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/Sprite2DRenderer.h"
#include "System/framework.h"

#include "imgui.h"

CompSprite2D::CompSprite2D(Sprite2DRenderer* renderer, const char* filename) :CompSpriteBase(filename), renderer(renderer)
{
}

void CompSprite2D::Start()
{
  renderer->Register(this);

  // DebugとRelease時のウィンドウサイズの差を埋める
#ifndef _DEBUG
  ObjectTransform& transform = GetGameObject()->transform;
  transform.position.x *= SCREEN_DIFFERENCE_SCALE;
  transform.position.y *= SCREEN_DIFFERENCE_SCALE;

#else
  ObjectTransform& transform = GetGameObject()->transform;
  transform.scale /= SCREEN_DIFFERENCE_SCALE;

#endif // _DEBUG

  Update(0);
}

void CompSprite2D::Update(const float& elapsedTime)
{
  DirectX::XMFLOAT3 position = gameObject.lock()->transform.position;
  CalcAffineTransform({ position.x,position.y });
}

void CompSprite2D::OnDestroy()
{
  renderer->Remove(this);
}

void CompSprite2D::DrawImGui()
{
  CompSpriteBase::DrawImGui();

  ImGui::InputInt("DrawOrder", &drawOrder, 1);
}

