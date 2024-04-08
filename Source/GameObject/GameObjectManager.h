#pragma once

#include <memory>
#include <set>
#include "GameObject.h"
#include "Task/TaskManager.h"
#include "StageManager.h"
#include "CharacterManager.h"

class Scene;

// アクターマネージャー
class GameObjectManager
{
public:
  GameObjectManager(Scene* scene) :scene(scene) {}

  // 作成
  SharedObject Create(TASK_LAYER layer, bool isStage = false);

  // 初期化
  void Initialize();

  void Finalize();

  // 削除
  void Remove(SharedObject player);

  // 更新
  void Update(const float& elapsedTime);

  // 描画
  void RenderGui();

  Scene* GetScene() { return scene; }

  // ステージマネージャーの取得
  StageManager& GetStageManager() { return stageManager; }

  // キャラクターマネージャーの取得
  CharacterManager& GetCharacterManager() { return characterManager; }

  // コンポーネントの更新処理内でオブジェクトの生成を行う用
  std::mutex& GetMutex() { return mutex; }

private:
  void Collisions();
  void DiscardObjects();

  void DrawLister();
  void DrawDetail();

private:
  Scene* scene;
  std::mutex mutex;

  // 生成予定の動的オブジェクトの配列
  std::vector<SharedObject>		startObjects;
  // 動的オブジェクトの配列
  std::vector<SharedObject>		objects;
  // 静的オブジェクトの配列
  std::vector<SharedObject>		staticObjects;
  // 削除予定のオブジェクトの配列
  std::set<SharedObject>			removeObjects;

  // 並列処理タスクの管理
  TaskManager       taskManager;
  // キャラクターオブジェクトの管理
  CharacterManager  characterManager;
  // ステージオブジェクトの管理
  StageManager      stageManager;

  // デバッグ用のオブジェクトポインタ
  std::set<SharedObject>				selectionObjects;
  bool				hiddenLister = false;
  bool				hiddenDetail = false;

};
