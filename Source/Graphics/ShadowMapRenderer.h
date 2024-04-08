#pragma once

#include "RenderTarget.h"
#include "DepthStencil.h"
#include "RenderContext/RenderContext.h"
#include "GaussianBlur.h"
#include "RendererBase.h"
#include "Shaders/ShadowCastShader.h"
#include "Shaders/StaticModelShader.h"

#include <vector>
#include <memory>

class ModelRenderer;

// �\�t�g�V���h�E ( PCF )
class ShadowMapRenderer : public RendererBase
{
public:
  // �J�X�P�[�h�V���h�E�̉�
  static const int SHADOW_MAP_COUNT = 3;

private:
  // �萔�o�b�t�@�\����
  struct ShadowCB
  {
    int index = 0;
    DirectX::XMFLOAT3 dummy;
    DirectX::XMFLOAT4X4 cascadedLVP[SHADOW_MAP_COUNT]; // ���C�g�r���[�v���W�F�N�V�����s��
  };

public:
  ShadowMapRenderer();

  // �V���h�E�}�b�v�ɕ`��
  void Render(ModelRenderer* renderer);

  void DrawImGui();

private:
  // �萔�o�b�t�@�X�V
  void DrawShadowMap(ID3D11DeviceContext* dc, ModelRenderer* renderer);

  // �N���b�v�s��Ə�Z�������C�g�r���[�v���W�F�N�V�����s����Z�o
  void CalcCascadedLVP(ShadowCB& cb);

private:
  const float Near = 1.0f;
  const float Far = 300.0f;
  static const int RENDER_TARGET_SIZE = 3200;

  float cascadeArea[SHADOW_MAP_COUNT + 1] = {
    Near,
    20,
    120,
    Far
  };

  std::unique_ptr<ShadowCastShader>  shadowCastShader = nullptr;
  std::unique_ptr<StaticModelShader>  shadowCastStaticShader = nullptr;
  std::array<std::unique_ptr<DepthStencil>, SHADOW_MAP_COUNT> shadowMaps;

  float lightDistance = 100.0f;
  float drawDistance = 210.0f;

  Microsoft::WRL::ComPtr<ID3D11Buffer> shadowCB;

};
