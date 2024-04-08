#pragma once

#include "Component.h"
#include "Math/OriginMath.h"
#include "Graphics/ShaderStaties.h"
#include "Graphics/Shaders/SpriteShader.h"

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <string>

class FontRenderer;

class CompFont :public Component
{
public:
  // �g�p����t�H���g�̃t�@�C���p�X�A�`�悷�镶���̗�
  CompFont(FontRenderer* renderer, const char* filename, int maxSpriteCount);
  ~CompFont() {}

  // ���O�擾
  virtual const char* GetName() const { return "CompFont"; };

  void Start()override;

  void Update(const float& elapsedTime)override;

  void OnDestroy()override;

  void Render(ID3D11DeviceContext* dc);

  void DrawImGui()override;

  // ���̓����l�͂��̂܂܂ɓ����l�������I�ɕύX
  void EnforceColorAlpha(const float& alpha);

  void SetString(std::wstring str) { string = str; }

  const DirectX::XMFLOAT4 GetColor() const { return color; }
  void SetColor(DirectX::XMFLOAT4 newColor) { color = newColor; }

  const BS_MODE GetBsMode()const { return bsMode; }
  const DS_MODE GetDsMode()const { return dsMode; }
  const RS_MODE GetRsMode()const { return rsMode; }

  void SetBsMode(const BS_MODE& mode) { bsMode = mode; }
  void SetDsMode(const DS_MODE& mode) { dsMode = mode; }
  void SetRsMode(const RS_MODE& mode) { rsMode = mode; }

private:
  void UpdateBuffer(ID3D11DeviceContext* dc);

private:
  FontRenderer* renderer = nullptr;
  std::wstring  string = L"";
  DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };

  Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;
  std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	shaderResourceViews;

  // �`��X�e�[�g
  BS_MODE bsMode = BS_MODE::ALPHA;
  DS_MODE dsMode = DS_MODE::ZT_ON_ZW_ON;
  RS_MODE rsMode = RS_MODE::SOLID_CULL_NONE;

  struct CharacterInfo
  {
    static const WORD			NonCode = 0;			// �R�[�h�Ȃ�
    static const WORD			EndCode = 0xFFFF;		// �I���R�[�h
    static const WORD			ReturnCode = 0xFFFE;	// ���s�R�[�h
    static const WORD			TabCode = 0xFFFD;		// �^�u�R�[�h
    static const WORD			SpaceCode = 0xFFFC;		// �󔒃R�[�h

    float	left;			// �e�N�X�`�������W
    float	top;			// �e�N�X�`������W
    float	right;		// �e�N�X�`���E���W
    float	bottom;		// �e�N�X�`�������W
    float	xOffset;	// �I�t�Z�b�gX
    float	yOffset;	// �I�t�Z�b�gY
    float	xAdvance;	// �v���|�[�V���i����
    float	width;		// �摜��
    float	height;		// �摜����
    int		page;			// �e�N�X�`���ԍ�
    bool	ascii;		// �A�X�L�[����
  };

  float							fontWidth;
  float							fontHeight;
  int								textureCount;
  int								characterCount;
  std::vector<CharacterInfo>		characterInfos;
  std::vector<WORD>				characterIndices;

  struct Subset
  {
    ID3D11ShaderResourceView* shaderResourceView;
    UINT						startIndex;
    UINT						indexCount;
  };
  std::vector<Subset>				subsets;
  std::vector<std::array<SpriteShader::Vertex, 4>> vertexDatas = {};
  UINT							currentIndexCount;
  int								currentPage;
};
