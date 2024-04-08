#include "ShadowMapRenderer.h"
#include "Graphics.h"
#include "ModelRenderer.h"
#include "LightManager.h"
#include "Camera/Camera.h"
#include "Math/OriginMath.h"
#include "System/misc.h"
#include "imgui.h"

ShadowMapRenderer::ShadowMapRenderer()
{
  Graphics& graphics = Graphics::Instance();

  // �萔�o�b�t�@�̍쐬
  {
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(ShadowCB);
    desc.StructureByteStride = 0;

    HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, nullptr, shadowCB.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // �����_�[�^�[�Q�b�g�̐���
  for (int i = 0; i < SHADOW_MAP_COUNT; ++i) {
    int textureSize = RENDER_TARGET_SIZE / (i + 1);
    shadowMaps[i] = std::make_unique<DepthStencil>(RENDER_TARGET_SIZE, RENDER_TARGET_SIZE);
  }

  // �V�F�[�_�[����
  shadowCastShader = std::make_unique<ShadowCastShader>(graphics.GetDevice());
  shadowCastStaticShader = std::make_unique<StaticModelShader>("Shader/StaticShadowCasterVS.cso", nullptr);
}

void ShadowMapRenderer::Render(ModelRenderer* renderer)
{
  ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

  // ���̃����_�[�^�[�Q�b�g��ޔ�
  CacheRenderTargets(dc);

  // �V�F�[�_�[�Ń��\�[�X�Ƃ��ăo�C���h���̃����_�[�^�[�Q�b�g����������
  {
    ID3D11ShaderResourceView* srv[SHADOW_MAP_COUNT] = { nullptr,nullptr,nullptr };
    dc->PSSetShaderResources(14, SHADOW_MAP_COUNT, srv);
  }

  // �V���h�E�}�b�v�ɕ`��
  DrawShadowMap(dc, renderer);

  // ���̃����_�[�^�[�Q�b�g�ɖ߂�
  RestoreRenderTargets(dc);

  // �V�F�[�_�[���\�[�X�r���[�Ƃ���Slot14�ɓo�^
  {
    ID3D11ShaderResourceView* srv[SHADOW_MAP_COUNT] = {
      shadowMaps[0]->GetShaderResourceView().Get(),
      shadowMaps[1]->GetShaderResourceView().Get(),
      shadowMaps[2]->GetShaderResourceView().Get()
    };
    dc->PSSetShaderResources(14, SHADOW_MAP_COUNT, srv);
  }
}

void ShadowMapRenderer::DrawImGui()
{
  ImGui::Begin("Shadow Map");
  ImGui::DragFloat("Distance", &lightDistance);
  ImGui::DragFloat4("AreaRange", cascadeArea);

  for (int i = 0; i < SHADOW_MAP_COUNT; ++i) {
    ImGui::Image(shadowMaps[i].get()->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
  }
  ImGui::End();
}

void ShadowMapRenderer::DrawShadowMap(ID3D11DeviceContext* dc, ModelRenderer* renderer)
{
  // �萔�o�b�t�@�ݒ�
  Graphics::Instance().GetDeviceContext()->VSSetConstantBuffers(11, 1, shadowCB.GetAddressOf());
  Graphics::Instance().GetDeviceContext()->PSSetConstantBuffers(11, 1, shadowCB.GetAddressOf());

  // �J�X�P�[�h�V���h�E�}�b�v�p�̎ˉe�s����v�Z
  ShadowCB cb{};
  CalcCascadedLVP(cb);

  // �h���[�R�[��
  for (int i = 0; i < SHADOW_MAP_COUNT; ++i) {
#pragma region ��ʃN���A�������_�[�^�[�Q�b�g�E�r���[�|�[�g�ݒ�
    // �[�x�}�b�v�������_�[�^�[�Q�b�g�ɐݒ�
    {
      ID3D11RenderTargetView* rtv = nullptr;
      ID3D11DepthStencilView* dsv = shadowMaps[i]->GetDepthStencilView().Get();
      dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

      dc->OMSetRenderTargets(1, &rtv, dsv);
    }

    // �r���[�|�[�g�̐ݒ�
    {
      D3D11_VIEWPORT	viewport{};
      viewport.Width = static_cast<float>(shadowMaps[i]->GetWidth());
      viewport.Height = static_cast<float>(shadowMaps[i]->GetHeight());
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      dc->RSSetViewports(1, &viewport);
    }
#pragma endregion

    // �萔�o�b�t�@�X�V
    cb.index = i;
    Graphics::Instance().GetDeviceContext()->UpdateSubresource(shadowCB.Get(), 0, 0, &cb, 0, 0);

    // �ÓI���f���`��
    renderer->GetStaticRenderer()->Render(dc, {}, shadowCastStaticShader.get());
    // ���I���f���`��
    renderer->RenderDynamic(dc, {}, shadowCastShader.get());
  }
}

void ShadowMapRenderer::CalcCascadedLVP(ShadowCB& cb)
{
  //	�J�����̃p�����[�^�[���擾
  Camera& camera = Camera::Instance();
  DirectX::XMVECTOR	CameraForward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetForward()));
  DirectX::XMVECTOR	CameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetRight()));
  DirectX::XMVECTOR	CameraUp = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetUp()));
  DirectX::XMVECTOR	CameraPosition = DirectX::XMLoadFloat3(&camera.GetEye());

  DirectX::XMMATRIX BaseLVP;
#pragma region ���C�g�r���[�v���W�F�N�V�����s����쐬
  {
    Light* light = LightManager::Instance().GetShadowLight();

    // ���C�g�r���[�v���W�F�N�V�����s��̍쐬
    DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&light->GetDirection());
    // ���C�g�ʒu�̓J�����̏����O�ɒǏ]
    DirectX::XMVECTOR Position = CameraPosition;
    Position = DirectX::XMVectorAdd(Position, DirectX::XMVectorScale(Direction, -lightDistance));

    // �r���[�s��
    DirectX::XMMATRIX V = DirectX::XMMatrixLookToLH(
      Position, Direction,
      DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    // �v���W�F�N�V�����s�� ( ���s�����Ȃ̂ŕ��s���e )
    DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(drawDistance, drawDistance, Near, Far);

    BaseLVP = V * P;
  }
#pragma endregion

  // �e�J�X�P�[�h�G���A�̃N���b�v�s������߁A�ˉe�s��Ɋ|����
  float nearDepth = cascadeArea[0];
  for (int areaNo = 0; areaNo < SHADOW_MAP_COUNT; ++areaNo)
  {
    float farDepth = cascadeArea[areaNo + 1];

    DirectX::XMVECTOR	vertex[8];
#pragma region �G���A�����鎋�����8���_���Z�o����
    {
      //	�G���A�̋ߕ��ʂ̒��S����̏�ʂ܂ł̋��������߂�
      float	nearY = tanf(camera.GetFovY() / 2.0f) * nearDepth;

      //	�G���A�̋ߕ��ʂ̒��S����̉E�ʂ܂ł̋��������߂�
      //	�A�X�y�N�g��͏c�̒����ɑ΂��Ẳ��̒����̔䗦�Ȃ̂ŁA�c�̒����ɃA�X�y�N�g�����Z����Ƌ��܂�B
      float	nearX = nearY * camera.GetAspect();

      //	�G���A�̉����ʂ̒��S����̏�ʂ܂ł̋��������߂�
      float	farY = tanf(camera.GetFovY() / 2.0f) * farDepth;

      //	�G���A�̉����ʂ̒��S����̉E�ʂ܂ł̋��������߂�
      //	�A�X�y�N�g��͏c�̒����ɑ΂��Ẳ��̒����̔䗦�Ȃ̂ŁA�c�̒����ɃA�X�y�N�g�����Z����Ƌ��܂�B
      float	farX = farY * camera.GetAspect();

      //	�G���A�̋ߕ��ʂ̒��S���W�����߂�
      DirectX::XMVECTOR	nearPosition = DirectX::XMVectorAdd(CameraPosition, DirectX::XMVectorScale(CameraForward, nearDepth));

      //	�G���A�̉����ʂ̒��S���W�����߂�
      DirectX::XMVECTOR	farPosition = DirectX::XMVectorAdd(CameraPosition, DirectX::XMVectorScale(CameraForward, farDepth));

      //	8���_�����߂�
      {
        // �ߕ���
        {
          // ��
          DirectX::XMVECTOR nearTop = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorScale(CameraUp, nearY));
          // ��
          DirectX::XMVECTOR nearBottom = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorScale(CameraUp, -nearY));
          // �E
          DirectX::XMVECTOR nearRight = DirectX::XMVectorScale(CameraRight, nearX);
          // ��
          DirectX::XMVECTOR nearLeft = DirectX::XMVectorScale(CameraRight, -nearX);

          // �ߕ��ʂ̉E��
          vertex[0] = DirectX::XMVectorAdd(nearTop, nearRight);
          // �ߕ��ʂ̍���
          vertex[1] = DirectX::XMVectorAdd(nearTop, nearLeft);
          // �ߕ��ʂ̉E��
          vertex[2] = DirectX::XMVectorAdd(nearBottom, nearRight);
          // �ߕ��ʂ̍���
          vertex[3] = DirectX::XMVectorAdd(nearBottom, nearLeft);
        }

        // ������
        {
          // ��
          DirectX::XMVECTOR farTop = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorScale(CameraUp, farY));
          // ��
          DirectX::XMVECTOR farBottom = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorScale(CameraUp, -farY));
          // �E
          DirectX::XMVECTOR farRight = DirectX::XMVectorScale(CameraRight, farX);
          // ��
          DirectX::XMVECTOR farLeft = DirectX::XMVectorScale(CameraRight, -farX);

          // �����ʂ̉E��
          vertex[4] = DirectX::XMVectorAdd(farTop, farRight);
          // �����ʂ̍���
          vertex[5] = DirectX::XMVectorAdd(farTop, farLeft);
          // �����ʂ̉E��
          vertex[6] = DirectX::XMVectorAdd(farBottom, farRight);
          // �����ʂ̍���
          vertex[7] = DirectX::XMVectorAdd(farBottom, farLeft);
        }
      }
    }
#pragma endregion

    DirectX::XMFLOAT3	vertexMin(FLT_MAX, FLT_MAX, FLT_MAX), vertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
#pragma region ���_��ϊ����čő�l,�ŏ��l�����߂�
    {
      DirectX::XMFLOAT3	p;
      for (auto& it : vertex)
      {
        // ���C�g�r���[�v���W�F�N�V������Ԃɕϊ�
        DirectX::XMStoreFloat3(&p, DirectX::XMVector3TransformCoord(it, BaseLVP));

        // �ő�l�A�ŏ��l�����߂�
        vertexMin.x = min(p.x, vertexMin.x);
        vertexMin.y = min(p.y, vertexMin.y);
        vertexMin.z = min(p.z, vertexMin.z);
        vertexMax.x = max(p.x, vertexMax.x);
        vertexMax.y = max(p.y, vertexMax.y);
        vertexMax.z = max(p.z, vertexMax.z);
      }
    }
#pragma endregion

    DirectX::XMFLOAT4X4	clopMatrix;
#pragma region �N���b�v�s������߂�
    {
      // �g�嗦
      float xScale = 2.0f / (vertexMax.x - vertexMin.x);
      float yScale = 2.0f / (vertexMax.y - vertexMin.y);

      // AABB�̒��S�_�����߁A���S���W�̕��������s�ړ�
      float	xOffset = -0.5f * (vertexMax.x + vertexMin.x) * xScale;
      float	yOffset = -0.5f * (vertexMax.y + vertexMin.y) * yScale;

      // �N���b�v�s��
      clopMatrix._11 = xScale;
      clopMatrix._12 = 0;
      clopMatrix._13 = 0;
      clopMatrix._14 = 0;
      clopMatrix._21 = 0;
      clopMatrix._22 = yScale;
      clopMatrix._23 = 0;
      clopMatrix._24 = 0;
      clopMatrix._31 = 0;
      clopMatrix._32 = 0;
      clopMatrix._33 = 1;
      clopMatrix._34 = 0;
      clopMatrix._41 = xOffset;
      clopMatrix._42 = yOffset;
      clopMatrix._43 = 0;
      clopMatrix._44 = 1;
    }
#pragma endregion

    //	���C�g�r���[�v���W�F�N�V�����s��ɃN���b�v�s�����Z
    DirectX::XMStoreFloat4x4(&cb.cascadedLVP[areaNo], BaseLVP * DirectX::XMLoadFloat4x4(&clopMatrix));

    // ���̃G���A�̋ߕ��ʂ܂ł̋�����������
    nearDepth = farDepth;
  }

}
