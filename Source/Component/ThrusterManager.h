#pragma once

class ThrusterManager
{
public:
  // スラスター容量の最大値
  static constexpr float THRUSTER_CAPACITY = 100.0f;

public:
  void Update(const float& elapsedTime, const bool& isGround);

  // スラスターを消費
  void QuantitySubtract(const float& value);

  bool IsOverHeat() { return (thrusterQuantity <= 0); }
  const float& GetThrusterQuantity() const { return thrusterQuantity; }

private:
  // スラスターの回復速度 ( 接地状態 )
  const float THRUSTER_HEAL_LAND = 160.0f;
  // スラスターの回復速度 ( 空中状態 )
  const float THRUSTER_HEAL_AIR = 32.0f;
  // オーバーヒート後のスラスターの回復量
  const float THRUSTER_HEAL_OVERHEAT = 48.0f;

  // スラスターが回復し始めるまでのクールタイム
  const float THRUSTER_COOL_TIME = 1.0f;
  // オーバーヒート時のクールタイム
  const float OVERHEAT_COOL_TIME = 1.6f;

  // スラスター残量 ( 0でオーバーヒート )
  float thrusterQuantity = THRUSTER_CAPACITY;
  float thrusterCoolTimer = THRUSTER_COOL_TIME;

  bool isOverHeat = false;

};
