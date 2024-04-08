#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class NaturalVignetting
{
public:
	NaturalVignetting(ID3D11Device* device);
	~NaturalVignetting() {}

	void Render(ID3D11DeviceContext* dc, const RenderContext& rc, const Sprite* sprite);

private:
	void Begin(ID3D11DeviceContext* dc);
	void End(ID3D11DeviceContext* dc);

private:
	struct Constant
	{
		float ASPECT_RATIO;
		float VIGNETTING_MECHANICAL_RADIUS2;
		float VIGNETTING_MECHANICAL_INV_SMOOTHNESS;
		float VIGNETTING_MECHANICAL_SCALE;

		float VIGNETTING_NATURAL_COS_FACTOR;
		float VIGNETTING_NATURAL_COS_POWER;
		float VIGNETTING_NATURAL_SCALE;
		float VIGNETTING_COLOR;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>				ConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;

};
