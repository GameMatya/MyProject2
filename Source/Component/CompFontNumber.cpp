#include "CompFontNumber.h"
#include <imgui.h>
#include <sstream>
#include <iomanip>

void CompFontNumber::Start()
{
  font = gameObject.lock()->GetComponent<CompFont>();
}

void CompFontNumber::Update(const float& elapsedTime)
{
  std::wstringstream wss;
  // �t�B�[���h����ݒ肵0�Ŗ��߂�
  if (fieldWidth > 0) {
    wss << std::setw(fieldWidth) << std::setfill(L'0');
  }

  wss << std::fixed << std::setprecision(fractionCount) << *valuePtr;
  font.lock()->SetString(wss.str());
}
