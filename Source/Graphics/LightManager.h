#pragma once

#include <vector>
#include <memory>
#include "Light.h"

#include "Graphics/Texture.h"

// ���C�g�Ǘ��N���X
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

  // ���C�g��o�^����
  void Register(Light* light);

  // ���C�g�̓o�^����������
  void Remove(Light* light);

  // �o�^�ς݂̃��C�g��S�폜����
  void Clear();

  // ���C�g����RenderContext�ɐς�
  void UpdateConstatBuffer();

  // �f�o�b�O���̕\��
  void DrawDebugGUI();

  // �f�o�b�O�v���~�e�B�u�̕\��
  void DrawDebugPrimitive();

  // �X�J�C�}�b�v�̍ēǂݍ���
  void ReloadSkyMap(ID3D11DeviceContext* dc, ID3D11Device* device,const char* folderName);

  // �p�m���}�X�J�C�{�b�N�X�p�̉摜
  const	ID3D11ShaderResourceView* GetSkyboxTexture() const { return skyBox.Get(); }

  // ���C�g�̏��
  int GetLightCount() const { return static_cast<int>(lights.size()); }
  Light* GetLight(const int& index) const { return lights.at(index); }

  // �V���h�E�}�b�v�Ɏg�p���郉�C�g
  Light* GetShadowLight() const { return shadowLight; }
  void SetShadowLight(Light* light) { shadowLight = light; }

private:
  // GPU�ɃX�J�C�{�b�N�X��ݒ肷��
  void SetSkyboxSrv();

private:
  std::vector<Light*>	lights;
  Light* shadowLight = nullptr;

  Microsoft::WRL::ComPtr<ID3D11Buffer>	lightCb; // �萔�o�b�t�@

  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	skyBox; // �X�J�C�{�b�N�X
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	diffuseIem; // �v���t�B���^�����O�σX�J�C�{�b�N�X(Diffuse)
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	specularPmrem; // �v���t�B���^�����O�σX�J�C�{�b�N�X(Specular)
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	lutGgx; // �X�J�C�{�b�N�X�̐F�Ή��\

};
