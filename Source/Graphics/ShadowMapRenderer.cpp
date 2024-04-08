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

  // 定数バッファの作成
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

  // レンダーターゲットの生成
  for (int i = 0; i < SHADOW_MAP_COUNT; ++i) {
    int textureSize = RENDER_TARGET_SIZE / (i + 1);
    shadowMaps[i] = std::make_unique<DepthStencil>(RENDER_TARGET_SIZE, RENDER_TARGET_SIZE);
  }

  // シェーダー生成
  shadowCastShader = std::make_unique<ShadowCastShader>(graphics.GetDevice());
  shadowCastStaticShader = std::make_unique<StaticModelShader>("Shader/StaticShadowCasterVS.cso", nullptr);
}

void ShadowMapRenderer::Render(ModelRenderer* renderer)
{
  ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

  // 元のレンダーターゲットを退避
  CacheRenderTargets(dc);

  // シェーダーでリソースとしてバインド中のレンダーターゲットを解除する
  {
    ID3D11ShaderResourceView* srv[SHADOW_MAP_COUNT] = { nullptr,nullptr,nullptr };
    dc->PSSetShaderResources(14, SHADOW_MAP_COUNT, srv);
  }

  // シャドウマップに描画
  DrawShadowMap(dc, renderer);

  // 元のレンダーターゲットに戻す
  RestoreRenderTargets(dc);

  // シェーダーリソースビューとしてSlot14に登録
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
  // 定数バッファ設定
  Graphics::Instance().GetDeviceContext()->VSSetConstantBuffers(11, 1, shadowCB.GetAddressOf());
  Graphics::Instance().GetDeviceContext()->PSSetConstantBuffers(11, 1, shadowCB.GetAddressOf());

  // カスケードシャドウマップ用の射影行列を計算
  ShadowCB cb{};
  CalcCascadedLVP(cb);

  // ドローコール
  for (int i = 0; i < SHADOW_MAP_COUNT; ++i) {
#pragma region 画面クリア＆レンダーターゲット・ビューポート設定
    // 深度マップをレンダーターゲットに設定
    {
      ID3D11RenderTargetView* rtv = nullptr;
      ID3D11DepthStencilView* dsv = shadowMaps[i]->GetDepthStencilView().Get();
      dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

      dc->OMSetRenderTargets(1, &rtv, dsv);
    }

    // ビューポートの設定
    {
      D3D11_VIEWPORT	viewport{};
      viewport.Width = static_cast<float>(shadowMaps[i]->GetWidth());
      viewport.Height = static_cast<float>(shadowMaps[i]->GetHeight());
      viewport.MinDepth = 0.0f;
      viewport.MaxDepth = 1.0f;
      dc->RSSetViewports(1, &viewport);
    }
#pragma endregion

    // 定数バッファ更新
    cb.index = i;
    Graphics::Instance().GetDeviceContext()->UpdateSubresource(shadowCB.Get(), 0, 0, &cb, 0, 0);

    // 静的モデル描画
    renderer->GetStaticRenderer()->Render(dc, {}, shadowCastStaticShader.get());
    // 動的モデル描画
    renderer->RenderDynamic(dc, {}, shadowCastShader.get());
  }
}

void ShadowMapRenderer::CalcCascadedLVP(ShadowCB& cb)
{
  //	カメラのパラメーターを取得
  Camera& camera = Camera::Instance();
  DirectX::XMVECTOR	CameraForward = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetForward()));
  DirectX::XMVECTOR	CameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetRight()));
  DirectX::XMVECTOR	CameraUp = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&camera.GetUp()));
  DirectX::XMVECTOR	CameraPosition = DirectX::XMLoadFloat3(&camera.GetEye());

  DirectX::XMMATRIX BaseLVP;
#pragma region ライトビュープロジェクション行列を作成
  {
    Light* light = LightManager::Instance().GetShadowLight();

    // ライトビュープロジェクション行列の作成
    DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&light->GetDirection());
    // ライト位置はカメラの少し前に追従
    DirectX::XMVECTOR Position = CameraPosition;
    Position = DirectX::XMVectorAdd(Position, DirectX::XMVectorScale(Direction, -lightDistance));

    // ビュー行列
    DirectX::XMMATRIX V = DirectX::XMMatrixLookToLH(
      Position, Direction,
      DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    // プロジェクション行列 ( 平行光源なので平行投影 )
    DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(drawDistance, drawDistance, Near, Far);

    BaseLVP = V * P;
  }
#pragma endregion

  // 各カスケードエリアのクロップ行列を求め、射影行列に掛ける
  float nearDepth = cascadeArea[0];
  for (int areaNo = 0; areaNo < SHADOW_MAP_COUNT; ++areaNo)
  {
    float farDepth = cascadeArea[areaNo + 1];

    DirectX::XMVECTOR	vertex[8];
#pragma region エリアを内包する視推台の8頂点を算出する
    {
      //	エリアの近平面の中心からの上面までの距離を求める
      float	nearY = tanf(camera.GetFovY() / 2.0f) * nearDepth;

      //	エリアの近平面の中心からの右面までの距離を求める
      //	アスペクト比は縦の長さに対しての横の長さの比率なので、縦の長さにアスペクト比を乗算すると求まる。
      float	nearX = nearY * camera.GetAspect();

      //	エリアの遠平面の中心からの上面までの距離を求める
      float	farY = tanf(camera.GetFovY() / 2.0f) * farDepth;

      //	エリアの遠平面の中心からの右面までの距離を求める
      //	アスペクト比は縦の長さに対しての横の長さの比率なので、縦の長さにアスペクト比を乗算すると求まる。
      float	farX = farY * camera.GetAspect();

      //	エリアの近平面の中心座標を求める
      DirectX::XMVECTOR	nearPosition = DirectX::XMVectorAdd(CameraPosition, DirectX::XMVectorScale(CameraForward, nearDepth));

      //	エリアの遠平面の中心座標を求める
      DirectX::XMVECTOR	farPosition = DirectX::XMVectorAdd(CameraPosition, DirectX::XMVectorScale(CameraForward, farDepth));

      //	8頂点を求める
      {
        // 近平面
        {
          // 上
          DirectX::XMVECTOR nearTop = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorScale(CameraUp, nearY));
          // 下
          DirectX::XMVECTOR nearBottom = DirectX::XMVectorAdd(nearPosition, DirectX::XMVectorScale(CameraUp, -nearY));
          // 右
          DirectX::XMVECTOR nearRight = DirectX::XMVectorScale(CameraRight, nearX);
          // 左
          DirectX::XMVECTOR nearLeft = DirectX::XMVectorScale(CameraRight, -nearX);

          // 近平面の右上
          vertex[0] = DirectX::XMVectorAdd(nearTop, nearRight);
          // 近平面の左上
          vertex[1] = DirectX::XMVectorAdd(nearTop, nearLeft);
          // 近平面の右下
          vertex[2] = DirectX::XMVectorAdd(nearBottom, nearRight);
          // 近平面の左下
          vertex[3] = DirectX::XMVectorAdd(nearBottom, nearLeft);
        }

        // 遠平面
        {
          // 上
          DirectX::XMVECTOR farTop = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorScale(CameraUp, farY));
          // 下
          DirectX::XMVECTOR farBottom = DirectX::XMVectorAdd(farPosition, DirectX::XMVectorScale(CameraUp, -farY));
          // 右
          DirectX::XMVECTOR farRight = DirectX::XMVectorScale(CameraRight, farX);
          // 左
          DirectX::XMVECTOR farLeft = DirectX::XMVectorScale(CameraRight, -farX);

          // 遠平面の右上
          vertex[4] = DirectX::XMVectorAdd(farTop, farRight);
          // 遠平面の左上
          vertex[5] = DirectX::XMVectorAdd(farTop, farLeft);
          // 遠平面の右下
          vertex[6] = DirectX::XMVectorAdd(farBottom, farRight);
          // 遠平面の左下
          vertex[7] = DirectX::XMVectorAdd(farBottom, farLeft);
        }
      }
    }
#pragma endregion

    DirectX::XMFLOAT3	vertexMin(FLT_MAX, FLT_MAX, FLT_MAX), vertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
#pragma region 頂点を変換して最大値,最小値を求める
    {
      DirectX::XMFLOAT3	p;
      for (auto& it : vertex)
      {
        // ライトビュープロジェクション空間に変換
        DirectX::XMStoreFloat3(&p, DirectX::XMVector3TransformCoord(it, BaseLVP));

        // 最大値、最小値を求める
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
#pragma region クロップ行列を求める
    {
      // 拡大率
      float xScale = 2.0f / (vertexMax.x - vertexMin.x);
      float yScale = 2.0f / (vertexMax.y - vertexMin.y);

      // AABBの中心点を求め、中心座標の分だけ平行移動
      float	xOffset = -0.5f * (vertexMax.x + vertexMin.x) * xScale;
      float	yOffset = -0.5f * (vertexMax.y + vertexMin.y) * yScale;

      // クロップ行列
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

    //	ライトビュープロジェクション行列にクロップ行列を乗算
    DirectX::XMStoreFloat4x4(&cb.cascadedLVP[areaNo], BaseLVP * DirectX::XMLoadFloat4x4(&clopMatrix));

    // 次のエリアの近平面までの距離を代入する
    nearDepth = farDepth;
  }

}
