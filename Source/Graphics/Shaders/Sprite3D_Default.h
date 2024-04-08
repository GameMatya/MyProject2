#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class Sprite3D_Default : public Sprite3D_Shader
{
public:
	Sprite3D_Default(ID3D11Device* device);
	~Sprite3D_Default() override {}

	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
	void Draw(ID3D11DeviceContext* dc, const Sprite3D* sprite, const DirectX::XMFLOAT4& color) override;
	void End(ID3D11DeviceContext* dc) override;

private:
	struct CbScene
	{
		DirectX::XMFLOAT4X4		viewProjection;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer;

	struct CbMesh
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;

};
