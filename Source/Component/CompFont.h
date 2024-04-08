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
  // 使用するフォントのファイルパス、描画する文字の量
  CompFont(FontRenderer* renderer, const char* filename, int maxSpriteCount);
  ~CompFont() {}

  // 名前取得
  virtual const char* GetName() const { return "CompFont"; };

  void Start()override;

  void Update(const float& elapsedTime)override;

  void OnDestroy()override;

  void Render(ID3D11DeviceContext* dc);

  void DrawImGui()override;

  // 元の透明値はそのままに透明値を強制的に変更
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

  // 描画ステート
  BS_MODE bsMode = BS_MODE::ALPHA;
  DS_MODE dsMode = DS_MODE::ZT_ON_ZW_ON;
  RS_MODE rsMode = RS_MODE::SOLID_CULL_NONE;

  struct CharacterInfo
  {
    static const WORD			NonCode = 0;			// コードなし
    static const WORD			EndCode = 0xFFFF;		// 終了コード
    static const WORD			ReturnCode = 0xFFFE;	// 改行コード
    static const WORD			TabCode = 0xFFFD;		// タブコード
    static const WORD			SpaceCode = 0xFFFC;		// 空白コード

    float	left;			// テクスチャ左座標
    float	top;			// テクスチャ上座標
    float	right;		// テクスチャ右座標
    float	bottom;		// テクスチャ下座標
    float	xOffset;	// オフセットX
    float	yOffset;	// オフセットY
    float	xAdvance;	// プロポーショナル幅
    float	width;		// 画像幅
    float	height;		// 画像高さ
    int		page;			// テクスチャ番号
    bool	ascii;		// アスキー文字
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
