#pragma once

#include "Component.h"
#include "GameObject/GameObject.h" 
#include "CompPlayer.h"
#include "StateMachine/StateMachine.h"

class CompCameraPlayer :public Component
{
public:
  // X�����̒ǂ������鑬�x ( �� 0 ~ 1 �x )
  inline static const float       CHASE_SPEED_X = 0.45f;
  // Y�����̒ǂ������鑬�x ( �� 0 ~ 1 �x )
  inline static const float       CHASE_SPEED_Y = 0.06f;
  // Z�����̒ǂ������鑬�x ( �� 0 ~ 1 �x )
  inline static const float       CHASE_SPEED_Z = 0.05f;

  enum class CAMERA_STATE {
    FOLLOW,
    ROCK_ON
  };

public:
  CompCameraPlayer(std::weak_ptr<CompPlayer> player) :player(player) {}

  void Start()override;

  void Update(const float& elapsedTime)override;

  const char* GetName() const override { return "Camera Player"; };

  DirectX::XMFLOAT3 CalcCameraUp();

  std::weak_ptr<CompPlayer> GetPlayer() { return player; }
  DirectX::XMFLOAT3 GetPlayerPos() { return player.lock()->GetGameObject()->transform.position + DirectX::XMFLOAT3(0.0f, OFFSET, 0.0f); }
  const DirectX::XMFLOAT3& GetOldPlayerPos() const { return oldPlayerPosition; }
  const float& GetDistance() const { return distance; }
  StateMachine<CompCameraPlayer, CAMERA_STATE>& GetStateMachine() { return cameraSM; }

private:
  void UpdateDistance();

private:
  std::weak_ptr<CompPlayer> player;
  StateMachine<CompCameraPlayer, CAMERA_STATE> cameraSM;

  DirectX::XMFLOAT3 oldPosition = {};
  DirectX::XMFLOAT3 oldPlayerPosition = {};
  DirectX::XMFLOAT3 cameraUp = { 0.0f, 1.0f, 0.0f };

  float slopeApplyRate = 0.0f;
  float distance = 6.0f;

  const float SLOPE_RATE_MOVE = 0.04f;
  const float SLOPE_RATE_BOOST = 0.1f;

  // �����_�̍�������
  inline static const float       OFFSET = 2.0f;
  // �I�u�W�F�N�g����J�����ւ̋��� ( �ʏ펞 )
  inline static const float       DISTANCE = 6.0f;
  // �I�u�W�F�N�g����J�����ւ̋��� ( �ߐڍU���� )
  inline static const float       DISTANCE_SLASH = 12.0f;

  // �I�u�W�F�N�g��ǂ������鑬�x
  inline static const float       CHASE_SPEED = 3.0f;

};
