#include "ThrusterManager.h"
#include <assert.h>
#include <algorithm>

void ThrusterManager::Update(const float& elapsedTime, const bool& isGround)
{
  if (thrusterQuantity == THRUSTER_CAPACITY)return;

  thrusterCoolTimer -= elapsedTime;

  // �N�[���^�C����0�ȉ��̏ꍇ�A�X���X�^�[�����X�ɉ񕜂���
  if (thrusterCoolTimer <= 0) {
    const float& healSpeed = (isGround == true) ? THRUSTER_HEAL_LAND : THRUSTER_HEAL_AIR;
    thrusterQuantity += healSpeed * elapsedTime;
    thrusterQuantity = (std::min)(thrusterQuantity, THRUSTER_CAPACITY);

    // �I�[�o�[�q�[�g�������ۂ݈̂��ʉ񕜂�����
    if (isOverHeat == true) {
      thrusterQuantity += THRUSTER_HEAL_OVERHEAT;
      isOverHeat = false;
    }
  }
}

void ThrusterManager::QuantitySubtract(const float& value)
{
  // �֐����Ăяo���O�ɃI�[�o�[�q�[�g���Ă��邩�̃`�F�b�N�����Ă��邱�Ƃ�O��
  assert(IsOverHeat() == false);

  thrusterQuantity -= value;
  thrusterCoolTimer = THRUSTER_COOL_TIME;

  // �e�ʂ�0�ȉ��̏ꍇ�A0.0f�ɍĐݒ肵�N�[���^�C�����I�[�o�[�q�[�g���̕��ɂ���
  if (thrusterQuantity <= 0) {
    thrusterQuantity = 0.0f;
    thrusterCoolTimer = OVERHEAT_COOL_TIME;
    isOverHeat = true;
  }
}
