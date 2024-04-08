#include "CompFontTimeLimit.h"

void CompFontTimeLimit::Start()
{
  font = gameObject.lock()->GetComponent<CompFont>();
}

void CompFontTimeLimit::Update(const float& elapsedTime)
{
  // �������Ԃ����܂ŏ��Ȃ��Ȃ����當����Ԃ�����
  if (*timer <= CAUTION_BORDER) {
    float alpha = font.lock()->GetColor().w;
    DirectX::XMFLOAT4 color = CAUTION_COLOR;
    color.w = alpha;

    font.lock()->SetColor(color);
  }
}
