#include "GameObjectManager.h"
#include "Component/CompCollision.h"
#include "Component/CompTriggerCollision.h"
#include "Component/CompModel.h"

#include "imgui.h"

// �I�u�W�F�N�g����
SharedObject GameObjectManager::Create(TASK_LAYER layer, bool isStage)
{
  SharedObject object = std::make_shared<GameObject>(layer, this);
  {
    static int id = 0;
    char name[256];
    ::sprintf_s(name, sizeof(name), "GameObject%d", id++);
    object->SetName(name);
  }
  // ���I�I�u�W�F�N�g
  if (layer != TASK_LAYER::NONE)
  {
    startObjects.emplace_back(object);
    taskManager.Register(layer, object);
  }
  // �ÓI�I�u�W�F�N�g
  else
  {
    staticObjects.emplace_back(object);
    // �����o���L��̃I�u�W�F�N�g
    if (isStage)
      stageManager.Register(object.get());
  }

  return object;
}

// �����ݒ�
void GameObjectManager::Initialize()
{
  // ���I�I�u�W�F�N�g�̏�������
  for (SharedObject& object : startObjects)
  {
    object->Start();
    objects.emplace_back(object);
  }
  startObjects.clear();

  // �ÓI�I�u�W�F�N�g�̍X�V����
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

// �폜
void GameObjectManager::Remove(SharedObject object)
{
  removeObjects.insert(object);
}

// �X�V
void GameObjectManager::Update(const float& elapsedTime)
{
  // ���I�I�u�W�F�N�g�̏�������
  if (startObjects.empty() == false) {
    for (SharedObject& object : startObjects)
    {
      object->Start();
      objects.emplace_back(object);
    }
    startObjects.clear();
  }

  // �I�u�W�F�N�g�̍X�V���}���`�X���b�h�ōs��
  taskManager.Update(elapsedTime);

  // �I�u�W�F�N�g���m�̉����o������
  Collisions();

  // �j�����X�g�ɓo�^���ꂽ�I�u�W�F�N�g��j�����Ă���
  DiscardObjects();
}

// �`��
void GameObjectManager::RenderGui()
{
  // ���X�^�[�`��
  DrawLister();

  // �ڍו`��
  DrawDetail();
}

// �I�u�W�F�N�g���m�̉����o������
void GameObjectManager::Collisions()
{
  for (int i = 0; i < objects.size(); ++i)
  {
    CompCollision* colliderA = objects.at(i)->GetComponent<CompCollision>().get();

    // �����蔻��R���|�[�l���g��������Ζ���
    if (colliderA == nullptr)continue;

    // ���I�I�u�W�F�N�g�Ƃ̉����o������
    for (int j = i + 1; j < objects.size(); ++j)
    {
      // �����̎q�I�u�W�F�N�g�Ȃ疳��
      if (objects.at(i) == objects.at(j)->transform.GetParentObject().lock())continue;
      if (objects.at(i)->transform.GetParentObject().expired() == false) {
        // �����̐e�I�u�W�F�N�g�Ȃ疳��
        if (objects.at(i)->transform.GetParentObject().lock() == objects.at(j))continue;
        // �����e�̎q�I�u�W�F�N�g�Ȃ疳��
        if (objects.at(i)->transform.GetParentObject().lock() == objects.at(j)->transform.GetParentObject().lock())continue;
      }

      CompCollision* colliderB = objects.at(j)->GetComponent<CompCollision>().get();

      // �����蔻��R���|�[�l���g��������Ζ���
      if (colliderB == nullptr)continue;

      colliderA->Collide(colliderB);
    }

    // �ÓI�I�u�W�F�N�g�Ƃ̉����o������
    for (SharedObject& object : staticObjects)
    {
      CompCollision* colliderB = object->GetComponent<CompCollision>().get();

      // �����蔻��R���|�[�l���g��������Ζ���
      if (colliderB == nullptr)continue;
      colliderA->Collide(colliderB);
    }
  }
}

// �j�����X�g�ɓo�^���ꂽ�I�u�W�F�N�g��j�����Ă���
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

    // ���I�I�u�W�F�N�g�̔j��
    std::vector<SharedObject>::iterator itrObject = std::find(objects.begin(), objects.end(), object);
    if (itrObject != objects.end())
    {
      objects.erase(itrObject);
      taskManager.Remove(object->GetLayer(), object);
      continue;
    }

    // �ÓI�I�u�W�F�N�g�̔j��
    itrObject = std::find(staticObjects.begin(), staticObjects.end(), object);
    if (itrObject != staticObjects.end())
    {
      staticObjects.erase(itrObject);
      continue;
    }

    assert(!"�I�u�W�F�N�g���X�g�ɓo�^����Ă��Ȃ��I�u�W�F�N�g������");
  }
  removeObjects.clear();
}

// ���X�^�[�`��
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
        // �P��I�������Ή����Ă���
        ImGuiIO& io = ImGui::GetIO();
        selectionObjects.clear();
        selectionObjects.insert(object);
      }

      ImGui::TreePop();
    }
  }
  ImGui::End();
}

// �ڍו`��
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
