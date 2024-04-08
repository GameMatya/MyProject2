#include "StageManager.h"

bool StageManager::Collision(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult* hitResult)
{
  bool result = false;

  // �X�^�[�g�ƌ�_�̋�����ۑ�����
  float length = FLT_MAX;

  // hit �̕���
  HitResult hit2 = {};

  for (int i = 0; i < stages.size(); ++i)
  {
    // ���C�L���X�g
    if (stages.at(i)->RayCast(start, end, &hit2)) {
      if (hitResult == nullptr)return true;

      result = true;

      //  ��_�ԋ��������߂�
      DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
      DirectX::XMVECTOR Point = DirectX::XMLoadFloat3(&hit2.position);
      DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Point, Start);

      // �������y�����邽�߂�LengthSq���g�� ( ���m�ȋ����͗v��Ȃ��� ) 
      float vecLength = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

      // ���܂ł̋������Z����Α��
      if (vecLength < length)
      {
        // �ŒZ�����̍X�V
        length = vecLength;

        // �ŒZ�����̃q�b�g������
        *hitResult = hit2;
      }
    }
  }

  return result;
}
