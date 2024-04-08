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

  // SceneConstantBuffer(slot 5)�̍X�V
  static void UpdateSceneConstant(ID3D11DeviceContext* dc, const float& elapsedTime, const float& totalTime);

  // �V�F�[�_�[�̍쐬
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

  // �V�F�[�_�[�̗L�����E������
  void Activate(ID3D11DeviceContext* dc);
  void Inactivate(ID3D11DeviceContext* dc);

  // �R���X�g���N�^�ŃV�F�[�_�[���쐬����ۂɎg�p����
  virtual HRESULT createVsFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertex_shader) = 0;

protected:
  Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // ���_�V�F�[�_
  Microsoft::WRL::ComPtr<ID3D11GeometryShader>	GS = nullptr; // �W�I���g���V�F�[�_
  Microsoft::WRL::ComPtr<ID3D11HullShader>			HS = nullptr; // �n���V�F�[�_
  Microsoft::WRL::ComPtr<ID3D11DomainShader>		DS = nullptr; // �h���C���V�F�[�_
  Microsoft::WRL::ComPtr<ID3D11PixelShader>			PS = nullptr; // �s�N�Z���V�F�[�_

  // ���̓��C�A�E�g
  Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout = nullptr;

private:
  // �ėp�萔�o�b�t�@
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

  // ���_�V�F�[�_�[�Ɠ��̓��C�A�E�g���i�[����A�z�z��
  struct SetVertexShaderAndInputLayout
  {
    SetVertexShaderAndInputLayout(ID3D11VertexShader* vertexShader, ID3D11InputLayout* inputLayout) : vertexShader(vertexShader), inputLayout(inputLayout) {}
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
  };
  inline static std::map<std::string, SetVertexShaderAndInputLayout> vsMap;

  // �s�N�Z���V�F�[�_�[���i�[����A�z�z��
  inline static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> psMap;

};
