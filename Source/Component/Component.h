#pragma once

#include <memory>
#include "GameObject/GameObject.h"

// コンポーネント
class Component
{
public:
  Component() {}
  virtual ~Component() {}

  // 名前取得
  virtual const char* GetName() const = 0;

  // 生成した際に一度だけ実行
  virtual void Start() {}

  // 毎フレーム実行
  virtual void Update(const float& elapsedTime) {}

  // 破棄する前に実行
  virtual void OnDestroy() {}

  // GUI描画
  virtual void DrawImGui() {}

  // コンポーネントの持ち主を設定
  void SetGameObject(SharedObject gameObject) { this->gameObject = gameObject; }

  void SetActive(const bool& active) { isActive = active; }

  const bool& GetActive() const { return isActive; }

  // コンポーネントの持ち主を取得
  SharedObject GetGameObject() { return this->gameObject.lock(); }

protected:
  // コンポーネントの持ち主
  WeakObject	gameObject;

  bool isActive = true;

};
