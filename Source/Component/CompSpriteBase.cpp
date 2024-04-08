#include "CompSpriteBase.h"
#include "System/framework.h"

#include "imgui.h"

CompSpriteBase::CompSpriteBase(const char* filename)
{
  // テクスチャ読み込み ( パスの指定が無ければダミーを作成 )
  texture = std::make_unique<Texture>(filename, true);
  texSize = { static_cast<float>(texture->GetWidth()),static_cast<float>(texture->GetHeight()) };
}

void CompSpriteBase::CalcAffineTransform(const DirectX::XMFLOAT2& screenPosition)
{
  ObjectTransform& transform = gameObject.lock()->transform;

#pragma region アフィン変換に使用するパラメーターを準備
  const DirectX::XMFLOAT2 scale = { transform.scale.x,transform.scale.y };

  // ピボットの処理
  DirectX::XMFLOAT2 drawPivot = ((texSize * scale) / texSize) * pivot;

  // 描画位置
  DirectX::XMFLOAT2 position = screenPosition;
  position -= drawPivot;

  // 描画範囲
  DirectX::XMFLOAT2 extent = texSize * scale;

#pragma endregion

#pragma region アフィン変換
  // スプライトを構成する４頂点のスクリーン座標を計算する
  DirectX::XMFLOAT2 positions[] = {
    DirectX::XMFLOAT2(position.x            , position.y),			      // 左上
    DirectX::XMFLOAT2(position.x + extent.x , position.y),			      // 右上
    DirectX::XMFLOAT2(position.x            , position.y + extent.y),	// 左下
    DirectX::XMFLOAT2(position.x + extent.x , position.y + extent.y),	// 右下
  };

  // スプライトを構成する４頂点のテクスチャ座標を計算する
  DirectX::XMFLOAT2 texcoords[] = {
    DirectX::XMFLOAT2(texPos.x            , texPos.y),			        // 左上
    DirectX::XMFLOAT2(texPos.x + texSize.x, texPos.y),			        // 右上
    DirectX::XMFLOAT2(texPos.x            , texPos.y + texSize.y),	// 左下
    DirectX::XMFLOAT2(texPos.x + texSize.x, texPos.y + texSize.y),	// 右下
  };

  // スプライトの中心で回転させるために４頂点の中心位置が
  // 原点(0, 0)になるように一旦頂点を移動させる。
  DirectX::XMFLOAT2 offset = position + extent * (pivot / texSize);
  for (auto& p : positions)
  {
    p -= offset;
  }

  // 頂点を回転させる
  float theta = angle * (DirectX::XM_PI / 180.0f);	// 角度をラジアン(θ)に変換
  float c = cosf(theta);
  float s = sinf(theta);
  for (auto& p : positions)
  {
    DirectX::XMFLOAT2 r = p;
    p.x = c * r.x + -s * r.y;
    p.y = s * r.x + c * r.y;
  }

  // 回転のために移動させた頂点を元の位置に戻す
  for (auto& p : positions)
  {
    p += offset;
  }

  // スクリーン座標系からNDC座標系へ変換する。
  for (auto& p : positions)
  {
    p.x = 2.0f * p.x / SCREEN_WIDTH - 1.0f;
    p.y = 1.0f - 2.0f * p.y / SCREEN_HEIGHT;
  }

#pragma endregion

  // 頂点バッファのデータを作成
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
