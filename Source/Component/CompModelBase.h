#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <DirectXMath.h>

#include "Component.h"
#include "Graphics/ModelResourceBase.h"
#include "Graphics/RenderContext/RenderContext.h"

class ModelRenderer;

// ���f��
class CompModelBase : public Component, public Task
{
public:
  // ModelResource�̃f�[�^���瓖���蔻��R���|�[�l���g��ǉ�
  virtual void AddCompCollisions() = 0;

  // �`��p�����[�^�[��ݒ肷��֐���o�^
  void SetRenderFunction(std::function<void(RenderContext&)> func) { pushRenderContext = func; }

  // �`��p�����[�^�[��ݒ肷��֐�
  void PushRenderContext(RenderContext& rc) {
    if (pushRenderContext != nullptr)
      pushRenderContext(rc);
  }

protected:
  void AddCollisionsFromResource(ModelResourceBase* resource);
  virtual void SettingNodeCollision(Collider3D* collider, const ModelResourceBase::Collision::Collider& collision) {}

private:
  std::function<void(RenderContext&)> pushRenderContext = nullptr; // �`��p�����[�^�[��ݒ肷��֐��̃|�C���^

};
