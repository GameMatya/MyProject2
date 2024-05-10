#pragma once

#include "Shader.h"
#include "Component/CompModel.h"
#include "../RenderContext/RenderContext.h"
#include "Graphics/ShaderStaties.h"
#include "Math/OriginMath.h"

class ModelShader :public ShaderBase
{
public:
  ModelShader();
  ModelShader(const char* vsFilepath, const char* psFilepath);
  virtual ~ModelShader() {}

  // 描画開始
  virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc);
  virtual void UpdateShaderBuffer(ID3D11DeviceContext* dc, const RenderContext& rc) {};

  // 描画
  virtual void SetBuffers(ID3D11DeviceContext* dc, const std::vector<CompModel::Node>& nodes, const ModelResource::Mesh& mesh);
  virtual void DrawSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset, DirectX::XMFLOAT4 color = { 1,1,1,1 }) = 0;

  // 描画終了
  virtual void End(ID3D11DeviceContext* dc) { Inactivate(dc); };

protected:
  // VS用のCSOを読み込む ( 関数のラップ )
  HRESULT createVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader)override;

protected:
  static const int MAX_BONES = 160;
  struct CbMesh
  {
    DirectX::XMFLOAT4X4	boneTransforms[MAX_BONES];
  };

  struct CbSubset
  {
    DirectX::XMFLOAT4	materialColor;
  };

protected:
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> meshConstantBuffer;
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> subsetConstantBuffer;

};
