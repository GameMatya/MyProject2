#pragma once

#include <memory>

#include "Graphics/RenderTarget.h"
#include "Graphics/post_process/PostShader.h"
#include "BloomRenderer.h"
#include "ToneMapRenderer.h"

// �|�X�g�v���Z�X�p�̃����_���[
class PostprocessRenderer :public RendererBase
{
public:
  PostprocessRenderer();
  ~PostprocessRenderer();

  // �`��
  void	Render(ID3D11DeviceContext* deviceContext, PostProcessContext& rc,
    std::vector<PostShader*>  shaders, ID3D11ShaderResourceView* depthTexture);

  void DrawDebugGUI();

  // �V�[�����̐ݒ�
  void	SetSceneData(ID3D11ShaderResourceView* srv);

private:
  // �u���[��
  std::unique_ptr<BloomRenderer> bloomRender;

  // �g�[���}�b�v
  std::unique_ptr<ToneMapRenderer> toneMap;

  //	 �ėp�o�b�t�@
  std::unique_ptr<RenderTarget>	WorkTargets[2];

};
