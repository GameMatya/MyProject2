#pragma once

#include <d3d11.h>
#include <wrl.h>

class RendererBase
{
protected:
  // �ݒ肳��Ă��郌���_�[�^�[�Q�b�g��ێ�
  inline void CacheRenderTargets(ID3D11DeviceContext* dc) {
    dc->RSGetViewports(&cachedViewportCount, cachedViewports);
    dc->OMGetRenderTargets(1
      , cachedRenderTargetView.ReleaseAndGetAddressOf()
      , cachedDepthStencilView.ReleaseAndGetAddressOf());
  }

  // ���̃����_�[�^�[�Q�b�g�ɖ߂�
  inline void RestoreRenderTargets(ID3D11DeviceContext* dc) {
    dc->RSSetViewports(cachedViewportCount, cachedViewports);
    dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
    cachedViewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
  }

  // �ێ������r���[�|�[�g��ݒ�
  inline void SetViewPorts(ID3D11DeviceContext* dc) {
    dc->RSSetViewports(cachedViewportCount, cachedViewports);
  }

private:
  // ���ݐݒ肳��Ă��郌���_�[�^�[�Q�b�g��ێ�����̂Ɏg�p
  UINT cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
  D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView = nullptr;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView = nullptr;

};
