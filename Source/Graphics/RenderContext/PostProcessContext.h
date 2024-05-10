#pragma once

#include <DirectXMath.h>

// �F���␳���
struct ColorGradingData
{
  float	hueShift = 0;	// �F������
  float	saturation = 1;	// �ʓx����
  float	brightness = 1;	// ���x����
};

// �K�E�X�t�B���^�[�v�Z���
struct GaussianFilterData
{
  float				deviation = 10.0f;	// �W���΍�
};

// ���P�x���o�p���
struct LuminanceExtractionData
{
  float				threshold = 0.5f;	    // 臒l
  float				intensity = 1.0f;	    // �u���[���̋��x
  float       maxLuminance = 10.0f; // �ő�P�x
};

// ���ӌ������
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

// �u���E���ǃV�F�[�_�[
struct BrownFilterData
{
  float pixel_detail = 6; // ��ʂׂ̍���
  float display_flat = 0.2f; // ��ʂ̏o������ range( 0~1 )
  float noise_scroll = 0; // �m�C�Y�̃X�N���[��
};

// �f�B�X�^���X�t�H�O���
struct DistanceFogData
{
  DirectX::XMFLOAT4 color = { 1,1,1,1 };
};

// ��ʊE�[�x���
struct DoFData
{
  float _near = 0.05f;
  float _far = 0.2f;
};

struct PostProcessContext
{
  //	 �F���␳���
  ColorGradingData				colorGradingData;

  //	 �K�E�X�t�B���^�[���
  GaussianFilterData			gaussianFilterData;

  //	 ���P�x���o�p���
  LuminanceExtractionData	bloomData;

  // ���ӌ������
  VignettingData		      vignettingData;

  // �u���E���ǃe���r���
  BrownFilterData         brownFilterData;

  // �f�B�X�^���X�t�H�O���
  DistanceFogData         distanceFogData;

  // ��ʊE�[�x���
  DoFData                 dofData;

};