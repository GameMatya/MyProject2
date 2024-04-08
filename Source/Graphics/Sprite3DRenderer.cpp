#include "Sprite3DRenderer.h"
#include "Graphics.h"
#include "System/misc.h"

#include <algorithm>

Sprite3DRenderer::Sprite3DRenderer()
{
  ID3D11Device* device = Graphics::Instance().GetDevice();
  HRESULT hr;

  // ���_�V�F�[�_�[����
  {
    // ���̓��C�A�E�g
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    // �ʏ�V�F�[�_�[
    hr = ShaderBase::createVsFromCso(device, "Shader/Sprite3dVS.cso", defaultVS.GetAddressOf(), nullptr, inputElementDesc, ARRAYSIZE(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // �w�莲�r���{�[�h�V�F�[�_�[
    hr = ShaderBase::createVsFromCso(device, "Shader/Sprite3dBillboardAxisVS.cso", billboardAxisVS.GetAddressOf(), nullptr, inputElementDesc, ARRAYSIZE(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // �S���r���{�[�h�V�F�[�_�[
    hr = ShaderBase::createVsFromCso(device, "Shader/Sprite3dBillboardAllVS.cso", billboardAllVS.GetAddressOf(), nullptr, inputElementDesc, ARRAYSIZE(inputElementDesc));
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �萔�o�b�t�@�̐���
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

  // �V�F�[�_�[��ID�Ń\�[�g
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

  // �萔�o�b�t�@�ݒ�
  ConstantBuffer bufferData = {};
  dc->VSSetConstantBuffers(0, 1, cBuffer.GetAddressOf());

  for (auto& sprite : sprites) {
    // �V�F�[�_�[ID���قȂ�΁A�Ή������V�F�[�_�[��ݒ�
    if (sprite->GetShaderID() != currentShaderId)
    {
      // �V�F�[�_�[�̏I������
      shader->End(dc);

      currentShaderId = sprite->GetShaderID();

      // ���̃V�F�[�_�[���擾
      shader = graphics.GetShader(currentShaderId);
      shader->Begin(dc, {});
    }

    // ���_�V�F�[�_�[�̐ݒ�
    {
      const BILLBOARD_AXIS& axisData = sprite->GetBillboardData();

      if (axisData == BILLBOARD_AXIS::NONE) {
        // �ʏ�`��
        dc->VSSetShader(defaultVS.Get(), NULL, 0);
      }
      else if (axisData == BILLBOARD_AXIS::AXIS_ALL) {
        // �S���r���{�[�h
        dc->VSSetShader(billboardAllVS.Get(), NULL, 0);
      }
      else
      {
        // ���ɉ������r���{�[�h
        dc->VSSetShader(billboardAxisVS.Get(), NULL, 0);
      }

    }

    // �萔�o�b�t�@�X�V
    {
      DirectX::XMStoreFloat4x4(&bufferData.worldMatrix, sprite->GetGameObject()->transform.GetWorldTransform());

      const BILLBOARD_AXIS& axisData = sprite->GetBillboardData();
      bufferData.billboardData = BillboardAxis[static_cast<int>(axisData)];

      dc->UpdateSubresource(cBuffer.Get(), 0, 0, &bufferData, 0, 0);
    }

    // �u�����h�X�e�[�g�A���X�^���C�U�X�e�[�g��ݒ�
    shader->SetUpStates(dc, sprite->GetBsMode(), DS_MODE::ZT_ON_ZW_ON, sprite->GetRsMode());

    // �`��
    SpriteShader::VertexResource vertexResource = sprite->GetVertexResource();
    shader->Draw(dc, &vertexResource, sprite->GetTexture()->GetShaderResourceView().GetAddressOf());
  }

  shader->End(dc);
}