#include "ShaderStaties.h"
#include "Graphics.h"
#include "System/Misc.h"

ShaderStates::ShaderStates() {
  ID3D11Device* device = Graphics::Instance().GetDevice();
  HRESULT hr;

  // ブレンドステート
  {
    D3D11_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    // BS_NONE
    {
      desc = {};
      desc.AlphaToCoverageEnable = false;
      desc.IndependentBlendEnable = false;
      desc.RenderTarget[0].BlendEnable = false;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

      hr = device->CreateBlendState(&desc, blendStates[0].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // BS_ALPHA
    {
      desc = {};
      desc.AlphaToCoverageEnable = false;
      desc.IndependentBlendEnable = false;
      desc.RenderTarget[0].BlendEnable = true;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

      hr = device->CreateBlendState(&desc, blendStates[1].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // BS_ADD
    {
      desc = {};
      desc.AlphaToCoverageEnable = FALSE;
      desc.IndependentBlendEnable = FALSE;
      desc.RenderTarget[0].BlendEnable = TRUE;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      hr = device->CreateBlendState(&desc, blendStates[2].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // BS_SUBTRACT
    {
      desc = {};
      desc.AlphaToCoverageEnable = FALSE;
      desc.IndependentBlendEnable = FALSE;
      desc.RenderTarget[0].BlendEnable = TRUE;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_SRC_ALPHA
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3D11_BLEND_ONE
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; //D3D11_BLEND_OP_SUBTRACT
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      hr = device->CreateBlendState(&desc, blendStates[3].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	BS_REPLACE
    {
      desc = {};
      desc.AlphaToCoverageEnable = FALSE;
      desc.IndependentBlendEnable = FALSE;
      desc.RenderTarget[0].BlendEnable = TRUE;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      hr = device->CreateBlendState(&desc, blendStates[4].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	BS_MULTIPLY
    {
      desc = {};
      desc.AlphaToCoverageEnable = FALSE;
      desc.IndependentBlendEnable = FALSE;
      desc.RenderTarget[0].BlendEnable = TRUE;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      hr = device->CreateBlendState(&desc, blendStates[5].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	BS_LIGHTEN
    {
      desc = {};
      desc.AlphaToCoverageEnable = FALSE;
      desc.IndependentBlendEnable = FALSE;
      desc.RenderTarget[0].BlendEnable = TRUE;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      hr = device->CreateBlendState(&desc, blendStates[6].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	BS_DARKEN
    {
      desc = {};
      desc.AlphaToCoverageEnable = FALSE;
      desc.IndependentBlendEnable = FALSE;
      desc.RenderTarget[0].BlendEnable = TRUE;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      hr = device->CreateBlendState(&desc, blendStates[7].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    //	BS_SCREEN
    {
      desc = {};
      desc.AlphaToCoverageEnable = FALSE;
      desc.IndependentBlendEnable = FALSE;
      desc.RenderTarget[0].BlendEnable = TRUE;
      desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3DBLEND_INVDESTCOLOR
      desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; //D3DBLEND_ONE
      desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
      desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
      desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
      desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
      hr = device->CreateBlendState(&desc, blendStates[8].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

  }

  // 深度ステンシルステート
  {
    D3D11_DEPTH_STENCIL_DESC desc;

    // None
    {
      ZeroMemory(&desc, sizeof(desc));

      hr = device->CreateDepthStencilState(&desc, depthStencilStates[(int)(DS_MODE::NONE)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // TEST_ON & WRITE_ON
    {
      desc.DepthEnable = TRUE;
      desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DS_MODE::ZT_ON_ZW_ON)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // TEST_ON & WRITE_ON
    {
      desc.DepthEnable = TRUE;
      desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
      desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DS_MODE::ZT_ON_ZW_OFF)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // TEST_ON & WRITE_ON
    {
      desc.DepthEnable = FALSE;
      desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
      desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DS_MODE::ZT_OFF_ZW_ON)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // TEST_ON & WRITE_ON
    {
      desc.DepthEnable = FALSE;
      desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
      desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
      hr = device->CreateDepthStencilState(&desc, depthStencilStates[static_cast<size_t>(DS_MODE::ZT_OFF_ZW_OFF)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // MASK
    {
      ZeroMemory(&desc, sizeof(desc));

      desc.DepthEnable = TRUE;// 深度テストを行う
      desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;// 深度値を書き込まない
      desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;// 深度テストに必ず失敗する（描画はしないがステンシル値は書き込む）
      desc.StencilEnable = TRUE;// ステンシルテストを行う
      desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
      desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
      desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
      desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;// ステンシルテストには必ず合格
      desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;// 深度テストに失敗してステンシルテストに成功した場合
      desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      hr = device->CreateDepthStencilState(&desc, depthStencilStates[(int)(DS_MODE::MASK)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // APPLY_MASK
    {
      ZeroMemory(&desc, sizeof(desc));

      desc.DepthEnable = TRUE;
      desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO; // ステンシルバッファへの書き込みをオフにする
      desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS; // 深度テストに必ず失敗する
      desc.StencilEnable = TRUE;
      desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
      desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
      desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
      desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
      desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      hr = device->CreateDepthStencilState(&desc, depthStencilStates[(int)(DS_MODE::APPLY_MASK)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // EXCLUSIVE
    {
      ZeroMemory(&desc, sizeof(desc));

      desc.DepthEnable = TRUE;
      desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO; // ステンシルバッファへの書き込みをオフにする
      desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS; // 深度テストに必ず失敗する
      desc.StencilEnable = TRUE;
      desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
      desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
      desc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
      desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
      desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
      hr = device->CreateDepthStencilState(&desc, depthStencilStates[(int)(DS_MODE::EXCLUSIVE)].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
  }

  // ラスタライザーステート
  {
    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.FrontCounterClockwise = false;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0;
    desc.SlopeScaledDepthBias = 0;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;
    desc.MultisampleEnable = true;

    // NONE
    {
      desc.FillMode = D3D11_FILL_SOLID;
      desc.CullMode = D3D11_CULL_NONE;
      desc.AntialiasedLineEnable = false;

      hr = device->CreateRasterizerState(&desc, rasterizerStates[(int)RS_MODE::SOLID_CULL_NONE].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // CULL_BACK
    {
      desc.FillMode = D3D11_FILL_SOLID;
      desc.CullMode = D3D11_CULL_BACK;
      desc.AntialiasedLineEnable = false;

      hr = device->CreateRasterizerState(&desc, rasterizerStates[(int)RS_MODE::SOLID_CULL_BACK].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // CULL_FRONT
    {
      desc.FillMode = D3D11_FILL_SOLID;
      desc.CullMode = D3D11_CULL_FRONT;
      desc.AntialiasedLineEnable = false;

      hr = device->CreateRasterizerState(&desc, rasterizerStates[(int)RS_MODE::SOLID_CULL_FRONT].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // WIREFRAME
    {
      desc.FillMode = D3D11_FILL_WIREFRAME;
      desc.CullMode = D3D11_CULL_NONE;
      desc.AntialiasedLineEnable = TRUE;

      hr = device->CreateRasterizerState(&desc, rasterizerStates[(int)RS_MODE::WIREFRAME].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
  }

  // サンプラーステート
  {
    D3D11_SAMPLER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.MipLODBias = 0;
    desc.MaxAnisotropy = 16;
    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    desc.BorderColor[0] = 0;
    desc.BorderColor[1] = 0;
    desc.BorderColor[2] = 0;
    desc.BorderColor[3] = 0;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    // MIP_POINT
    {
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
      hr = device->CreateSamplerState(&desc, samplerStates[SS_MODE::WRAP_POINT].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // MIP_LINEAR
    {
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      hr = device->CreateSamplerState(&desc, samplerStates[SS_MODE::WRAP_LINEAR].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // ANISOTROPIC
    {
      desc.Filter = D3D11_FILTER_ANISOTROPIC;
      hr = device->CreateSamplerState(&desc, samplerStates[SS_MODE::WRAP_ANISOTROPIC].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

    // MIP_POINT
    {
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
      hr = device->CreateSamplerState(&desc, samplerStates[SS_MODE::BORDER_POINT].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // MIP_LINEAR
    {
      desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      hr = device->CreateSamplerState(&desc, samplerStates[SS_MODE::BORDER_LINEAR].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // ANISOTROPIC
    {
      desc.Filter = D3D11_FILTER_ANISOTROPIC;
      hr = device->CreateSamplerState(&desc, samplerStates[SS_MODE::BORDER_ANISOTROPIC].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // SHADOW_ONLY
    {
      ZeroMemory(&desc, sizeof(desc));
      desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
      desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
      desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
      desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
      desc.ComparisonFunc = D3D11_COMPARISON_GREATER;
      desc.MinLOD = 0;
      desc.MaxLOD = D3D11_FLOAT32_MAX;
      hr = device->CreateSamplerState(&desc, samplerStates[SS_MODE::SHADOW].GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
  }

  ID3D11SamplerState* ss[] = {
    samplerStates[SS_MODE::WRAP_POINT].Get(),
    samplerStates[SS_MODE::WRAP_LINEAR].Get(),
    samplerStates[SS_MODE::WRAP_ANISOTROPIC].Get(),

    samplerStates[SS_MODE::BORDER_POINT].Get(),
    samplerStates[SS_MODE::BORDER_LINEAR].Get(),
    samplerStates[SS_MODE::BORDER_ANISOTROPIC].Get(),
  };
  Graphics::Instance().GetDeviceContext()->PSSetSamplers(0, ARRAYSIZE(ss), ss);

  // シャドウマップ用サンプラーステートは10番に登録
  Graphics::Instance().GetDeviceContext()->PSSetSamplers(10, 1, samplerStates[SS_MODE::SHADOW].GetAddressOf());
}