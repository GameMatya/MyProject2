#include "GameObjectManager.h"
#include "Component/CompCollision.h"
#include "Component/CompTriggerCollision.h"
#include "Component/CompModel.h"

#include "imgui.h"

// オブジェクト生成
SharedObject GameObjectManager::Create(TASK_LAYER layer, bool isStage)
{
  SharedObject object = std::make_shared<GameObject>(layer, this);
  {
    static int id = 0;
    char name[256];
    ::sprintf_s(name, sizeof(name), "GameObject%d", id++);
    object->SetName(name);
  }
  // 動的オブジェクト
  if (layer != TASK_LAYER::NONE)
  {
    startObjects.emplace_back(object);
    taskManager.Register(layer, object);
  }
  // 静的オブジェクト
  else
  {
    staticObjects.emplace_back(object);
    // 押し出し有りのオブジェクト
    if (isStage)
      stageManager.Register(object.get());
  }

  return object;
}

// 初期設定
void GameObjectManager::Initialize()
{
  // 動的オブジェクトの初期処理
  for (SharedObject& object : startObjects)
  {
    object->Start();
    objects.emplace_back(object);
  }
  startObjects.clear();

  // 静的オブジェクトの更新処理
  for (SharedObject& object : staticObjects)
  {
    object->Start();
    object->Update(0);

    CompModel* model = object->GetComponent<CompModel>().get();
    if (model == nullptr)continue;
    model->Update(0);
  }

  stageManager.Initialize();

  taskManager.Initialize();
}

void GameObjectManager::Finalize()
{
  taskManager.Finalize();

  if (startObjects.empty() == false) {
    for (auto& obj : startObjects) {
      Remove(obj);
    }
  }
  if (objects.empty() == false) {
    for (auto& obj : objects) {
      Remove(obj);
    }
  }
  if (staticObjects.empty() == false) {
    for (auto& obj : staticObjects) {
      Remove(obj);
    }
  }

  DiscardObjects();
}

// 削除
void GameObjectManager::Remove(SharedObject object)
{
  removeObjects.insert(object);
}

// 更新
void GameObjectManager::Update(const float& elapsedTime)
{
  // 動的オブジェクトの初期処理
  if (startObjects.empty() == false) {
    for (SharedObject& object : startObjects)
    {
      object->Start();
      objects.emplace_back(object);
    }
    startObjects.clear();
  }

  // オブジェクトの更新をマルチスレッドで行う
  taskManager.Update(elapsedTime);

  // オブジェクト同士の押し出し処理
  Collisions();

  // 破棄リストに登録されたオブジェクトを破棄していく
  DiscardObjects();
}

// 描画
void GameObjectManager::RenderGui()
{
  // リスター描画
  DrawLister();

  // 詳細描画
  DrawDetail();
}

// オブジェクト同士の押し出し処理
void GameObjectManager::Collisions()
{
  for (int i = 0; i < objects.size(); ++i)
  {
    CompCollision* colliderA = objects.at(i)->GetComponent<CompCollision>().get();

    // 当たり判定コンポーネントが無ければ無視
    if (colliderA == nullptr)continue;

    // 動的オブジェクトとの押し出し判定
    for (int j = i + 1; j < objects.size(); ++j)
    {
      // 自分の子オブジェクトなら無視
      if (objects.at(i) == objects.at(j)->transform.GetParentObject().lock())continue;
      if (objects.at(i)->transform.GetParentObject().expired() == false) {
        // 自分の親オブジェクトなら無視
        if (objects.at(i)->transform.GetParentObject().lock() == objects.at(j))continue;
        // 同じ親の子オブジェクトなら無視
        if (objects.at(i)->transform.GetParentObject().lock() == objects.at(j)->transform.GetParentObject().lock())continue;
      }

      CompCollision* colliderB = objects.at(j)->GetComponent<CompCollision>().get();

      // 当たり判定コンポーネントが無ければ無視
      if (colliderB == nullptr)continue;

      colliderA->Collide(colliderB);
    }

    // 静的オブジェクトとの押し出し判定
    for (SharedObject& object : staticObjects)
    {
      CompCollision* colliderB = object->GetComponent<CompCollision>().get();

      // 当たり判定コンポーネントが無ければ無視
      if (colliderB == nullptr)continue;
      colliderA->Collide(colliderB);
    }
  }
}

// 破棄リストに登録されたオブジェクトを破棄していく
void GameObjectManager::DiscardObjects()
{
  for (const SharedObject& object : removeObjects)
  {
    object->OnDestroy();

#ifdef _Debug
    std::set<SharedObject>::iterator itSelection = selectionObjects.find(object);
    if (itSelection != selectionObjects.end())
    {
      selectionObjects.erase(itSelection);
    }

#endif // _Debug

    // 動的オブジェクトの破棄
    std::vector<SharedObject>::iterator itrObject = std::find(objects.begin(), objects.end(), object);
    if (itrObject != objects.end())
    {
      objects.erase(itrObject);
      taskManager.Remove(object->GetLayer(), object);
      continue;
    }

    // 静的オブジェクトの破棄
    itrObject = std::find(staticObjects.begin(), staticObjects.end(), object);
    if (itrObject != staticObjects.end())
    {
      staticObjects.erase(itrObject);
      continue;
    }

    assert(!"オブジェクトリストに登録されていないオブジェクトがいる");
  }
  removeObjects.clear();
}

// リスター描画
void GameObjectManager::DrawLister()
{
  ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

  hiddenLister = !ImGui::Begin("object Lister", nullptr, ImGuiWindowFlags_None);
  if (!hiddenLister)
  {
    for (SharedObject& object : objects)
    {
      ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

      if (selectionObjects.find(object) != selectionObjects.end())
      {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
      }

      ImGui::TreeNodeEx(object.get(), nodeFlags, object->GetName());

      if (ImGui::IsItemClicked())
      {
        // 単一選択だけ対応しておく
        ImGuiIO& io = ImGui::GetIO();
        selectionObjects.clear();
        selectionObjects.insert(object);
      }

      ImGui::TreePop();
    }
  }
  ImGui::End();
}

// 詳細描画
void GameObjectManager::DrawDetail()
{
  ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

  hiddenDetail = !ImGui::Begin("object Detail", nullptr, ImGuiWindowFlags_None);
  if (!hiddenDetail)
  {
    SharedObject lastSelected = selectionObjects.empty() ? nullptr : *selectionObjects.rbegin();
    if (lastSelected != nullptr)
    {
      lastSelected->DrawImGui();
    }
  }
  ImGui::End();
}
