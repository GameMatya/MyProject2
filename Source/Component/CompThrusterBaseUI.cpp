#include "CompThrusterBaseUI.h"
#include <imgui.h>

void CompThrusterBaseUI::Start()
{
  sprite = gameObject.lock()->GetComponent<CompSprite2D>();
}

void CompThrusterBaseUI::Update(const float& elapsedTime)
{
  // �I�[�o�[�q�[�g��Ԃ̏ꍇ�A�ԐF�œ_�ł���
  float alpha = sprite.lock()->GetColor().w;
  if (*paramValue <= 0.0f) {
    timer += elapsedTime * timerSpeed;

    // ���̐F�����F����Ȃ��ׁAR������1�ȏ�̒l��ݒ肷��
    sprite.lock()->SetColor(
      Mathf::Lerp(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha), DirectX::XMFLOAT4(5.0f, 0.0f, 0.0f, alpha), fabs(sinf(timer)))
    );
  }
  else {
    sprite.lock()->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));
  }
}

void CompThrusterBaseUI::DrawImGui()
{
  ImGui::DragFloat("TimerSpeed", &timerSpeed);
}
