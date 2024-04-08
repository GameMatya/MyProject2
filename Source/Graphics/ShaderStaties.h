#pragma once

#include <d3d11.h>
#include <wrl.h>

// enum の作成
#if(1)
  // ブレンドステート
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

// ラスタライザ
enum class RS_MODE
{
  SOLID_CULL_NONE,
  SOLID_CULL_BACK,
  SOLID_CULL_FRONT,
  WIREFRAME,

  MAX
};

// 深度ステート
enum class DS_MODE
{
  NONE, // 深度テストしない
  ZT_ON_ZW_ON,    // テスト有、書き込み有
  ZT_ON_ZW_OFF,   // テスト有、書き込み無
  ZT_OFF_ZW_ON,   // テスト無、書き込み有
  ZT_OFF_ZW_OFF,  // テスト無、書き込み無

  // 以降ステンシルマスク用
  MASK,
  APPLY_MASK,
  EXCLUSIVE,

  MAX
};

#endif

class ShaderStates
{
private: // 非公開メソッドの作成
  ShaderStates();
  ~ShaderStates() {}

  // サンプラーステート (外部で触らせないのでPrivate属性)
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

public: // 公開メソッドの作成
#if(1)
  // シングルトン
  static ShaderStates& Instance()
  {
    static ShaderStates instance;
    return instance;
  }

  // ブレンドステートの取得
  ID3D11BlendState* getBlendState(BS_MODE index)
  {
    return blendStates[static_cast<int>(index)].Get();
  }

  // ラスタライザの取得
  ID3D11RasterizerState* getRasterizerState(RS_MODE index)
  {
    return rasterizerStates[static_cast<int>(index)].Get();
  }

  // 深度ステート の取得
  ID3D11DepthStencilState* getDepthStencilState(DS_MODE index)
  {
    return depthStencilStates[static_cast<int>(index)].Get();
  }

#endif

private: // メンバ変数
  Microsoft::WRL::ComPtr<ID3D11BlendState>		    blendStates[static_cast<int>(BS_MODE::MAX)];
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>	  rasterizerStates[static_cast<int>(RS_MODE::MAX)];
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates[static_cast<int>(DS_MODE::MAX)];
  Microsoft::WRL::ComPtr<ID3D11SamplerState>	    samplerStates[SS_MODE::MAX];

};