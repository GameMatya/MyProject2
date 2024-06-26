#pragma once

#include <vector>
#include <memory>
#include "Light.h"

#include "Graphics/Texture.h"

// ライト管理クラス
class LightManager
{
  LightManager();
  ~LightManager();

public:
  static LightManager& Instance()
  {
    static	LightManager	lightManager;
    return	lightManager;
  }

  // ライトを登録する
  void Register(Light* light);

  // ライトの登録を解除する
  void Remove(Light* light);

  // 登録済みのライトを全削除する
  void Clear();

  // ライト情報をRenderContextに積む
  void UpdateConstatBuffer();

  // デバッグ情報の表示
  void DrawDebugGUI();

  // デバッグプリミティブの表示
  void DrawDebugPrimitive();

  // スカイマップの再読み込み
  void ReloadSkyMap(ID3D11DeviceContext* dc, ID3D11Device* device,const char* folderName);

  // パノラマスカイボックス用の画像
  const	ID3D11ShaderResourceView* GetSkyboxTexture() const { return skyBox.Get(); }

  // ライトの情報
  int GetLightCount() const { return static_cast<int>(lights.size()); }
  Light* GetLight(const int& index) const { return lights.at(index); }

  // シャドウマップに使用するライト
  Light* GetShadowLight() const { return shadowLight; }
  void SetShadowLight(Light* light) { shadowLight = light; }

private:
  // GPUにスカイボックスを設定する
  void SetSkyboxSrv();

private:
  std::vector<Light*>	lights;
  Light* shadowLight = nullptr;

  Microsoft::WRL::ComPtr<ID3D11Buffer>	lightCb; // 定数バッファ

  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	skyBox; // スカイボックス
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	diffuseIem; // プレフィルタリング済スカイボックス(Diffuse)
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	specularPmrem; // プレフィルタリング済スカイボックス(Specular)
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	lutGgx; // スカイボックスの色対応表

};
