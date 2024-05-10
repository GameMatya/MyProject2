#pragma once

#include <DirectXMath.h>

// 色調補正情報
struct ColorGradingData
{
  float	hueShift = 0;	// 色相調整
  float	saturation = 1;	// 彩度調整
  float	brightness = 1;	// 明度調整
};

// ガウスフィルター計算情報
struct GaussianFilterData
{
  float				deviation = 10.0f;	// 標準偏差
};

// 高輝度抽出用情報
struct LuminanceExtractionData
{
  float				threshold = 0.5f;	    // 閾値
  float				intensity = 1.0f;	    // ブルームの強度
  float       maxLuminance = 10.0f; // 最大輝度
};

// 周辺減光情報
struct VignettingData
{
  float ASPECT_RATIO;
  float MECHANICAL_RADIUS2;
  float MECHANICAL_INV_SMOOTHNESS;
  float MECHANICAL_SCALE;

  float NATURAL_COS_FACTOR;
  float NATURAL_COS_POWER;
  float NATURAL_SCALE;
  float COLOR;
};

// ブラウン管シェーダー
struct BrownFilterData
{
  float pixel_detail = 6; // 画面の細かさ
  float display_flat = 0.2f; // 画面の出っ張り具合 range( 0~1 )
  float noise_scroll = 0; // ノイズのスクロール
};

// ディスタンスフォグ情報
struct DistanceFogData
{
  DirectX::XMFLOAT4 color = { 1,1,1,1 };
};

// 被写界深度情報
struct DoFData
{
  float _near = 0.05f;
  float _far = 0.2f;
};

struct PostProcessContext
{
  //	 色調補正情報
  ColorGradingData				colorGradingData;

  //	 ガウスフィルター情報
  GaussianFilterData			gaussianFilterData;

  //	 高輝度抽出用情報
  LuminanceExtractionData	bloomData;

  // 周辺減光情報
  VignettingData		      vignettingData;

  // ブラウン管テレビ情報
  BrownFilterData         brownFilterData;

  // ディスタンスフォグ情報
  DistanceFogData         distanceFogData;

  // 被写界深度情報
  DoFData                 dofData;

};