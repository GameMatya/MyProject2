#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/ShaderStaties.h"
#include "Math/OriginMath.h"
#include "Graphics/RenderContext/RenderContext.h"

class PostShader :public ShaderBase
{
public:
  PostShader();
  PostShader(const char* vsFilepath, const char* psFilepath);

  // SceneConstantBuffer( slot 9 )�̍X�V
  static void UpdatePostConstant(ID3D11DeviceContext* dc, const PostProcessContext& rc);

  // �`��J�n
  virtual void Begin(ID3D11DeviceContext* dc);

  // �`��  sceneSrv = �`��Ɏg���V�[�� ( �z��ɑΉ� )
  virtual void Draw(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** sceneSrv, const int ArraySize);

  // �`��I��
  virtual void End(ID3D11DeviceContext* dc);

protected:
  // VS�p��CSO��ǂݍ��� ( �֐��̃��b�v )
  HRESULT createVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader)override
  {
    // ���_�V�F�[�_�[���쐬
    return ShaderBase::createVsFromCso(device, cso_name, vertex_shader, nullptr, nullptr, 0);
  }

private: // �萔�o�b�t�@�֌W
  struct CBLuminanceExtraction
  {
    float				threshold = 0.5f;	    // 臒l
    float				intensity = 1.0f;	    // �u���[���̋��x
    float       maxLuminance = 12.0f; // �ő�P�x
    float       dummy = 0.0f;
  };

  struct CBColorGrading
  {
    float	hueShift;	  // �F������
    float	saturation;	// �ʓx����
    float	brightness;	// ���x����
    float	dummy;
  };

  struct CBDistanceFog
  {
    DirectX::XMFLOAT4 color;
  };

  // �萔�o�b�t�@�p�\����
  struct CbPostProcess
  {
    CBLuminanceExtraction luminance;
    CBColorGrading colorGrading;
    CBDistanceFog distanceFog;
  };

protected:
  inline static Microsoft::WRL::ComPtr<ID3D11Buffer> postConstantBuffer;

};
