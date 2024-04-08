#pragma once

#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>

#include "Task/Task.h"

#include "ObjectTransform.h"

// 前方宣言
class Component;
class GameObjectManager;

using WeakObject = std::weak_ptr<GameObject>;
using SharedObject = std::shared_ptr<GameObject>;

// アクター
class GameObject : public std::enable_shared_from_this<GameObject>, public Task
{
public:
  GameObject(TASK_LAYER layer, GameObjectManager* manager) :layer(layer), objectManager(manager), transform(this) {}
  ~GameObject();

  // 生成した際に一度だけ呼ばれる
  void Start();

  // 毎フレーム呼ばれる
  void Update(const float& elapsedTime) override;

  void OnDestroy();

  // GUI表示
  void DrawImGui();

  // 指定した位置に向く
  void LookAt(const DirectX::XMFLOAT3& lookPosition);

  // 自身を破棄
  void Destroy();

#pragma region ゲッター・セッター
  // スレッドレイヤーの取得
  bool GetIsDestroy() { return isDestroy; }

  // スレッドレイヤーの取得
  const TASK_LAYER& GetLayer() const { return layer; }

  // マネージャー取得
  GameObjectManager* GetObjectManager() { return objectManager; }

  // 押し出しの強さの設定
  void SetPushPower(const float& pushPower) { this->pushPower = pushPower; }
  // 押し出しの強さの取得
  const float& GetPushPower() const { return pushPower; }

  // CompModelコンポーネントで使用
  void SetHaveModel() { haveCompModel = true; }

  // 名前の設定
  void SetName(const char* name) { this->name = name; }
  // 名前の取得
  const char* GetName() const { return name.c_str(); }

  // コンポーネント追加
  template<class T, class... Args>
  std::shared_ptr<T> AddComponent(Args... args)
  {
    std::shared_ptr<T> component = std::make_shared<T>(args...);
    component->SetGameObject(shared_from_this());
    startComponents.emplace_back(component);
    return component;
  }
  // コンポーネント取得
  template<class T>
  std::shared_ptr<T> GetComponent()
  {
    for (std::shared_ptr<Component>& component : components)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    for (std::shared_ptr<Component>& component : startComponents)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    return nullptr;
  }
  template<class T>
  std::shared_ptr<T> GetComponentConst() const
  {
    for (const std::shared_ptr<Component>& component : components)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    for (const std::shared_ptr<Component>& component : startComponents)
    {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
      if (p == nullptr) continue;
      return p;
    }
    return nullptr;
  }

#pragma endregion

public:
  // 姿勢行列
  ObjectTransform transform;

private:
  // 破棄予定かどうか
  bool isDestroy = false;

  GameObjectManager* objectManager;

  std::string			name;

  // 当たり判定の押し出しの強さ
  float pushPower = 1;

  // CompModelコンポーネントを持っているか
  bool haveCompModel = false;

  std::vector<std::shared_ptr<Component>>	startComponents;
  std::vector<std::shared_ptr<Component>>	components;

  // マルチスレッドでのレイヤー番号
  TASK_LAYER layer;

  // デバッグ用変数
#ifdef _DEBUG
  DirectX::XMFLOAT3 axis = { 0,1,0 }; // 回転軸
  float rotate = 0; // 回転量

#endif // _DEBUG

};
