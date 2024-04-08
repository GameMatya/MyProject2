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
  // フィールド幅を設定し0で埋める
  if (fieldWidth > 0) {
    wss << std::setw(fieldWidth) << std::setfill(L'0');
  }

  wss << std::fixed << std::setprecision(fractionCount) << *valuePtr;
  font.lock()->SetString(wss.str());
}
