#include "ThrusterManager.h"
#include <assert.h>
#include <algorithm>

void ThrusterManager::Update(const float& elapsedTime, const bool& isGround)
{
  if (thrusterQuantity == THRUSTER_CAPACITY)return;

  thrusterCoolTimer -= elapsedTime;

  // クールタイムが0以下の場合、スラスターを徐々に回復する
  if (thrusterCoolTimer <= 0) {
    const float& healSpeed = (isGround == true) ? THRUSTER_HEAL_LAND : THRUSTER_HEAL_AIR;
    thrusterQuantity += healSpeed * elapsedTime;
    thrusterQuantity = (std::min)(thrusterQuantity, THRUSTER_CAPACITY);

    // オーバーヒートだった際のみ一定量回復させる
    if (isOverHeat == true) {
      thrusterQuantity += THRUSTER_HEAL_OVERHEAT;
      isOverHeat = false;
    }
  }
}

void ThrusterManager::QuantitySubtract(const float& value)
{
  // 関数を呼び出す前にオーバーヒートしているかのチェックをしていることを前提
  assert(IsOverHeat() == false);

  thrusterQuantity -= value;
  thrusterCoolTimer = THRUSTER_COOL_TIME;

  // 容量が0以下の場合、0.0fに再設定しクールタイムをオーバーヒート時の物にする
  if (thrusterQuantity <= 0) {
    thrusterQuantity = 0.0f;
    thrusterCoolTimer = OVERHEAT_COOL_TIME;
    isOverHeat = true;
  }
}
