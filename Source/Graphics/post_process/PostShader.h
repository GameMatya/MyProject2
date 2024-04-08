#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/ShaderStaties.h"
#include "Math/OriginMath.h"
#include "Graphics/RenderContext/RenderContext.h"

class PostShader :public ShaderBase
{
public:
  PostShader();
  PostShader(const char* vsFilepath, const char* psFilepath);

  // SceneConstantBuffer( slot 9 )の更新
  static void UpdatePostConstant(ID3D11DeviceContext* dc, const PostProcessContext& rc);

  // 描画開始
  virtual void Begin(ID3D11DeviceContext* dc);

  // 描画  sceneSrv = 描画に使うシーン ( 配列に対応 )
  virtual void Draw(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** sceneSrv, const int ArraySize);

  // 描画終了
  virtual void End(ID3D11DeviceContext* dc);

protected:
  // VS用のCSOを読み込む ( 関数のラップ )
  HRESULT createVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader)override
  {
    // 頂点シェーダーを作成
    return ShaderBase::createVsFromCso(device, cso_name, vertex_shader, nullptr, nullptr, 0);
  }

private: // 定数バッファ関係
  struct CBLuminanceExtraction
  {
    float				threshold = 0.5f;	    // 閾値
    float				intensity = 1.0f;	    // ブルームの強度
    float       maxLuminance = 12.0f; // 最大輝度
    float       dummy = 0.0f;
  };

  struct CBColorGrading
  {
    float	hueShift;	  // 色相調整
    float	saturation;	// 彩度調整
    float	brightness;	// 明度調整
    float	dummy;
  };

  struct CBDistanceFog
  {
    DirectX::XMFLOAT4 color;
  };

  // 定数バッファ用構造体
  struct CbPostProcess
  {
    CBLuminanceExtraction luminance;
    CBColorGrading colorGrading;
    CBDistanceFog distanceFog;
  };

protected:
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> postConstantBuffer;

};
