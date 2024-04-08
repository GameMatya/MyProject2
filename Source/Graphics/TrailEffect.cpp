#include "TrailEffect.h"
#include "TrailEffectManager.h"
#include "Graphics.h"
#include "Texture.h"
#include "SceneSystem/SceneRenderer.h"
#include "System/misc.h"

TrailEffect::TrailEffect(const int& maxVolume, const float& lifeTime, const char* filepath)
  : MAX_TRAIL_DATA_VOLUME(maxVolume), TRAIL_LIFE(lifeTime)
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  // �g�p����e�N�X�`����ǂݍ���
  HRESULT hr = Texture::LoadTexture(filepath, device, textureSRV.GetAddressOf());
  assert(SUCCEEDED(hr));

  // �V�F�[�_�[�쐬
  if (shader == nullptr)
  {
    shader = std::make_unique<SpriteShader>("Shader/TrailEffectVS.cso", "Shader/SpritePS.cso");
  }

  // ���_�o�b�t�@
  {
    D3D11_BUFFER_DESC desc;
    // ���̒��_�� + �X�v���C����ԂŒǉ�����钸�_��
    int maxVertexVolume = (MAX_TRAIL_DATA_VOLUME + (MAX_TRAIL_DATA_VOLUME - 2) * static_cast<int>(1.0f / velvetyRate)) * 2;
    desc.ByteWidth = sizeof(SpriteShader::Vertex) * maxVertexVolume;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  TrailEffectManager::Instance().Register(this);
}

TrailEffect::~TrailEffect()
{
  TrailEffectManager::Instance().Remove(this);
}

void TrailEffect::Set(const DirectX::XMFLOAT3& headPos, const DirectX::XMFLOAT3& tailPos)
{
  TrailData& param = trailDataCollection.emplace_front();
  param.head = headPos;
  param.tail = tailPos;
  param.lifeTimer = TRAIL_LIFE;
}

// ���_�f�[�^�̍X�V�ƁA���_�o�b�t�@�ɒ��_��ǉ�
void TrailEffect::Update()
{
  UpdateTrailData(SceneRenderer::Instance().GetElapsedTime());

  // ���_�������Ȃ��ꍇ�A�������Ȃ�
  if (trailDataCollection.size() < 2)return;

  // ���_�o�b�t�@�̓��e���X�V
  UpdateVertexBufferData();
}

void TrailEffect::Render(ID3D11DeviceContext* dc)
{
  // ���_�f�[�^���`��ɕK�v�Ȑ��𖞂����Ă��Ȃ������珈�����Ȃ�
  if (vertices.size() <= 4) return;

  // �`�揀��
  shader->Begin(dc, {}, BS_MODE::ADD, DS_MODE::ZT_ON_ZW_OFF, RS_MODE::SOLID_CULL_NONE);

  // �`��
  Draw(dc);

  // �`��̌㏈��
  shader->End(dc);
}

void TrailEffect::Clear()
{
  trailDataCollection.clear();
}

void TrailEffect::Draw(ID3D11DeviceContext* dc)
{
  // ���_�o�b�t�@�[���Z�b�g
  UINT stride = sizeof(SpriteShader::Vertex);
  UINT offset = 0;
  dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

  // ���_�o�b�t�@�̍X�V
  {
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    memcpy(mappedSubresource.pData, vertices.data(), sizeof(SpriteShader::Vertex) * vertices.size());

    dc->Unmap(vertexBuffer.Get(), 0);
  }

  // �e�N�X�`���ݒ�
  dc->PSSetShaderResources(0, 1, textureSRV.GetAddressOf());

  dc->Draw(static_cast<UINT>(vertices.size()), 0);
}

void TrailEffect::UpdateTrailData(const float& elapsedTime)
{
  // �f�[�^�ʂ��I�[�o�[�����ꍇ�A�Â��f�[�^���폜
  if (trailDataCollection.size() > MAX_TRAIL_DATA_VOLUME)
    trailDataCollection.pop_back();

  int deadCount = 0;

  // �����X�V
  for (auto& data : trailDataCollection) {
    data.lifeTimer -= elapsedTime;

    if (data.lifeTimer <= 0.0f)
      deadCount++;
  }

  // �������s�����f�[�^�͍폜
  while (deadCount) {
    trailDataCollection.pop_back();
    deadCount--;
  }
}

void TrailEffect::UpdateVertexBufferData()
{
  vertices.clear();

  // �g���C���̒��_��
  int vertexCount = static_cast<int>(trailDataCollection.size()) - 1;
  // �X�v���C����ԂŒǉ�����钸�_�� ( �擪�ƍŌ�͕�Ԃ���Ȃ��ׁA-2 )
  vertexCount += (vertexCount - 2) * static_cast<int>(1.0f / velvetyRate);

  // �g���C����UV�ݒ�p�ϐ�
  float amount = 1.0f / vertexCount;
  // UV��V���W
  float v = 0.0f;

  // �擪�̒��_��ǉ�
  AddVertex(trailDataCollection[0].head, DirectX::XMFLOAT2(1.0f, v));
  AddVertex(trailDataCollection[0].tail, DirectX::XMFLOAT2(0.0f, v));
  v += amount;

  // �g���C���̃X�v���C�����
  AddSplineCurve(amount, v);

  // �Ō���̒��_��ǉ�
  AddVertex(trailDataCollection[trailDataCollection.size() - 1].head, DirectX::XMFLOAT2(1.0f, v));
  AddVertex(trailDataCollection[trailDataCollection.size() - 1].tail, DirectX::XMFLOAT2(0.0f, v));
}

void TrailEffect::AddVertex(const DirectX::XMVECTOR& position, const DirectX::XMFLOAT2& texcoord)
{
  SpriteShader::Vertex& vertex = vertices.emplace_back();
  DirectX::XMStoreFloat3(&vertex.position, position);
  vertex.texcoord = texcoord;
}

void TrailEffect::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texcoord)
{
  SpriteShader::Vertex& vertex = vertices.emplace_back();
  vertex.position = position;
  vertex.texcoord = texcoord;
}

void TrailEffect::AddSplineCurve(const float& amount, float& v)
{
  const int splineCount = static_cast<int>(trailDataCollection.size()) - 3;
  for (int i = 0; i < splineCount; ++i)
  {
    DirectX::XMVECTOR head0 = DirectX::XMLoadFloat3(&trailDataCollection[i].head);
    DirectX::XMVECTOR head1 = DirectX::XMLoadFloat3(&trailDataCollection[i + 1].head);
    DirectX::XMVECTOR head2 = DirectX::XMLoadFloat3(&trailDataCollection[i + 2].head);
    DirectX::XMVECTOR head3 = DirectX::XMLoadFloat3(&trailDataCollection[i + 3].head);

    DirectX::XMVECTOR tail0 = DirectX::XMLoadFloat3(&trailDataCollection[i].tail);
    DirectX::XMVECTOR tail1 = DirectX::XMLoadFloat3(&trailDataCollection[i + 1].tail);
    DirectX::XMVECTOR tail2 = DirectX::XMLoadFloat3(&trailDataCollection[i + 2].tail);
    DirectX::XMVECTOR tail3 = DirectX::XMLoadFloat3(&trailDataCollection[i + 3].tail);

    for (float k = 0; k < 1.0f; k += velvetyRate)
    {
      DirectX::XMVECTOR Head = DirectX::XMVectorCatmullRom(head0, head1, head2, head3, min(k, 1.0f));
      DirectX::XMVECTOR Tail = DirectX::XMVectorCatmullRom(tail0, tail1, tail2, tail3, min(k, 1.0f));

      // ���_�o�b�t�@�ɏ��ǉ�
      AddVertex(Head, DirectX::XMFLOAT2(1.0f, v));
      AddVertex(Tail, DirectX::XMFLOAT2(0.0f, v));

      v += amount;
    }
  }
}
