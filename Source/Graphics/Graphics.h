#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>

#include "Shaders/SpriteShader.h"
#include "Shaders/ModelShader.h"
#include "Shaders/StaticModelShader.h"
#include "post_process/PostShader.h"
#include "Graphics/RenderTarget.h"

#include "DebugRenderer.h"
#include "LineRenderer.h"

enum class SHADER_ID_MODEL {
  FIRST_DEFERRED,

  MAX
};

enum class SHADER_ID_POST {
  DEFAULT,
  SECOND_DEFERRED,
  SKY_BOX,
  COLOR_GRADING,
  DISTANCE_FOG,

  MAX
};

enum class SHADER_ID_SPRITE {
  DEFAULT,
  BEND_SCREEN,

  MAX
};

class Graphics
{
public:
  Graphics(HWND hWnd);
  ~Graphics();

  // �C���X�^���X�擾
  static Graphics& Instance() { return *instance; }

  // �f�o�C�X�擾
  ID3D11Device* GetDevice() const { return device.Get(); }

  // �f�o�C�X�R���e�L�X�g�擾
  ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

  // �X���b�v�`�F�[���擾
  IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }

  // �����_�[�^�[�Q�b�g�r���[�擾
  ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

  // �f�v�X�X�e���V���r���[�擾
  ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

  // �X�N���[�����擾
  UINT GetScreenWidth() const { return screenWidth; }

  // �X�N���[�������擾
  UINT GetScreenHeight() const { return screenHeight; }

  // �X�N���[���T�C�Y�擾
  DirectX::XMUINT2 GetScreenSize() const { return { screenWidth,screenHeight }; }

  // �~���[�e�b�N�X�擾
  std::mutex& GetMutex() { return graphicMutex; }

  // �V�F�[�_�[�擾
  ModelShader* GetShader(const SHADER_ID_MODEL& index) { return modelShaders[(int)index].get(); }
  StaticModelShader* GetShaderStatic(const SHADER_ID_MODEL& index) { return staticModelShaders[(int)index].get(); }
  PostShader* GetShader(const SHADER_ID_POST& index) { return postShaders[(int)index].get(); }
  SpriteShader* GetShader(const SHADER_ID_SPRITE& index) { return spriteShaders[(int)index].get(); }

  // �����_�[�^�[�Q�b�g�擾
  RenderTarget* GetDebugRenderTarget() { return debugRenderTarget.get(); }

  // �f�o�b�O�����_���擾
  DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

  // ���C�������_���擾
  LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

  // �E�B���h�E�n���h���擾
  const HWND& GetHwnd()const { return hwnd; }

private:
  static Graphics* instance;
  CONST HWND hwnd;

  // SceneView�pRenderTarget
  std::unique_ptr<RenderTarget>       debugRenderTarget;
  std::unique_ptr<DebugRenderer>      debugRenderer;
  std::unique_ptr<LineRenderer>       lineRenderer;

  std::unique_ptr<SpriteShader>       spriteShaders[(int)SHADER_ID_SPRITE::MAX] = {};
  std::unique_ptr<ModelShader>        modelShaders[(int)SHADER_ID_MODEL::MAX] = {};
  std::unique_ptr<StaticModelShader>  staticModelShaders[(int)SHADER_ID_MODEL::MAX] = {};
  std::unique_ptr<PostShader>         postShaders[(int)SHADER_ID_POST::MAX] = {};

  Microsoft::WRL::ComPtr<ID3D11Device>			      device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext>		  immediateContext;
  Microsoft::WRL::ComPtr<IDXGISwapChain>			    swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
  Microsoft::WRL::ComPtr<ID3D11Texture2D>			    depthStencilBuffer;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

  UINT	screenWidth;
  UINT	screenHeight;

  std::mutex graphicMutex;

};
