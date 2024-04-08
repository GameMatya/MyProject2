#pragma once

class ThrusterManager
{
public:
  // �X���X�^�[�e�ʂ̍ő�l
  static constexpr float THRUSTER_CAPACITY = 100.0f;

public:
  void Update(const float& elapsedTime, const bool& isGround);

  // �X���X�^�[������
  void QuantitySubtract(const float& value);

  bool IsOverHeat() { return (thrusterQuantity <= 0); }
  const float& GetThrusterQuantity() const { return thrusterQuantity; }

private:
  // �X���X�^�[�̉񕜑��x ( �ڒn��� )
  const float THRUSTER_HEAL_LAND = 160.0f;
  // �X���X�^�[�̉񕜑��x ( �󒆏�� )
  const float THRUSTER_HEAL_AIR = 32.0f;
  // �I�[�o�[�q�[�g��̃X���X�^�[�̉񕜗�
  const float THRUSTER_HEAL_OVERHEAT = 48.0f;

  // �X���X�^�[���񕜂��n�߂�܂ł̃N�[���^�C��
  const float THRUSTER_COOL_TIME = 1.0f;
  // �I�[�o�[�q�[�g���̃N�[���^�C��
  const float OVERHEAT_COOL_TIME = 1.6f;

  // �X���X�^�[�c�� ( 0�ŃI�[�o�[�q�[�g )
  float thrusterQuantity = THRUSTER_CAPACITY;
  float thrusterCoolTimer = THRUSTER_COOL_TIME;

  bool isOverHeat = false;

};
