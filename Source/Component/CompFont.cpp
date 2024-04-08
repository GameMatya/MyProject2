#include "CompFont.h"
#include "Graphics/FontRenderer.h"
#include "System/framework.h"
#include "System/Misc.h"

#include <imgui.h>
#include <WICTextureLoader.h>

CompFont::CompFont(FontRenderer* renderer, const char* filename, int maxSpriteCount) :renderer(renderer)
{
  ID3D11Device* device = Graphics::Instance().GetDevice();

  vertexDatas.resize(maxSpriteCount);

  // 頂点バッファ
  {
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(SpriteShader::Vertex) * maxSpriteCount * 4);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, vertexBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // インデックスバッファ
  {
    D3D11_BUFFER_DESC bufferDesc = {};
    D3D11_SUBRESOURCE_DATA subresourceData = {};

    std::unique_ptr<UINT[]> indices = std::make_unique<UINT[]>(maxSpriteCount * 6);
    // 四角形 を 三角形 ２つに展開
    // 0---1      0---1  4
    // |   |  →  |／  ／|
    // 2---3      2  3---5
    {
      UINT* p = indices.get();
      for (int i = 0; i < maxSpriteCount * 4; i += 4)
      {
        p[0] = i + 0;
        p[1] = i + 1;
        p[2] = i + 2;
        p[3] = i + 2;
        p[4] = i + 1;
        p[5] = i + 3;
        p += 6;
      }
    }

    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * maxSpriteCount * 6);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    subresourceData.pSysMem = indices.get();
    subresourceData.SysMemPitch = 0; //Not use for index buffers.
    subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
    HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, indexBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
  }

  // .fnt 読み込み
  {
    FILE* fp = nullptr;
    fopen_s(&fp, filename, "rb");
    _ASSERT_EXPR_A(fp, "FNT File not found");

    fseek(fp, 0, SEEK_END);
    long fntSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<char[]> fntData = std::make_unique<char[]>(fntSize);
    fread(fntData.get(), fntSize, 1, fp);
    fclose(fp);

    // (***.fnt)ヘッダ解析
    char* context = nullptr;	// 内部的に使用するので深く考えない
    const char* delimiter = " ,=\"\r\n";
    const char* delimiter2 = ",=\"\r\n";
    char* pToken = ::strtok_s(fntData.get(), delimiter, &context);
    _ASSERT_EXPR_A(::strcmp(pToken, "info") == 0, "FNT Format");

    // face
    ::strtok_s(nullptr, delimiter, &context);
    const char* face = ::strtok_s(nullptr, delimiter2, &context);
    // size
    ::strtok_s(nullptr, delimiter, &context);
    int size = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // bold
    ::strtok_s(nullptr, delimiter, &context);
    int bold = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // italic
    ::strtok_s(nullptr, delimiter, &context);
    int italic = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // charset
    ::strtok_s(nullptr, delimiter, &context);
    const char* charset = ::strtok_s(nullptr, delimiter, &context);
    // unicode
    if (::strcmp(charset, "unicode") == 0)
      charset = "";
    else
      ::strtok_s(nullptr, delimiter, &context);
    int unicode = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // stretchH
    ::strtok_s(nullptr, delimiter, &context);
    int stretchH = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // smooth
    ::strtok_s(nullptr, delimiter, &context);
    int smooth = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // aa
    ::strtok_s(nullptr, delimiter, &context);
    int aa = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // padding
    ::strtok_s(nullptr, delimiter, &context);
    int padding1 = ::atoi(::strtok_s(nullptr, delimiter, &context));
    int padding2 = ::atoi(::strtok_s(nullptr, delimiter, &context));
    int padding3 = ::atoi(::strtok_s(nullptr, delimiter, &context));
    int padding4 = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // spacing
    ::strtok_s(nullptr, delimiter, &context);
    int spacing1 = ::atoi(::strtok_s(nullptr, delimiter, &context));
    int spacing2 = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // outline
    ::strtok_s(nullptr, delimiter, &context);
    int outline = ::atoi(::strtok_s(nullptr, delimiter, &context));

    // common
    ::strtok_s(nullptr, delimiter, &context);
    // lineHeight
    ::strtok_s(nullptr, delimiter, &context);
    int lineHeight = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // base
    ::strtok_s(nullptr, delimiter, &context);
    int base = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // scaleW
    ::strtok_s(nullptr, delimiter, &context);
    int scaleW = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // scaleH
    ::strtok_s(nullptr, delimiter, &context);
    int scaleH = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // pages
    ::strtok_s(nullptr, delimiter, &context);
    int pages = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // packed
    ::strtok_s(nullptr, delimiter, &context);
    int packed = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // alphaChnl
    ::strtok_s(nullptr, delimiter, &context);
    int alphaChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // redChnl
    ::strtok_s(nullptr, delimiter, &context);
    int redChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // greenChnl
    ::strtok_s(nullptr, delimiter, &context);
    int greenChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));
    // blueChnl
    ::strtok_s(nullptr, delimiter, &context);
    int blueChnl = ::atoi(::strtok_s(nullptr, delimiter, &context));

    // ディレクトリパス取得
    char dirname[256];
    ::_splitpath_s(filename, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);
    shaderResourceViews.resize(pages);
    for (int i = 0; i < pages; i++)
    {
      // page
      ::strtok_s(nullptr, delimiter, &context);
      // id
      ::strtok_s(nullptr, delimiter, &context);
      int id = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // file
      ::strtok_s(nullptr, delimiter, &context);
      const char* file = ::strtok_s(nullptr, delimiter2, &context);

      // 相対パスの解決
      char fname[256];
      ::_makepath_s(fname, 256, nullptr, dirname, file, nullptr);

      // マルチバイト文字からワイド文字へ変換
      size_t length;
      wchar_t wfname[256];
      ::mbstowcs_s(&length, wfname, 256, fname, _TRUNCATE);

      // テクスチャ読み込み
      Microsoft::WRL::ComPtr<ID3D11Resource> resource;
      HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfname, resource.GetAddressOf(), shaderResourceViews.at(i).GetAddressOf());
      _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }

    // chars
    ::strtok_s(nullptr, delimiter, &context);
    // count
    ::strtok_s(nullptr, delimiter, &context);
    int count = ::atoi(::strtok_s(nullptr, delimiter, &context));

    // データ格納
    fontWidth = static_cast<float>(size);
    fontHeight = static_cast<float>(lineHeight);
    textureCount = pages;
    characterCount = count + 1;
    characterInfos.resize(characterCount);
    characterIndices.resize(0xFFFF);
    ::memset(characterIndices.data(), 0, sizeof(WORD) * characterIndices.size());

    characterIndices.at(0x00) = CharacterInfo::EndCode;
    characterIndices.at(0x0a) = CharacterInfo::ReturnCode;
    characterIndices.at(0x09) = CharacterInfo::TabCode;
    characterIndices.at(0x20) = CharacterInfo::SpaceCode;

    // 文字情報解析
    int n = 1;
    for (int i = 0; i < count; i++)
    {
      // char
      ::strtok_s(nullptr, delimiter, &context);
      // id
      ::strtok_s(nullptr, delimiter, &context);
      int id = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // x
      ::strtok_s(nullptr, delimiter, &context);
      int x = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // y
      ::strtok_s(nullptr, delimiter, &context);
      int y = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // width
      ::strtok_s(nullptr, delimiter, &context);
      int width = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // height
      ::strtok_s(nullptr, delimiter, &context);
      int height = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // xoffset
      ::strtok_s(nullptr, delimiter, &context);
      int xoffset = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // yoffset
      ::strtok_s(nullptr, delimiter, &context);
      int yoffset = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // xadvance
      ::strtok_s(nullptr, delimiter, &context);
      int xadvance = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // page
      ::strtok_s(nullptr, delimiter, &context);
      int page = ::atoi(::strtok_s(nullptr, delimiter, &context));
      // chnl
      ::strtok_s(nullptr, delimiter, &context);
      int chnl = ::atoi(::strtok_s(nullptr, delimiter, &context));

      // 文字コード格納
      if (id >= 0x10000) continue;

      CharacterInfo& info = characterInfos.at(n);

      characterIndices.at(id) = static_cast<WORD>(i + 1);

      // 文字情報格納
      info.left = static_cast<float>(x) / static_cast<float>(scaleW);
      info.top = static_cast<float>(y) / static_cast<float>(scaleH);
      info.right = static_cast<float>(x + width) / static_cast<float>(scaleW);
      info.bottom = static_cast<float>(y + height) / static_cast<float>(scaleH);
      info.xOffset = static_cast<float>(xoffset);
      info.yOffset = static_cast<float>(yoffset);
      info.xAdvance = static_cast<float>(xadvance);
      info.width = static_cast<float>(width);
      info.height = static_cast<float>(height);
      info.page = page;
      info.ascii = (id < 0x100);

      n++;
    }
  }
}

void CompFont::Start()
{
  renderer->Register(this);

  // DebugとRelease時のウィンドウサイズの差を埋める
#ifndef _DEBUG
  ObjectTransform& transform = GetGameObject()->transform;
  transform.position.x *= SCREEN_DIFFERENCE_SCALE;
  transform.position.y *= SCREEN_DIFFERENCE_SCALE;

#else
  ObjectTransform& transform = GetGameObject()->transform;
  transform.scale /= SCREEN_DIFFERENCE_SCALE;

#endif // _DEBUG

}

void CompFont::Update(const float& elapsedTime)
{
  if (string.empty())return;
  currentIndexCount = 0;
  currentPage = -1;
  subsets.clear();

  size_t length = string.size();
  ObjectTransform& transform = gameObject.lock()->transform;

  DirectX::XMFLOAT2 position = { transform.position.x ,transform.position.y };
  float startX = position.x;
  float startY = position.y;
  float space = fontWidth;

  for (size_t i = 0; i < length; ++i)
  {
    // 文字値から文字情報が格納されているコードを取得
    WORD word = static_cast<WORD>(string[i]);
    WORD code = characterIndices.at(word);

    // 特殊制御用コードの処理
    if (code == CharacterInfo::EndCode)
    {
      break;
    }
    else if (code == CharacterInfo::ReturnCode)
    {
      position.x = startX;
      position.y += fontHeight * transform.scale.y;
      continue;
    }
    else if (code == CharacterInfo::TabCode)
    {
      position.x += space * 4;
      continue;
    }
    else if (code == CharacterInfo::SpaceCode)
    {
      position.x += space;
      continue;
    }

    // 文字情報を取得し、頂点データを編集
    const CharacterInfo& info = characterInfos.at(code);

    float positionX = position.x + info.xOffset; // + 0.5f;
    float positionY = position.y + info.yOffset; // + 0.5f;

    // 0---1
    // |   |
    // 2---3
#if(1)
    vertexDatas[i][0].position.x = positionX;
    vertexDatas[i][0].position.y = positionY;
    vertexDatas[i][0].position.z = 0.0f;
    vertexDatas[i][0].texcoord.x = info.left;
    vertexDatas[i][0].texcoord.y = info.top;
    vertexDatas[i][0].color.x = color.x;
    vertexDatas[i][0].color.y = color.y;
    vertexDatas[i][0].color.z = color.z;
    vertexDatas[i][0].color.w = color.w;

    vertexDatas[i][1].position.x = positionX + (info.width * transform.scale.x);
    vertexDatas[i][1].position.y = positionY;
    vertexDatas[i][1].position.z = 0.0f;
    vertexDatas[i][1].texcoord.x = info.right;
    vertexDatas[i][1].texcoord.y = info.top;
    vertexDatas[i][1].color.x = color.x;
    vertexDatas[i][1].color.y = color.y;
    vertexDatas[i][1].color.z = color.z;
    vertexDatas[i][1].color.w = color.w;

    vertexDatas[i][2].position.x = positionX;
    vertexDatas[i][2].position.y = positionY + (info.height * transform.scale.y);
    vertexDatas[i][2].position.z = 0.0f;
    vertexDatas[i][2].texcoord.x = info.left;
    vertexDatas[i][2].texcoord.y = info.bottom;
    vertexDatas[i][2].color.x = color.x;
    vertexDatas[i][2].color.y = color.y;
    vertexDatas[i][2].color.z = color.z;
    vertexDatas[i][2].color.w = color.w;

    vertexDatas[i][3].position.x = positionX + (info.width * transform.scale.x);
    vertexDatas[i][3].position.y = positionY + (info.height * transform.scale.y);
    vertexDatas[i][3].position.z = 0.0f;
    vertexDatas[i][3].texcoord.x = info.right;
    vertexDatas[i][3].texcoord.y = info.bottom;
    vertexDatas[i][3].color.x = color.x;
    vertexDatas[i][3].color.y = color.y;
    vertexDatas[i][3].color.z = color.z;
    vertexDatas[i][3].color.w = color.w;
#endif
    // NDC座標変換
    for (int j = 0; j < 4; ++j)
    {
      vertexDatas[i][j].position.x = 2.0f * vertexDatas[i][j].position.x / SCREEN_WIDTH - 1.0f;
      vertexDatas[i][j].position.y = 1.0f - 2.0f * vertexDatas[i][j].position.y / SCREEN_HEIGHT;
    }

    position.x += info.xAdvance * transform.scale.x;

    // テクスチャが切り替わる度に描画する情報を設定
    if (currentPage != info.page)
    {
      currentPage = info.page;

      Subset subset;
      subset.shaderResourceView = shaderResourceViews.at(info.page).Get();
      subset.startIndex = currentIndexCount;
      subset.indexCount = 0;
      subsets.emplace_back(subset);
    }
    currentIndexCount += 6;
  }
}

void CompFont::OnDestroy()
{
  renderer->Remove(this);
}

void CompFont::UpdateBuffer(ID3D11DeviceContext* dc)
{
  D3D11_MAPPED_SUBRESOURCE mapped_subresource;
  dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);

  SpriteShader::Vertex* currentVertex = reinterpret_cast<SpriteShader::Vertex*>(mapped_subresource.pData);
  for (int i = 0; i < string.size(); ++i) {
    currentVertex[0 + 4 * i] = vertexDatas[i][0];
    currentVertex[1 + 4 * i] = vertexDatas[i][1];
    currentVertex[2 + 4 * i] = vertexDatas[i][2];
    currentVertex[3 + 4 * i] = vertexDatas[i][3];
  }

  // 頂点編集終了
  dc->Unmap(vertexBuffer.Get(), 0);
}

void CompFont::Render(ID3D11DeviceContext* dc)
{
  // 何も描画するものが無かったらリターン
  if (subsets.size() == 0)return;

  // 頂点バッファ更新
  UpdateBuffer(dc);

  // サブセット調整
  size_t size = subsets.size();
  for (size_t i = 1; i < size; ++i)
  {
    Subset& prev = subsets.at(i - 1);
    Subset& next = subsets.at(i);
    prev.indexCount = next.startIndex - prev.startIndex;
  }
  Subset& last = subsets.back();
  last.indexCount = currentIndexCount - last.startIndex;

  // 頂点バッファ設定
  UINT stride = sizeof(SpriteShader::Vertex);
  UINT offset = 0;
  dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
  dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
  dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // 描画
  for (size_t i = 0; i < size; ++i)
  {
    Subset& subset = subsets.at(i);

    dc->PSSetShaderResources(0, 1, &subset.shaderResourceView);
    dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
  }
}

void CompFont::DrawImGui()
{
  int bsID = static_cast<int>(bsMode);
  ImGui::Combo("Blend State", &bsID, "NONE\0ALPHA\0ADD\0SUB\0REPLACE\0MULTI\0LIGHT\0DARK\0SCREEN\0\0");
  bsMode = static_cast<BS_MODE>(bsID);

  int dsID = static_cast<int>(dsMode);
  ImGui::Combo("Depth State", &dsID, "NONE\0ZT_ON_ZW_ON\0ZT_ON_ZW_OFF\0ZT_OFF_ZW_ON\0ZT_OFF_ZW_OFF\0MASK\0APPLY_MASK\0EXCLUSIVE\0\0");
  dsMode = static_cast<DS_MODE>(dsID);

  int rsID = static_cast<int>(rsMode);
  ImGui::Combo("Rasterize State", &rsID, "NONE\0BACK\0FRONT\0\0");
  rsMode = static_cast<RS_MODE>(rsID);

  ImGui::ColorEdit4("Color", &color.x);
}

void CompFont::EnforceColorAlpha(const float& alpha)
{
  size_t length = string.size();

  for (size_t i = 0; i < length; ++i)
  {
    // 0---1
    // |   |
    // 2---3
#if(1)
    vertexDatas[i][0].color.w = alpha;
    vertexDatas[i][1].color.w = alpha;
    vertexDatas[i][2].color.w = alpha;
    vertexDatas[i][3].color.w = alpha;
#endif
  }
}
