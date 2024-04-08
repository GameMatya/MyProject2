#pragma once

#include <memory>

#include "PostShader.h"
#include "Graphics/RendererBase.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/texture.h"

// �Ō�̗v�f�͑O�̃t���[���̕��ϋP�x��ێ����Ă������߂̕�
#define ToneBufferCount (10)

class ToneMapRenderer :public RendererBase
{
public:
  ToneMapRenderer();

  // ��������
  void Photometry();

  // �g�[���}�b�s���O���s
  void Execute();

  void SetSceneSRV(ID3D11ShaderResourceView* srv) { sceneSRV = srv; }

private:
  std::unique_ptr<PostShader> copyShader;
  std::unique_ptr<PostShader> luminanceShader;
  std::unique_ptr<PostShader> shrinkLogShader;
  std::unique_ptr<PostShader> mixAvgLumiShader;
  std::unique_ptr<PostShader> toneMapShader;

  int currentBufferNum = 0;

  ID3D11ShaderResourceView* sceneSRV = nullptr;

  // ���ϋP�x ( �k���o�b�t�@�@ )
  std::unique_ptr<RenderTarget> luminanceTargets[ToneBufferCount];

  // �ŏI�I�ȕ��ϋP�x
  std::unique_ptr<RenderTarget> averageLuminance;
  // �O�̃t���[���̕��ϋP�x
  std::unique_ptr<RenderTarget> oldAverageLuminance;

};
