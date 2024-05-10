#include "LightManager.h"
#include "Graphics.h"
#include "System/misc.h"
#include <imgui.h>

LightManager::LightManager()
{
  Graphics& graphics = Graphics::Instance();
  ID3D11DeviceContext* dc = graphics.GetDeviceContext();

  // 定数バッファの作成
  D3D11_BUFFER_DESC desc;
  ::memset(&desc, 0, sizeof(desc));
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  desc.ByteWidth = sizeof(LightCB);
  desc.StructureByteStride = 0;

  HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, 0, lightCb.GetAddressOf());
  _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

  // 定数バッファ設定
  dc->VSSetConstantBuffers(8, 1, lightCb.GetAddressOf());
  dc->PSSetConstantBuffers(8, 1, lightCb.GetAddressOf());
  dc->CSSetConstantBuffers(8, 1, lightCb.GetAddressOf());
  dc->GSSetConstantBuffers(8, 1, lightCb.GetAddressOf());
  dc->HSSetConstantBuffers(8, 1, lightCb.GetAddressOf());
  dc->DSSetConstantBuffers(8, 1, lightCb.GetAddressOf());

  // スカイマップ読み込み
  ReloadSkyMap(dc, graphics.GetDevice(), "kloofendal_43d_clear_puresky_2k");
}

LightManager::~LightManager()
{
  Clear();
}

void LightManager::Register(Light* light)
{
  lights.emplace_back(light);
}

void LightManager::Remove(Light* light)
{
  std::vector<Light*>::iterator	it = std::find(lights.begin(), lights.end(), light);
  if (it != lights.end())
  {
    lights.erase(it);
    delete	light;
  }
}

void LightManager::Clear()
{
  for (Light* light : lights)
  {
    delete	light;
  }
  lights.clear();
}

void LightManager::UpdateConstatBuffer()
{
  LightCB cb;

  // 登録されている光源の情報を設定
  for (Light* light : lights)
  {
    light->PushRenderContext(cb);
  }

  Graphics::Instance().GetDeviceContext()->UpdateSubresource(lightCb.Get(), 0, 0, &cb, 0, 0);
}

void LightManager::SetSkyboxSrv()
{
  if (skyBox == nullptr)return;
  ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

  ID3D11ShaderResourceView* srvs[] = {
    skyBox.Get(),
    diffuseIem.Get(),
    specularPmrem.Get(),
    lutGgx.Get()
  };

  dc->PSSetShaderResources(10, ARRAYSIZE(srvs), srvs);
}

void LightManager::DrawDebugGUI()
{
  if (ImGui::TreeNode("Lights"))
  {
    int nodeId = 0;
    for (Light* light : lights)
    {
      ImGui::PushID(nodeId++);
      light->DrawDebugGUI();
      ImGui::PopID();
    }
    ImGui::TreePop();
  }
}

void LightManager::DrawDebugPrimitive()
{
  for (Light* light : lights)
  {
    light->DrawDebugPrimitive();
  }
}

void LightManager::ReloadSkyMap(ID3D11DeviceContext* dc, ID3D11Device* device, const char* folderName)
{
  std::string filepath = "Data/SkyBox/";
  filepath += folderName;
  filepath += "/";

  // スカイボックスを読み込む
  Texture::LoadTexture((filepath + folderName + ".DDS").c_str(), device, skyBox.GetAddressOf());
  Texture::LoadTexture((filepath + "diffuse_iem.dds").c_str(), device, diffuseIem.GetAddressOf());
  Texture::LoadTexture((filepath + "specular_pmrem.dds").c_str(), device, specularPmrem.GetAddressOf());
  Texture::LoadTexture("Data/SkyBox/lut_ggx.DDS", device, lutGgx.GetAddressOf());

  SetSkyboxSrv();
}
