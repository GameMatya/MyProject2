#include "CompSpriteBase.h"
#include "System/framework.h"

#include "imgui.h"

CompSpriteBase::CompSpriteBase(const char* filename)
{
  // �e�N�X�`���ǂݍ��� ( �p�X�̎w�肪������΃_�~�[���쐬 )
  texture = std::make_unique<Texture>(filename, true);
  texSize = { static_cast<float>(texture->GetWidth()),static_cast<float>(texture->GetHeight()) };
}

void CompSpriteBase::CalcAffineTransform(const DirectX::XMFLOAT2& screenPosition)
{
  ObjectTransform& transform = gameObject.lock()->transform;

#pragma region �A�t�B���ϊ��Ɏg�p����p�����[�^�[������
  const DirectX::XMFLOAT2 scale = { transform.scale.x,transform.scale.y };

  // �s�{�b�g�̏���
  DirectX::XMFLOAT2 drawPivot = ((texSize * scale) / texSize) * pivot;

  // �`��ʒu
  DirectX::XMFLOAT2 position = screenPosition;
  position -= drawPivot;

  // �`��͈�
  DirectX::XMFLOAT2 extent = texSize * scale;

#pragma endregion

#pragma region �A�t�B���ϊ�
  // �X�v���C�g���\������S���_�̃X�N���[�����W���v�Z����
  DirectX::XMFLOAT2 positions[] = {
    DirectX::XMFLOAT2(position.x            , position.y),			      // ����
    DirectX::XMFLOAT2(position.x + extent.x , position.y),			      // �E��
    DirectX::XMFLOAT2(position.x            , position.y + extent.y),	// ����
    DirectX::XMFLOAT2(position.x + extent.x , position.y + extent.y),	// �E��
  };

  // �X�v���C�g���\������S���_�̃e�N�X�`�����W���v�Z����
  DirectX::XMFLOAT2 texcoords[] = {
    DirectX::XMFLOAT2(texPos.x            , texPos.y),			        // ����
    DirectX::XMFLOAT2(texPos.x + texSize.x, texPos.y),			        // �E��
    DirectX::XMFLOAT2(texPos.x            , texPos.y + texSize.y),	// ����
    DirectX::XMFLOAT2(texPos.x + texSize.x, texPos.y + texSize.y),	// �E��
  };

  // �X�v���C�g�̒��S�ŉ�]�����邽�߂ɂS���_�̒��S�ʒu��
  // ���_(0, 0)�ɂȂ�悤�Ɉ�U���_���ړ�������B
  DirectX::XMFLOAT2 offset = position + extent * (pivot / texSize);
  for (auto& p : positions)
  {
    p -= offset;
  }

  // ���_����]������
  float theta = angle * (DirectX::XM_PI / 180.0f);	// �p�x�����W�A��(��)�ɕϊ�
  float c = cosf(theta);
  float s = sinf(theta);
  for (auto& p : positions)
  {
    DirectX::XMFLOAT2 r = p;
    p.x = c * r.x + -s * r.y;
    p.y = s * r.x + c * r.y;
  }

  // ��]�̂��߂Ɉړ����������_�����̈ʒu�ɖ߂�
  for (auto& p : positions)
  {
    p += offset;
  }

  // �X�N���[�����W�n����NDC���W�n�֕ϊ�����B
  for (auto& p : positions)
  {
    p.x = 2.0f * p.x / SCREEN_WIDTH - 1.0f;
    p.y = 1.0f - 2.0f * p.y / SCREEN_HEIGHT;
  }

#pragma endregion

  // ���_�o�b�t�@�̃f�[�^���쐬
  for (int i = 0; i < 4; ++i)
  {
    vertexResource[i].position.x = positions[i].x;
    vertexResource[i].position.y = positions[i].y;
    vertexResource[i].position.z = 0.0f;

    vertexResource[i].color = color;

    vertexResource[i].texcoord.x = texcoords[i].x / texture->GetWidth();
    vertexResource[i].texcoord.y = texcoords[i].y / texture->GetHeight();
  }
}

void CompSpriteBase::DrawImGui()
{
  ImGui::DragFloat("Angle", &angle);
  ImGui::DragFloat2("Pivot", &pivot.x);
  ImGui::DragFloat2("Tex Pos", &texPos.x);
  ImGui::DragFloat2("Tex Size", &texSize.x);

  int bsID = static_cast<int>(bsMode);
  ImGui::Combo("Blend State", &bsID, "NONE\0ALPHA\0ADD\0SUB\0REPLACE\0MULTI\0LIGHT\0DARK\0SCREEN\0\0");
  bsMode = static_cast<BS_MODE>(bsID);

  int dsID = static_cast<int>(dsMode);
  ImGui::Combo("Depth State", &dsID, "NONE\0ZT_ON_ZW_ON\0ZT_ON_ZW_OFF\0ZT_OFF_ZW_ON\0ZT_OFF_ZW_OFF\0MASK\0APPLY_MASK\0EXCLUSIVE\0\0");
  dsMode = static_cast<DS_MODE>(dsID);

  int rsID = static_cast<int>(rsMode);
  ImGui::Combo("Rasterize State", &rsID, "NONE\0BACK\0FRONT\0\0");
  rsMode = static_cast<RS_MODE>(rsID);

  ImGui::ColorEdit4("Color", &color.x);
}
