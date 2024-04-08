#pragma once

#include "Math/OriginMath.h"
#include "GameObject/StageManager.h"

//�J����
class Camera
{
private:
  Camera();
  ~Camera() {}

  // ������̍쐬
  void createFrustum();

public:
  // �B��̃C���X�^���X�擾
  static Camera& Instance()
  {
    static Camera camera;
    return camera;
  }

  // �w��ʒu�Ɍ���
  void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
    const DirectX::XMFLOAT3& up);

  // �w������Ɍ���
  void SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& up);
  void SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT4& direction,
    const DirectX::XMFLOAT3& up);

  // �p�[�X�y�N�e�B�u�ݒ�
  void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

  // �J�����̈ړ� ( ������Lerp�֐� )
  DirectX::XMFLOAT3 MovieMove(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, float timer);

  // �J�����̈ړ����g�������[�r�[����
  bool Movie(DirectX::XMFLOAT3 startEye, DirectX::XMFLOAT3 endEye,
    DirectX::XMFLOAT3 startFocus, DirectX::XMFLOAT3 endFocus,
    float timer, DirectX::XMFLOAT3 cameraAttitude = { 0,1,0 });

  // �X�e�[�W�Ƃ̓����蔻��
  void CollideStage(StageManager& manager, DirectX::XMFLOAT3& eye, DirectX::XMFLOAT3& focus);

  //// ������Ƃ̓����蔻��
  //bool hitFrustum(AABB aabb);
  //// ������Ƃ̓����蔻��( AABB���g��Ȃ��� )
  //bool hitFrustum(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 boxSize) { return hitFrustum({ position ,boxSize }); }

  void debugDraw();

  // �r���[�s��擾
  const DirectX::XMFLOAT4X4& GetView()const { return view; }
  // �v���W�F�N�V�����s��擾
  const DirectX::XMFLOAT4X4& GetProjection()const { return projection; }

  // ���_�擾
  const DirectX::XMFLOAT3& GetEye()const { return eye; }
  // �����_�擾
  const DirectX::XMFLOAT3& GetFocus()const { return focus; }
  // ������擾
  const DirectX::XMFLOAT3& GetUp()const { return up; }
  // �O�����擾
  const DirectX::XMFLOAT3& GetForward()const { return forward; }
  // �E�����擾
  const DirectX::XMFLOAT3& GetRight()const { return right; }

  // ����p�擾
  const	float& GetFovY() { return	fovY; }
  // �A�X�y�N�g��擾
  const	float& GetAspect() { return	aspect; }
  // �߃N���b�v�ʂ܂ł̋������擾
  const	float& GetNearZ() { return	nearZ; }
  // ���N���b�v�ʂ܂ł̋��������擾
  const	float& GetFarZ() { return	farZ; }

private:
  struct Plane
  {
    DirectX::XMFLOAT3	n;	// �@��
    float				d;	// ���_����̍ŒZ����
  };
  struct Frustum
  {
    Plane plane[6] = {};
  };
  Frustum frustum;

  DirectX::XMFLOAT4X4  view = {};
  DirectX::XMFLOAT4X4  projection = {};

  DirectX::XMFLOAT3    eye = {};
  DirectX::XMFLOAT3    focus = {};

  DirectX::XMFLOAT3    up = {};
  DirectX::XMFLOAT3    forward = {};
  DirectX::XMFLOAT3    right = {};

  float					fovY;
  float					aspect;
  float					nearZ;
  float					farZ;

  DirectX::XMFLOAT3	debugLineNorm[4] = {};

};
