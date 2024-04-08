#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <map>
#include <string>
#include "Graphics/ShaderStaties.h"
#include "Graphics/RenderContext/RenderContext.h"

class ShaderBase
{
public:
  virtual ~ShaderBase() {};

  // SceneConstantBuffer(slot 5)の更新
  static void UpdateSceneConstant(ID3D11DeviceContext* dc, const float& elapsedTime, const float& totalTime);

  // シェーダーの作成
  static HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
  static HRESULT createGsFromCso(ID3D11Device* device, const char* csoName, ID3D11GeometryShader** geometryShader);
  static HRESULT createDsFromCso(ID3D11Device* device, const char* csoName, ID3D11DomainShader** domainShader);
  static HRESULT createHsFromCso(ID3D11Device* device, const char* csoName, ID3D11HullShader** hullShader);
  static HRESULT createPsFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader);
  static HRESULT createCsFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** comShader);

  static void SetUpStates(ID3D11DeviceContext*, BS_MODE, DS_MODE, RS_MODE);

  Microsoft::WRL::ComPtr<ID3D11VertexShader>	 GetVertexShader() { return VS; }
  Microsoft::WRL::ComPtr<ID3D11GeometryShader> GetGeometryShader() { return GS; }
  Microsoft::WRL::ComPtr<ID3D11HullShader>		 GetHullShader() { return HS; }
  Microsoft::WRL::ComPtr<ID3D11DomainShader>	 GetDomainShader() { return DS; }
  Microsoft::WRL::ComPtr<ID3D11PixelShader>		 GetPixelShader() { return PS; }

protected:
  ShaderBase();

  // シェーダーの有効化・無効化
  void Activate(ID3D11DeviceContext* dc);
  void Inactivate(ID3D11DeviceContext* dc);

  // コンストラクタでシェーダーを作成する際に使用する
  virtual HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertex_shader) = 0;

protected:
  Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // 頂点シェーダ
  Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GS = nullptr; // ジオメトリシェーダ
  Microsoft::WRL::ComPtr<ID3D11HullShader>			HS = nullptr; // ハルシェーダ
  Microsoft::WRL::ComPtr<ID3D11DomainShader>		DS = nullptr; // ドメインシェーダ
  Microsoft::WRL::ComPtr<ID3D11PixelShader>			PS = nullptr; // ピクセルシェーダ

  // 入力レイアウト
  Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout = nullptr;

private:
  // 汎用定数バッファ
  struct CbScene
  {
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
    DirectX::XMFLOAT4X4 viewProjection;
    DirectX::XMFLOAT4X4	inverseView;
    DirectX::XMFLOAT4X4	inverseProjection;

    DirectX::XMFLOAT3 cameraPosition;
    float             cameraPositionW = 1.0f;
    DirectX::XMFLOAT4 cameraScope; // x = Near, y = Far, z & w = Padding

    DirectX::XMFLOAT4 time; // x = TotalTime, y = ElapsedTime, z & w = Padding
  };
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;

  // 頂点シェーダーと入力レイアウトを格納する連想配列
  struct SetVertexShaderAndInputLayout
  {
    SetVertexShaderAndInputLayout(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout) : vertexShader(vertexShader), inputLayout(inputLayout) {}
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
  };
  inline static std::map<std::string, SetVertexShaderAndInputLayout> vsMap;

  // ピクセルシェーダーを格納する連想配列
  inline static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> psMap;

};
