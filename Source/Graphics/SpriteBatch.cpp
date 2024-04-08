#include "SpriteBatch.h"
#include <stdio.h> 
#include <WICTextureLoader.h>

#include "System/Misc.h"
#include "Graphics/Graphics.h"

//--------------------------------------------------------------
//  �R���X�g���N�^
//--------------------------------------------------------------
SpriteBatch::SpriteBatch(size_t maxInstance) {
  MAX_INSTANCES = static_cast<UINT>(maxInstance);

  ID3D11Device* device = Graphics::Instance().GetDevice();

  HRESULT hr = S_OK;

  //VertexBuffer�̍쐬
  {
    vertex vertices[4] = {
    { DirectX::XMFLOAT3(-0.5, +0.5, 0) },
    { DirectX::XMFLOAT3(+0.5, +0.5, 0) },
    { DirectX::XMFLOAT3(-0.5, -0.5, 0) },
    { DirectX::XMFLOAT3(+0.5, -0.5, 0) },
    };

    // ���_�o�b�t�@�̍쐬
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0; //Not use for vertex buffers.
    subresourceData.SysMemSlicePitch = 0; //Not use for vertex buffers.
    if (FAILED(device->CreateBuffer(&bufferDesc, &subresourceData, &buffer)))
    {
      assert(!"���_�o�b�t�@�̍쐬�Ɏ��s(SpriteBatch)");
      return;
    }
  }

  {
    // �t�@�C�����J��
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\sprite_batch_vs.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�����߂�
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // ���_�V�F�[�_�[����
    HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // �C���v�b�g���C�A�E�g�̍쐬
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = device->CreateInputLayout(layout, numElements, csoData.get(), csoSize, inputLayout.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �s�N�Z���V�F�[�_�[
  {
    // �t�@�C�����J��
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\sprite_batch_ps.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�����߂�
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // �s�N�Z���V�F�[�_�[����
    HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �C���X�^���X�o�b�t�@�̍쐬
  {
    instance* inst = new instance[MAX_INSTANCES];
    {
      D3D11_BUFFER_DESC bd = {};
      D3D11_SUBRESOURCE_DATA sd = {};

      bd.ByteWidth = sizeof(instance) * static_cast<UINT>(MAX_INSTANCES);
      bd.Usage = D3D11_USAGE_DYNAMIC;
      bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      bd.MiscFlags = 0;
      bd.StructureByteStride = 0;
      sd.pSysMem = inst;
      sd.SysMemPitch = 0; //Not use for vertex buffers.mm 
      sd.SysMemSlicePitch = 0; //Not use for vertex buffers.
      if (FAILED(device->CreateBuffer(&bd, &sd, &instanceBuffer)))
      {
        assert(!"�o�b�t�@�̐����Ɏ��s(SpriteBatch)");
        return;
      }
    }
    delete[] inst;
  }

  // �_�~�[�e�N�X�`���̐���
  {
    const int width = 8;
    const int height = 8;
    UINT pixels[width * height];
    ::memset(pixels, 0xFF, sizeof(pixels));

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

    Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
    HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // Texture2D_Desc�̎擾
    texture->GetDesc(&tex2dDesc);

    hr = device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // ���X�^���C�U�X�e�[�g���쐬
  {
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
    rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0;
    rasterizerDesc.DepthClipEnable = FALSE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    if (FAILED(device->CreateRasterizerState(&rasterizerDesc, &rasterizerState)))
    {
      assert(!"���X�^���C�U�X�e�[�g�̍쐬�Ɏ��s(SpriteBatch)");
      return;
    }
  }

  // �[�x�X�e�[�g���쐬
  {
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    if (FAILED(device->CreateDepthStencilState(&dsDesc, &depthStencilState)))
    {
      assert(!"�f�v�X�X�e���V���X�e�[�g�̍쐬�Ɏ��s(SpriteBatch)");
      return;
    }
  }

}

SpriteBatch::SpriteBatch(ID3D11Device* device, const char* fileName, size_t maxInstance)
{
  MAX_INSTANCES = static_cast<UINT>(maxInstance);;

  //VertexBuffer�̍쐬
  {
    vertex vertices[4] = {
        { DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
        { DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
        { DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
        { DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
    };

    // ���_�o�b�t�@�̍쐬
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0; //Not use for vertex buffers.
    subresourceData.SysMemSlicePitch = 0; //Not use for vertex buffers.
    if (FAILED(device->CreateBuffer(&bufferDesc, &subresourceData, &buffer)))
    {
      assert(!"���_�o�b�t�@�̍쐬�Ɏ��s(SpriteBatch)");
      return;
    }
  }

  {
    // �t�@�C�����J��
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\sprite_batch_vs.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�����߂�
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // ���_�V�F�[�_�[����
    HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // �C���v�b�g���C�A�E�g�̍쐬
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    UINT numElements = ARRAYSIZE(layout);

    hr = device->CreateInputLayout(layout, numElements, csoData.get(), csoSize, inputLayout.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �s�N�Z���V�F�[�_�[
  {
    // �t�@�C�����J��
    FILE* fp = nullptr;
    fopen_s(&fp, "Shader\\sprite_batch_ps.cso", "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    // �t�@�C���̃T�C�Y�����߂�
    fseek(fp, 0, SEEK_END);
    long csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
    std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    // �s�N�Z���V�F�[�_�[����
    HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �C���X�^���X�o�b�t�@�̍쐬
  {
    instance* inst = new instance[MAX_INSTANCES];
    {
      D3D11_BUFFER_DESC bd = {};
      D3D11_SUBRESOURCE_DATA sd = {};

      bd.ByteWidth = sizeof(instance) * static_cast<UINT>(MAX_INSTANCES);
      bd.Usage = D3D11_USAGE_DYNAMIC;
      bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
      bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      bd.MiscFlags = 0;
      bd.StructureByteStride = 0;
      sd.pSysMem = inst;
      sd.SysMemPitch = 0; //Not use for vertex buffers.mm 
      sd.SysMemSlicePitch = 0; //Not use for vertex buffers.
      if (FAILED(device->CreateBuffer(&bd, &sd, &instanceBuffer)))
      {
        assert(!"�o�b�t�@�̐����Ɏ��s(SpriteBatch)");
        return;
      }
    }
    delete[] inst;
  }

  // �摜�̓ǂݍ���
  if (!LoadTexture(fileName))
  {
    assert(!"�e�N�X�`���摜�ǂݍ��ݎ��s(SpriteBatch)");
    return;
  }

  // ���X�^���C�U�X�e�[�g���쐬
  {
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
    rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0;
    rasterizerDesc.DepthClipEnable = FALSE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    if (FAILED(device->CreateRasterizerState(&rasterizerDesc, &rasterizerState)))
    {
      assert(!"���X�^���C�U�X�e�[�g�̍쐬�Ɏ��s(SpriteBatch)");
      return;
    }
  }

  // �[�x�X�e�[�g���쐬
  {
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    if (FAILED(device->CreateDepthStencilState(&dsDesc, &depthStencilState)))
    {
      assert(!"�f�v�X�X�e���V���X�e�[�g�̍쐬�Ɏ��s(SpriteBatch)");
      return;
    }
  }

}

//--------------------------------------------------------------
//  �O�����i�`��O��1�x�Ăԁj
//--------------------------------------------------------------
void SpriteBatch::begin(ID3D11DeviceContext* context)
{
  HRESULT hr = S_OK;
  // DeviceContext��SpriteBatch�̏���ݒ肷��

  UINT strides[2] = { sizeof(vertex), sizeof(instance) };
  UINT offsets[2] = { 0, 0 };
  ID3D11Buffer* vbs[2] = { buffer.Get(), instanceBuffer.Get() };
  context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  context->IASetInputLayout(inputLayout.Get());
  //context->OMSetDepthStencilState(depthStencilState, 1);
  context->RSSetState(rasterizerState.Get());
  context->VSSetShader(vertexShader.Get(), nullptr, 0);
  context->PSSetShader(pixelShader.Get(), nullptr, 0);
  context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

  UINT numViewports = 1;
  context->RSGetViewports(&numViewports, &viewport);

  D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
  D3D11_MAPPED_SUBRESOURCE mappedBuffer;
  hr = context->Map(instanceBuffer.Get(), 0, map, 0, &mappedBuffer);
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  instances = static_cast<instance*>(mappedBuffer.pData);

  instanceCount = 0;
}

//--------------------------------------------------------------
//  �`�揀��
//--------------------------------------------------------------
void SpriteBatch::render(
  const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
  const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
  const DirectX::XMFLOAT2& center, float angle/*radian*/,
  const DirectX::XMFLOAT4& color)
{
  // �`�悵�悤�Ƃ��Ă���񐔂��A�`�悷��ő�l�𒴂��Ă�����
  if (instanceCount >= MAX_INSTANCES)
  {
    assert(!"Number of instances must be less than MAX_INSTANCES.(SpriteBatch)");
    return;
  }

  // �傫����0��������`�悵�Ȃ�
  if (scale.x == 0.0f || scale.y == 0.0f) return;

  // �摜�T�C�Y�̐ݒ�
  float tw = texSize.x;
  float th = texSize.y;
  if (texSize.x == FLT_MIN && texSize.y == FLT_MIN)
  {
    tw = (float)tex2dDesc.Width;
    th = (float)tex2dDesc.Height;
  }

  //	float centerX = dw*0.5f, centerY = dh*0.5f; /*Center of Rotation*/
  float centerX = center.x;
  float centerY = center.y;
  float scaleX = scale.x;
  float scaleY = scale.y;

  centerX *= scaleX;
  centerY *= scaleY;

  // Angle��x�l
  FLOAT c = cosf(angle);
  // Angle��y�l
  FLOAT s = sinf(angle);

  FLOAT w = 2.0f / viewport.Width;
  FLOAT h = -2.0f / viewport.Height;

  instances[instanceCount].ndcTransform._11 = w * scaleX * tw * c;
  instances[instanceCount].ndcTransform._21 = h * scaleX * tw * s;
  instances[instanceCount].ndcTransform._31 = 0.0f;
  instances[instanceCount].ndcTransform._41 = 0.0f;
  instances[instanceCount].ndcTransform._12 = w * scaleY * th * -s;
  instances[instanceCount].ndcTransform._22 = h * scaleY * th * c;
  instances[instanceCount].ndcTransform._32 = 0.0f;
  instances[instanceCount].ndcTransform._42 = 0.0f;
  instances[instanceCount].ndcTransform._13 = 0.0f;
  instances[instanceCount].ndcTransform._23 = 0.0f;
  instances[instanceCount].ndcTransform._33 = 1.0f;
  instances[instanceCount].ndcTransform._43 = 0.0f;
  instances[instanceCount].ndcTransform._14 = w * (-centerX * c + -centerY * -s + centerX * 0 + position.x) - 1.0f;
  instances[instanceCount].ndcTransform._24 = h * (-centerX * s + -centerY * c + centerY * 0 + position.y) + 1.0f;
  instances[instanceCount].ndcTransform._34 = 0.0f;
  instances[instanceCount].ndcTransform._44 = 1.0f;

  // �摜�̕��E����
  float tex_width = static_cast<float>(tex2dDesc.Width);
  float tex_height = static_cast<float>(tex2dDesc.Height);

  // UV���W�̒���
  float u = tw * UV_ADJUST / tex_width;
  float v = th * UV_ADJUST / tex_height;

  instances[instanceCount].texcoordTransform = DirectX::XMFLOAT4(texPos.x / tex_width, texPos.y / tex_height, u, v);
  instances[instanceCount].color = color;

  instanceCount++;
}

//--------------------------------------------------------------
//  �㏈���i�`�揀�����1�x�Ăԁj
//--------------------------------------------------------------
void SpriteBatch::end(ID3D11DeviceContext* context)
{
  context->Unmap(instanceBuffer.Get(), 0);
  context->DrawInstanced(4, instanceCount, 0, 0);
}

// �摜��ǂݍ���
bool SpriteBatch::LoadTexture(const char* filename) {
  // �p�X�𕪉�
  char drive[256], dirname[256], fname[256], ext[256];
  ::_splitpath_s(filename, drive, sizeof(drive), dirname, sizeof(dirname), fname, sizeof(fname), ext, sizeof(ext));

  // �p�X������
  char filepath[256];
  ::_makepath_s(filepath, 256, drive, dirname, fname, ext);

  // �}���`�o�C�g�������烏�C�h�����֕ϊ�
  wchar_t wfilepath[256];
  ::MultiByteToWideChar(CP_ACP, 0, filepath, -1, wfilepath, 256);

  ID3D11Device* device = Graphics::Instance().GetDevice();

  // �e�N�X�`���ǂݍ���
  Microsoft::WRL::ComPtr<ID3D11Resource> resource;
  HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfilepath, resource.GetAddressOf(), shaderResourceView.ReleaseAndGetAddressOf());

  // �e�N�X�`���ǂݍ��ݐ����m�F
  if (FAILED(hr)) return false;

  Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
  hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

  // Texture2D_Desc�̎擾
  texture2d->GetDesc(&tex2dDesc);

  return true;
}