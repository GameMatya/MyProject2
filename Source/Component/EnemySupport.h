#pragma once

#include "Component.h"
#include "Component/CompCharacter.h"
#include "GameSystem/ScoreManager.h"

// �G�R���|�[�l���g�̊��N���X
class EnemySupport
{
public:
  void Initialize(WeakObject owner);

  // �v���C���[���G
  bool SearchPlayer();

  // TargetPosition�������_���ɐݒ�
  void SetRandomTarget();

  // �v���C���[�Ɍ������x�N�g��
  DirectX::XMFLOAT3 GetPlayerVec();

  // TargetPosition�Ɍ��������K���x�N�g����Ԃ�
  DirectX::XMFLOAT3 GetTargetDir();
  // TargetPosition�Ɍ�����XZ���ʂ̕����x�N�g��
  DirectX::XMFLOAT3 GetTargetDirXZ();
  // TargetPosition�Ɍ������x�N�g��
  DirectX::XMFLOAT3 GetTargetVec();

  void SetTerritoryPosition(const DirectX::XMFLOAT3& position) { territoryOrigin = position; }
  void SetTerritoryRange(const float& range) { territoryRange = range; }
  void SetTargetPosition(const DirectX::XMFLOAT3& position) { targetPosition = position; }
  void SetSearchRange(const float& range) { searchRange = range; }

  bool IsAlivePlayer();

  WeakObject GetPlayer() { return player; }
  float GetPlayerHeight() { return player.lock()->GetComponent<CompCharacter>()->GetWaistHeight(); }

protected:
  WeakObject owner;

  // �v���C���[�I�u�W�F�N�g�̃|�C���^
  WeakObject player;

  //  �꒣��̒��S�_
  DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
  //  �꒣��͈�
  float territoryRange = 10.0f;
  // �ڕW�n�_
  DirectX::XMFLOAT3 targetPosition = { 0,0,0 };

  // ���G�͈�
  float searchRange = 5.0f;

};
