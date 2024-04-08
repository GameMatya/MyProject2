#include "CompThrusterUI.h"
#include <imgui.h>

void CompThrusterUI::Start()
{
  scrollSprite = gameObject.lock()->GetComponent<CompSprite2D>();
  initTexSizeX = scrollSprite.lock()->GetTexSize().x;
}

void CompThrusterUI::Update(const float& elapsedTime)
{
  DirectX::XMFLOAT2 texSize = DirectX::XMFLOAT2(initTexSizeX * (*scrollValue), scrollSprite.lock()->GetTexSize().y);
  scrollSprite.lock()->SetTexSize(texSize);

  // �X���X�^�[�c�ʂ�3�����؂�����ԐF�ɂ���
  float alpha = scrollSprite.lock()->GetColor().w;
  if (*scrollValue <= 0.3f) {
    // ���̐F�����F����Ȃ��ׁAR������1�ȏ�̒l��ݒ肷��
    scrollSprite.lock()->SetColor(DirectX::XMFLOAT4(3.0f, 0.0f, 0.0f, alpha));
  }
  else {
    scrollSprite.lock()->SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));
  }
}