#pragma once

#include <wrl.h>
#include <d3d11.h>
#include "Math/OriginMath.h"
#include "Graphics/RenderContext/RenderContext.h"
#include "ShaderStaties.h"

// SpriteShader��
class SpriteBatch
{
public:
  //--------------------------------------------------------------
  //  �\���̒�`
  //--------------------------------------------------------------
  struct vertex
  {
    DirectX::XMFLOAT3 position = {};
    DirectX::XMFLOAT2 texcoord = {};
  };

  // �u�����h�X�e�[�g�̐ݒ� (Begin �̑O�ɌĂ�ł���)
  void setBlendState(BS_MODE index, ID3D11DeviceContext* dc) {
    blendState = ShaderStates::Instance().getBlendState(index);
    const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
  }

  //--------------------------------------------------------------
  //  �R���X�g���N�^
  //--------------------------------------------------------------
  SpriteBatch(size_t);
  SpriteBatch(ID3D11Device*, const char*, size_t);

  //--------------------------------------------------------------
  //  �f�X�g���N�^
  //--------------------------------------------------------------
  ~SpriteBatch() {};

  //--------------------------------------------------------------
  //  �O����
  //--------------------------------------------------------------
  void begin(ID3D11DeviceContext*);

  //--------------------------------------------------------------
  //  �X�v���C�g�o�b�`�`��
  //--------------------------------------------------------------
  void render(
    const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
    const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
    const DirectX::XMFLOAT2& center, float angle/*radian*/,
    const DirectX::XMFLOAT4& color
  );

  //--------------------------------------------------------------
  //  �㏈��
  //--------------------------------------------------------------
  void end(ID3D11DeviceContext*);

private:
  bool LoadTexture(const char* filename);

private:
  const float UV_ADJUST = 1.0f;

  D3D11_VIEWPORT viewport;

  // �V�F�[�_�|�C���^
  Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader = nullptr;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader = nullptr;

  Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout = nullptr;
  // ���_�o�b�t�@
  Microsoft::WRL::ComPtr<ID3D11Buffer> buffer = nullptr;

  // �摜�f�[�^
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView = nullptr;
  D3D11_TEXTURE2D_DESC        tex2dDesc = {};

  // �X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11BlendState> blendState = nullptr;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState = nullptr;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState = nullptr;

  // �`�悷��ő吔
  UINT MAX_INSTANCES;

  // ���݂̕`�搔
  UINT instanceCount = 0;

  Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;

  //--------------------------------------------------------------
  //  �\���̒�`
  //--------------------------------------------------------------
  struct instance
  {
    DirectX::XMFLOAT4X4 ndcTransform = {};
    DirectX::XMFLOAT4 texcoordTransform = {};
    DirectX::XMFLOAT4 color = {};
  };
  instance* instances = nullptr;

};
