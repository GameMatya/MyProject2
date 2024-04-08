#include <stdlib.h>
#include <filesystem>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <stb_image.h>

#include "Texture.h"
#include "Graphics/Graphics.h"
#include "System/Misc.h"
#include "System/Logger.h"

Texture::Texture(const char* filename, const bool& createDummy)
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  HRESULT hr = LoadTexture(filename, device, shaderResourceView.GetAddressOf(), &texture2dDesc, createDummy, 0XFFFFFFFF);
  if (FAILED(hr)) {
    assert(!"テクスチャ画像読み込み失敗(Texture)");
  }
}

inline HRESULT Texture::LoadTexture(const char* filename, ID3D11Device* device, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* textureDesc, const bool& isCreateDummy, const UINT& dummyColor)
{
  // マルチバイト文字からワイド文字(日本語対応)へ変換
  std::filesystem::path filepath(filename);
  Microsoft::WRL::ComPtr<ID3D11Resource> resource;
  HRESULT hr = false;

  // DDSの読み込み
  if (filepath.extension().string() == ".DDS" ||
    filepath.extension().string() == ".dds")
  {
    hr = DirectX::CreateDDSTextureFromFile(device, filepath.c_str(), resource.GetAddressOf(), srv);
  }
  // DDS以外のテクスチャ読み込み
  else {
    // WICで読み込み
    hr = DirectX::CreateWICTextureFromFile(device, filepath.c_str(), resource.GetAddressOf(), srv);

    if (FAILED(hr))
    {
      // WICでサポートされていないフォーマットの場合（TGAなど）は
      // STBで画像読み込みをしてテクスチャを生成する
      int width, height, bpp;
      unsigned char* pixels = stbi_load(filename, &width, &height, &bpp, STBI_rgb_alpha);
      if (pixels != nullptr)
      {
        D3D11_TEXTURE2D_DESC desc = { 0 };
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA data;
        ::memset(&data, 0, sizeof(data));
        data.pSysMem = pixels;
        data.SysMemPitch = width * 4;

        Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
        hr = device->CreateTexture2D(&desc, &data, texture2d.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        hr = device->CreateShaderResourceView(texture2d.Get(), nullptr, srv);
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

        // 後始末
        stbi_image_free(pixels);
      }
    }
  }
  // D3D11_TEXTURE2D_DESCの作成
  if (textureDesc && SUCCEEDED(hr))
  {
    Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    texture2d->GetDesc(textureDesc);
  }

  // ダミーテクスチャの生成
  if (FAILED(hr) && isCreateDummy) {
    LOG("Create Dummy : %s\n", filename);

    const int width = 1;
    const int height = 1;
    UINT pixels[width * height];
    // ダミーテクスチャの色を設定
    std::fill(std::begin(pixels), std::end(pixels), dummyColor);

    D3D11_TEXTURE2D_DESC desc = { 0 };
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA data;
    ::memset(&data, 0, sizeof(data));
    data.pSysMem = pixels;
    data.SysMemPitch = width;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
    hr = device->CreateTexture2D(&desc, &data, texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    hr = device->CreateShaderResourceView(texture2d.Get(), nullptr, srv);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // D3D11_TEXTURE2D_DESCの作成
    if (textureDesc && SUCCEEDED(hr))
    {
      texture2d->GetDesc(textureDesc);
    }
  }

  return hr;
}

