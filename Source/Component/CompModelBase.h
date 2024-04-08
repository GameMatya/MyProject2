#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <DirectXMath.h>

#include "Component.h"
#include "Graphics/ModelResourceBase.h"
#include "Graphics/RenderContext/RenderContext.h"

class ModelRenderer;

// モデル
class CompModelBase : public Component, public Task
{
public:
  // ModelResourceのデータから当たり判定コンポーネントを追加
  virtual void AddCompCollisions() = 0;

  // 描画パラメーターを設定する関数を登録
  void SetRenderFunction(std::function<void(RenderContext&)> func) { pushRenderContext = func; }

  // 描画パラメーターを設定する関数
  void PushRenderContext(RenderContext& rc) {
    if (pushRenderContext != nullptr)
      pushRenderContext(rc);
  }

protected:
  void AddCollisionsFromResource(ModelResourceBase* resource);
  virtual void SettingNodeCollision(Collider3D* collider, const ModelResourceBase::Collision::Collider& collision) {}

private:
  std::function<void(RenderContext&)> pushRenderContext = nullptr; // 描画パラメーターを設定する関数のポインタ

};
