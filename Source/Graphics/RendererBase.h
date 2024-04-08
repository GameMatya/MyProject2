#pragma once

#include <d3d11.h>
#include <wrl.h>

class RendererBase
{
protected:
  // 設定されているレンダーターゲットを保持
  inline void CacheRenderTargets(ID3D11DeviceContext* dc) {
    dc->RSGetViewports(&cachedViewportCount, cachedViewports);
    dc->OMGetRenderTargets(1
      , cachedRenderTargetView.ReleaseAndGetAddressOf()
      , cachedDepthStencilView.ReleaseAndGetAddressOf());
  }

  // 元のレンダーターゲットに戻す
  inline void RestoreRenderTargets(ID3D11DeviceContext* dc) {
    dc->RSSetViewports(cachedViewportCount, cachedViewports);
    dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
    cachedViewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
  }

  // 保持したビューポートを設定
  inline void SetViewPorts(ID3D11DeviceContext* dc) {
    dc->RSSetViewports(cachedViewportCount, cachedViewports);
  }

private:
  // 現在設定されているレンダーターゲットを保持するのに使用
  UINT cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
  D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView = nullptr;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView = nullptr;

};
