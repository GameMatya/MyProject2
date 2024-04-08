#pragma once

#include <wrl.h>
#include <d3d11.h>

// テクスチャ
class Texture
{
public:
  Texture(const char* filename, const bool& createDummy = false);
  ~Texture() {}

  static HRESULT LoadTexture(const char* filename, ID3D11Device* device, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* textureDesc = nullptr, const bool& isCreateDummy = false, const UINT& dummyColor = 0x00);

  // シェーダーリソースビュー取得
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() { return shaderResourceView; }

  // テクスチャの情報取得
  inline const D3D11_TEXTURE2D_DESC& GetTexture2dDesc() const { return texture2dDesc; }

  // テクスチャ幅取得
  int GetWidth() const { return texture2dDesc.Width; }

  // テクスチャ高さ取得
  int GetHeight() const { return texture2dDesc.Height; }

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;
  D3D11_TEXTURE2D_DESC	texture2dDesc;

};
