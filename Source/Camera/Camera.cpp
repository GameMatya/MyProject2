#include <algorithm>
#include "Camera/Camera.h"
#include "Graphics/Graphics.h"

using namespace DirectX;

Camera::Camera()
{
}

void Camera::createFrustum()
{
  // �r���[�v���W�F�N�V�����s����擾����
  XMMATRIX matrix = {};
  XMMATRIX viewMat = XMLoadFloat4x4(&view);
  XMMATRIX projMat = XMLoadFloat4x4(&projection);
  matrix = viewMat * projMat;

  // �r���[�v���W�F�N�V�����s��̋t�s��
  XMMATRIX inv_matrix = XMMatrixInverse(nullptr, matrix);

  // �r���[�v���W�F�N�V�������̒��_�Z�o�p�ʒu�x�N�g��
  XMVECTOR verts[8] = {
    // near plane corners
    { -1, -1, 0 },	// [0]:����
    {  1, -1, 0 },	// [1]:�E��
    {  1,  1, 0 },	// [2]:�E��
    { -1,  1 ,0 },	// [3]:����

    // far plane corners.
    { -1, -1, 1 },	// [4]:����
    {  1, -1, 1 },	// [5]:�E��
    {  1,  1, 1 },	// [6]:�E��
    { -1,  1, 1 } 	// [7]:����
  };

  // �r���[�v���W�F�N�V�����s��̋t�s���p���āA�e���_���Z�o����(�N���b�s���O��ԁ��O���[�o�����)
#pragma region 
  verts[0] = DirectX::XMVector3TransformCoord(verts[0], inv_matrix);// [0]:����
  verts[6] = DirectX::XMVector3TransformCoord(verts[6], inv_matrix);// [6]:�E��

#ifdef _DEBUG // �f�o�b�O���C���p
  verts[1] = DirectX::XMVector3TransformCoord(verts[1], inv_matrix);// [1]:�E��
  verts[2] = DirectX::XMVector3TransformCoord(verts[2], inv_matrix);// [2]:�E��
  verts[3] = DirectX::XMVector3TransformCoord(verts[3], inv_matrix);// [3]:����

  verts[4] = DirectX::XMVector3TransformCoord(verts[4], inv_matrix);// [4]:����
  verts[5] = DirectX::XMVector3TransformCoord(verts[5], inv_matrix);// [5]:�E��
  verts[7] = DirectX::XMVector3TransformCoord(verts[7], inv_matrix);// [7]:����
#endif // DEBUG
#pragma endregion

  XMFLOAT4X4 matrix4X4 = {};
  XMStoreFloat4x4(&matrix4X4, matrix);

  // ������i�t���X�^���j���\������U���ʂ��Z�o����
  // 0:������, 1:�E����, 2:������, 3:�㑤��, 4:��O����, 5:������
  // �S�Ă̖ʂ̖@���͓����������悤�ɐݒ�
#pragma region �r���[�v���W�F�N�V�����s�񂩂�@�����擾
  // ������
  frustum.plane[0].n = { matrix4X4._14 + matrix4X4._11 ,matrix4X4._24 + matrix4X4._21 ,matrix4X4._34 + matrix4X4._31 };
  // �E����
  frustum.plane[1].n = { matrix4X4._14 - matrix4X4._11 ,matrix4X4._24 - matrix4X4._21 ,matrix4X4._34 - matrix4X4._31 };
  // ������
  frustum.plane[2].n = { matrix4X4._14 + matrix4X4._12,matrix4X4._24 + matrix4X4._22,matrix4X4._34 + matrix4X4._32 };
  // �㑤��
  frustum.plane[3].n = { matrix4X4._14 - matrix4X4._12,matrix4X4._24 - matrix4X4._22,matrix4X4._34 - matrix4X4._32 };
  // ��O����
  frustum.plane[4].n = { -matrix4X4._14 + matrix4X4._13,-matrix4X4._24 + matrix4X4._23 ,-matrix4X4._34 + matrix4X4._33 };
  // ������
  frustum.plane[5].n = { -matrix4X4._14 - matrix4X4._13,-matrix4X4._24 - matrix4X4._23,-matrix4X4._34 - matrix4X4._33 };
#pragma endregion

#pragma region �@���𐳋K��
  for (int i = 0; i < 6; ++i) {
    XMStoreFloat3(&frustum.plane[i].n,
      XMVector3Normalize(XMLoadFloat3(&frustum.plane[i].n)));
  }
#pragma endregion

  // �e���ʂɊ܂܂�钸�_�̈ʒu�x�N�g���ƒP�ʖ@���̓��ς�����āA���_����̋��������߂�
#pragma region
  // i = 0:������, 1 : �E����, 2 : ������, 3 : �㑤��, 4 : ��O����, 5 : ������
  for (int i = 0; i < 6; ++i) {
    XMVECTOR dist = XMVector3Dot(XMLoadFloat3(&frustum.plane[i].n),
      // 0 or 6 : �������_(��O)�ƉE�㒸�_(��)
      verts[(i + 1 % 2 == 0) * 6]);
    frustum.plane[i].d = XMVectorGetX(dist);
  }
#pragma endregion

#ifdef _DEBUG
  DirectX::XMVECTOR Fm_Norm[6] = {};

  for (int i = 0; i < 6; ++i) {
    Fm_Norm[i] = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&frustum.plane[i].n));
  }

  // �e���E��outLineNorm��ʂ̖@���̊O�ς��狁�߂Đ��K������
  // �������E��	
  XMStoreFloat3(&debugLineNorm[0], XMVector3Normalize(XMVector3Cross(Fm_Norm[0], Fm_Norm[2])));
  // �E�����E��
  XMStoreFloat3(&debugLineNorm[1], XMVector3Normalize(XMVector3Cross(Fm_Norm[2], Fm_Norm[1])));
  // �E�㋫�E��
  XMStoreFloat3(&debugLineNorm[2], XMVector3Normalize(XMVector3Cross(Fm_Norm[1], Fm_Norm[3])));
  // ���㋫�E��
  XMStoreFloat3(&debugLineNorm[3], XMVector3Normalize(XMVector3Cross(Fm_Norm[3], Fm_Norm[0])));
#endif // DEBUG

}

//bool Camera::hitFrustum(AABB aabb) {
//  // ������̍쐬
//  createFrustum();
//
//  //�C�e���ʂ̖@���̐�����p����AABB�̂W���_�̒�����ŋߓ_�ƍŉ��_�����߂�
//  for (int i = 0; i < 6; i++)
//  {
//    XMFLOAT3 NegaPos = aabb.position;	// �ŋߓ_
//    XMFLOAT3 PosiPos = aabb.position;	// �ŉ��_
//
//    if (frustum.plane[i].n.x >= 0)
//    {
//      PosiPos.x += aabb.size.x;
//    }
//    else
//    {
//      PosiPos.x -= aabb.size.x;
//    }
//    if (frustum.plane[i].n.y >= 0)
//    {
//      PosiPos.y += aabb.size.y;
//    }
//    else
//    {
//      PosiPos.y -= aabb.size.y;
//    }
//    if (frustum.plane[i].n.z >= 0)
//    {
//      PosiPos.z += aabb.size.z;
//    }
//    else
//    {
//      PosiPos.z -= aabb.size.z;
//    }
//
//    if (frustum.plane[i].n.x >= 0)
//    {
//      //NegaPos.x -= aabb.r.x;
//      NegaPos.x = PosiPos.x - aabb.size.x * 2.0f;
//    }
//    else
//    {
//      //NegaPos.x += aabb.r.x;
//      NegaPos.x = PosiPos.x + aabb.size.x * 2.0f;
//    }
//    if (frustum.plane[i].n.y >= 0)
//    {
//      //NegaPos.y -= aabb.r.y;
//      NegaPos.y = PosiPos.y - aabb.size.y * 2.0f;
//    }
//    else
//    {
//      //NegaPos.y += aabb.r.y;
//      NegaPos.y = PosiPos.y + aabb.size.y * 2.0f;
//    }
//    if (frustum.plane[i].n.z >= 0)
//    {
//      //NegaPos.z -= aabb.r.z;
//      NegaPos.z = PosiPos.z - aabb.size.z * 2.0f;
//    }
//    else
//    {
//      //NegaPos.z += aabb.r.z;
//      NegaPos.z = PosiPos.z + aabb.size.z * 2.0f;
//    }
//
//    //�D�e���ʂƂ̓��ς��v�Z���A�����E���O����(�\������)���s��
//    //  �O���ƕ�����Ώ�����break���m�肳����
//    //  ������Ԃł���΁A�X�e�[�^�X��ύX���Ă��玟�̕��ʂƂ̃`�F�b�N�ɑ�����
//    //  �����ł���΁A���̂܂܎��̕��ʂƂ̃`�F�b�N�ɑ�����
//    XMVECTOR PosiDot = XMVector3Dot(XMLoadFloat3(&PosiPos), XMVector3Normalize(XMLoadFloat3(&frustum.plane[i].n)));
//    XMVECTOR NegaDot = XMVector3Dot(XMLoadFloat3(&NegaPos), XMVector3Normalize(XMLoadFloat3(&frustum.plane[i].n)));
//
//    float posidot = XMVectorGetX(PosiDot);
//    float negadot = XMVectorGetX(NegaDot);
//
//    if (posidot <= frustum.plane[i].d && negadot <= frustum.plane[i].d)
//    {
//      return false;
//    }
//  }
//
//  return true;
//}

void Camera::debugDraw()
{
  // LineRenderer* line= Graphics::Instance().GetLineRenderer();
  // DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();

  //for (int i = 0; i < 4; ++i) {
  //  // ���E���`��
  //  line->AddVertex(eye, { 1.0f, 1.0f, 1.0f, 1.0f });
  //  line->AddVertex({ eye.x + debugLineNorm[i].x * 40.0f, eye.y + debugLineNorm[i].y * 40.0f,eye.z + debugLineNorm[i].z * 40.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

  //  // Near�̎l�p�`��Far�̎l�p�`�̕`��
  //  if (i != 3)
  //  {
  //    line->AddVertex(debugFar[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugFar[i + 1], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[i + 1], { 1.0f,1.0f,1.0f,1.0f });
  //  }
  //  else
  //  {
  //    line->AddVertex(debugFar[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugFar[0], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[0], { 1.0f,1.0f,1.0f,1.0f });
  //  }

  //  // Near�̓_��Far�̓_�̕`��
  //  debug->DrawSphere(debugNear[i], 0.02f, { 1.0f,1.0f,0.0f,1.0f });
  //  debug->DrawSphere(debugFar[i], 0.1f, { 0.0f,0.0f,1.0f,1.0f });
  //}
}

// �w������Ɍ���
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
  // ���_�A�����_�A���������r���[�s����쐬
  XMVECTOR Eye = XMLoadFloat3(&eye);
  XMVECTOR Focus = XMLoadFloat3(&focus);
  XMVECTOR Up = XMLoadFloat3(&up);
  XMMATRIX View = XMMatrixLookAtLH(Eye, Focus, Up);
  XMStoreFloat4x4(&view, View);

  // �r���[���t�s�񉻂��A���[���h�s��ɖ߂�
  XMMATRIX World = XMMatrixInverse(nullptr, View);
  XMFLOAT4X4 world;
  XMStoreFloat4x4(&world, World);

  // �J�����̕��������o��
  this->right.x = world._11;
  this->right.y = world._12;
  this->right.z = world._13;

  this->up.x = world._21;
  this->up.y = world._22;
  this->up.z = world._23;

  this->forward.x = world._31;
  this->forward.y = world._32;
  this->forward.z = world._33;

  // ���_�A�����_��ۑ�
  this->eye = eye;
  this->focus = focus;
}

void Camera::SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up)
{
  // ���_�A�����_�A���������r���[�s����쐬
  XMVECTOR Eye = XMLoadFloat3(&eye);
  XMVECTOR Direction = XMLoadFloat3(&direction);
  XMVECTOR Up = XMLoadFloat3(&up);
  XMMATRIX View = XMMatrixLookToLH(Eye, Direction, Up);
  XMStoreFloat4x4(&view, View);

  // �r���[���t�s�񉻂��A���[���h�s��ɖ߂�
  XMMATRIX World = XMMatrixInverse(nullptr, View);
  XMFLOAT4X4 world;
  XMStoreFloat4x4(&world, World);

  // �J�����̕��������o��
  this->right.x = world._11;
  this->right.y = world._12;
  this->right.z = world._13;

  this->up.x = world._21;
  this->up.y = world._22;
  this->up.z = world._23;

  this->forward.x = world._31;
  this->forward.y = world._32;
  this->forward.z = world._33;

  // ���_�A�����_��ۑ�
  this->eye = eye;
  this->focus = focus;
}

void Camera::SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT4& direction, const DirectX::XMFLOAT3& up)
{
  // ���_�A�����_�A���������r���[�s����쐬
  XMVECTOR Eye = XMLoadFloat3(&eye);
  XMVECTOR Direction = XMLoadFloat4(&direction);
  XMVECTOR Up = XMLoadFloat3(&up);
  XMMATRIX View = XMMatrixLookToLH(Eye, Direction, Up);
  XMStoreFloat4x4(&view, View);

  // �r���[���t�s�񉻂��A���[���h�s��ɖ߂�
  XMMATRIX World = XMMatrixInverse(nullptr, View);
  XMFLOAT4X4 world;
  XMStoreFloat4x4(&world, World);

  // �J�����̕��������o��
  this->right.x = world._11;
  this->right.y = world._12;
  this->right.z = world._13;

  this->up.x = world._21;
  this->up.y = world._22;
  this->up.z = world._23;

  this->forward.x = world._31;
  this->forward.y = world._32;
  this->forward.z = world._33;

  // ���_�A�����_��ۑ�
  this->eye = eye;
  this->focus = focus;
}

//�p�[�X�y�N�e�B�u�ݒ�
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
  // ��p�A��ʔ䗦�A�N���b�v��������v���W�F�N�V�����s����쐬
  XMMATRIX Projection = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
  XMStoreFloat4x4(&projection, Projection);

  //	�p�����[�^�[�����ۑ�
  this->fovY = fovY;
  this->aspect = aspect;
  this->nearZ = nearZ;
  this->farZ = farZ;
}

// �J�����ړ�
DirectX::XMFLOAT3 Camera::MovieMove(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, float timer)
{
  XMVECTOR Start = XMLoadFloat3(&start);
  XMVECTOR End = XMLoadFloat3(&end);

  XMFLOAT3 Value = {};
  XMStoreFloat3(&Value, XMVectorLerp(Start, End, timer));

  return Value;
}

// 0.001�܂ł̌덷�͔F�߂��r
inline bool Equal(DirectX::XMFLOAT3 left, DirectX::XMFLOAT3 right)
{
  if (fabs(left.x - right.x) > 0.001 * fmax(1, fmax(fabs(left.x), fabs(right.x)))) return false;
  if (fabs(left.y - right.y) > 0.001 * fmax(1, fmax(fabs(left.y), fabs(right.y)))) return false;
  if (fabs(left.z - right.z) > 0.001 * fmax(1, fmax(fabs(left.z), fabs(right.z)))) return false;

  return true;
}

bool Camera::Movie(
  DirectX::XMFLOAT3 startEye, DirectX::XMFLOAT3 endEye,
  DirectX::XMFLOAT3 startFocus, DirectX::XMFLOAT3 endFocus,
  float timer, DirectX::XMFLOAT3 cameraUp
) {
  startEye = MovieMove(startEye, endEye, min(1.0f, timer));
  startFocus = MovieMove(startFocus, endFocus, min(1.0f, timer));

  SetLookAt(startEye, startFocus, cameraUp);

  return (Equal(startEye, endEye) && Equal(startFocus, endFocus));
}

void Camera::CollideStage(StageManager& manager, DirectX::XMFLOAT3& eye, DirectX::XMFLOAT3& focus)
{
  HitResult result = {};
  if (manager.Collision(focus, eye, &result)) {
    eye = result.position;
  }
}
