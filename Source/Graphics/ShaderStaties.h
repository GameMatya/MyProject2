#pragma once

#include <d3d11.h>
#include <wrl.h>

// enum �̍쐬
#if(1)
  // �u�����h�X�e�[�g
enum class BS_MODE
{
  NONE,
  ALPHA,
  ADD,
  SUBTRACT,
  REPLACE,
  MULTIPLY,
  LIGHTEN,
  DARKEN,
  SCREEN,

  MAX
};

// ���X�^���C�U
enum class RS_MODE
{
  SOLID_CULL_NONE,
  SOLID_CULL_BACK,
  SOLID_CULL_FRONT,
  WIREFRAME,

  MAX
};

// �[�x�X�e�[�g
enum class DS_MODE
{
  NONE, // �[�x�e�X�g���Ȃ�
  ZT_ON_ZW_ON,    // �e�X�g�L�A�������ݗL
  ZT_ON_ZW_OFF,   // �e�X�g�L�A�������ݖ�
  ZT_OFF_ZW_ON,   // �e�X�g���A�������ݗL
  ZT_OFF_ZW_OFF,  // �e�X�g���A�������ݖ�

  // �ȍ~�X�e���V���}�X�N�p
  MASK,
  APPLY_MASK,
  EXCLUSIVE,

  MAX
};

#endif

class ShaderStates
{
private: // ����J���\�b�h�̍쐬
  ShaderStates();
  ~ShaderStates() {}

  // �T���v���[�X�e�[�g (�O���ŐG�点�Ȃ��̂�Private����)
  enum SS_MODE
  {
    WRAP_POINT,
    WRAP_LINEAR,
    WRAP_ANISOTROPIC,

    BORDER_POINT,
    BORDER_LINEAR,
    BORDER_ANISOTROPIC,

    SHADOW,

    MAX
  };

public: // ���J���\�b�h�̍쐬
#if(1)
  // �V���O���g��
  static ShaderStates& Instance()
  {
    static ShaderStates instance;
    return instance;
  }

  // �u�����h�X�e�[�g�̎擾
  ID3D11BlendState* getBlendState(BS_MODE index)
  {
    return blendStates[static_cast<int>(index)].Get();
  }

  // ���X�^���C�U�̎擾
  ID3D11RasterizerState* getRasterizerState(RS_MODE index)
  {
    return rasterizerStates[static_cast<int>(index)].Get();
  }

  // �[�x�X�e�[�g �̎擾
  ID3D11DepthStencilState* getDepthStencilState(DS_MODE index)
  {
    return depthStencilStates[static_cast<int>(index)].Get();
  }

#endif

private: // �����o�ϐ�
  Microsoft::WRL::ComPtr<ID3D11BlendState>		    blendStates[static_cast<int>(BS_MODE::MAX)];
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>	  rasterizerStates[static_cast<int>(RS_MODE::MAX)];
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates[static_cast<int>(DS_MODE::MAX)];
  Microsoft::WRL::ComPtr<ID3D11SamplerState>	    samplerStates[SS_MODE::MAX];

};