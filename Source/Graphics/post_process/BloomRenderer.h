#pragma once

#include <memory>

#include "Graphics/RendererBase.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/RenderContext/RenderContext.h"
#include "Graphics/GaussianBlur.h"
#include "Graphics/post_process/PostShader.h"

#define BLUR_PASS_COUNT (4)

// �|�X�g�v���Z�X�p�̃����_���[
class BloomRenderer :public RendererBase
{
public:
  BloomRenderer();
  ~BloomRenderer();

  // �`��
  void	Render(PostProcessContext& ppc);

  void	ImGuiRender();

  //	�V�[�����̐ݒ�
  void	SetSceneData(ID3D11ShaderResourceView* sceneSRV);

private:
  // �V�F�[�_�[
  std::unique_ptr<PostShader> luminanceShader = nullptr;
  std::unique_ptr<PostShader> finalPassShader = nullptr;

  // �V�[���`��f�[�^
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	sceneSRV;

  //	 ���P�x���o�ڂ����o�b�t�@
  std::unique_ptr<GaussianBlur> gaussianBlur[BLUR_PASS_COUNT];

  //	 ���P�x���o�o�b�t�@
  std::unique_ptr<RenderTarget>	luminanceExtractRenderTarget;

};
