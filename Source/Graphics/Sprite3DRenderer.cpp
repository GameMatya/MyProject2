#include "Sprite3DRenderer.h"
#include "Graphics.h"
#include "System/misc.h"

#include <algorithm>

Sprite3DRenderer::Sprite3DRenderer()
{
  ID3D11Device* device = Graphics::Instance().GetDevice();
  HRESULT hr;

  // 頂点シェーダー生成
  {
    // 入力レイアウト
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // 通常シェーダー
    hr = ShaderBase::createVsFromCso(device, "Shader/Sprite3dVS.cso", defaultVS.GetAddressOf(), nullptr, inputElementDesc, ARRAYSIZE(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // 指定軸ビルボードシェーダー
    hr = ShaderBase::createVsFromCso(device, "Shader/Sprite3dBillboardAxisVS.cso", billboardAxisVS.GetAddressOf(), nullptr, inputElementDesc, ARRAYSIZE(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // 全軸ビルボードシェーダー
    hr = ShaderBase::createVsFromCso(device, "Shader/Sprite3dBillboardAllVS.cso", billboardAllVS.GetAddressOf(), nullptr, inputElementDesc, ARRAYSIZE(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // 定数バッファの生成
  {
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.ByteWidth = sizeof(ConstantBuffer);

    hr = device->CreateBuffer(&desc, 0, cBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }
}

void Sprite3DRenderer::Register(CompSprite3D* sprite)
{
  sprites.emplace_back(sprite);

  // シェーダーのIDでソート
  std::sort(sprites.begin(), sprites.end(), [](CompSprite3D* left, CompSprite3D* right) {return left->GetShaderID() < right->GetShaderID(); });
}

void Sprite3DRenderer::Remove(CompSprite3D* sprite)
{
  std::vector<CompSprite3D*>::iterator it = std::find(sprites.begin(), sprites.end(), sprite);
  assert(it != sprites.end());

  sprites.erase(it);
}

void Sprite3DRenderer::Render()
{
  Graphics& graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = graphics.GetDeviceContext();

  SHADER_ID_SPRITE currentShaderId = SHADER_ID_SPRITE::DEFAULT;
  SpriteShader* shader = graphics.GetShader(currentShaderId);
  shader->Begin(dc, {});

  // 定数バッファ設定
  ConstantBuffer bufferData = {};
  dc->VSSetConstantBuffers(0, 1, cBuffer.GetAddressOf());

  for (auto& sprite : sprites) {
    // シェーダーIDが異なれば、対応したシェーダーを設定
    if (sprite->GetShaderID() != currentShaderId)
    {
      // シェーダーの終了処理
      shader->End(dc);

      currentShaderId = sprite->GetShaderID();

      // 次のシェーダーを取得
      shader = graphics.GetShader(currentShaderId);
      shader->Begin(dc, {});
    }

    // 頂点シェーダーの設定
    {
      const BILLBOARD_AXIS& axisData = sprite->GetBillboardData();

      if (axisData == BILLBOARD_AXIS::NONE) {
        // 通常描画
        dc->VSSetShader(defaultVS.Get(), NULL, 0);
      }
      else if (axisData == BILLBOARD_AXIS::AXIS_ALL) {
        // 全軸ビルボード
        dc->VSSetShader(billboardAllVS.Get(), NULL, 0);
      }
      else
      {
        // 軸に沿ったビルボード
        dc->VSSetShader(billboardAxisVS.Get(), NULL, 0);
      }

    }

    // 定数バッファ更新
    {
      DirectX::XMStoreFloat4x4(&bufferData.worldMatrix, sprite->GetGameObject()->transform.GetWorldTransform());

      const BILLBOARD_AXIS& axisData = sprite->GetBillboardData();
      bufferData.billboardData = BillboardAxis[static_cast<int>(axisData)];

      dc->UpdateSubresource(cBuffer.Get(), 0, 0, &bufferData, 0, 0);
    }

    // ブレンドステート、ラスタライザステートを設定
    shader->SetUpStates(dc, sprite->GetBsMode(), DS_MODE::ZT_ON_ZW_ON, sprite->GetRsMode());

    // 描画
    SpriteShader::VertexResource vertexResource = sprite->GetVertexResource();
    shader->Draw(dc, &vertexResource, sprite->GetTexture()->GetShaderResourceView().GetAddressOf());
  }

  shader->End(dc);
}