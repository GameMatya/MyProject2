#include "Scene.h"
#include "Math/OriginMath.h"
#include "Graphics/Graphics.h"
#include "Graphics/LightManager.h"
#include "Graphics/Sprite2DRenderer.h"
#include "Graphics/Sprite3DRenderer.h"
#include "Graphics/ParticleManager.h"
#include "SceneRenderer.h"

#include <imgui.h>
#include <imgui_internal.h>

using namespace DirectX;

Scene::Scene() :objectManager(this)
{
  // �t�F�[�h�C���E�A�E�g
  {
    GameObject* fadeInOut = objectManager.Create(TASK_LAYER::FIRST).get();
    fadeSprite = fadeInOut->AddComponent<CompBooleanSprite>(&fadeOutFlag, 1.6f);
    fadeInOut->transform.scale = { 1920,1080,0 }; // ��ʃT�C�Y
    CompSprite2D* sprite = fadeInOut->AddComponent<CompSprite2D>(&sprite2DRenderer).get();
    sprite->SetColor({ 0.0f,0.0f,0.0f,1.0f });
    sprite->SetDrawOrder(10);
    sprite->SetDsMode(DS_MODE::ZT_OFF_ZW_ON);
  }
}

void Scene::Finalize()
{
  sprite2DRenderer.Clear();
  sprite3DRenderer.Clear();
  objectManager.Finalize();
}

void Scene::Render()
{
  Graphics& graphics = Graphics::Instance();

  ID3D11DeviceContext* dc = graphics.GetDeviceContext();
  SceneRenderer& sceneRenderer = SceneRenderer::Instance();

  // ��ʃN���A&�����_�[�^�[�Q�b�g�̐ݒ�
  {
    ID3D11RenderTargetView* rtv;
#ifdef _DEBUG
    // �f�o�b�O���[�h�ł́ASceneView�p��RenderTarget���g��
    rtv = graphics.GetDebugRenderTarget()->GetRenderTargetView().Get();
#else
    rtv = graphics.GetRenderTargetView();
#endif // _DEBUG

    ID3D11DepthStencilView* dsv = Graphics::Instance().GetDepthStencilView();

    // �����_�[�^�[�Q�b�g�̂݉�ʃN���A
    FLOAT color[] = { 0.0f, 1.0f, 0.0f, 1.0f };	// RGBA(0.0�`1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);
  }

  // �ėp�萔�o�b�t�@�̍X�V
  sceneRenderer.UpdateConstants(dc);

  if (modelRenderer.ModelCount() > 0) {
    // ��&3D�I�u�W�F�N�g�`��
    sceneRenderer.Render3D(dc, this);

    // �|�X�g�v���Z�X
    sceneRenderer.RenderPostprocess(dc, GetPostShaders(), GetPostContext());
  }

  // �X�v���C�g�`��
  sprite2DRenderer.Render(dc);
  fontRenderer.Render(dc);

  // Debug���[�h�̂ݒʂ�
#ifdef _DEBUG
  // �����_�[�^�[�Q�b�g���o�b�N�o�b�t�@�ɕύX
  {
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    FLOAT color[] = { 1.0f,0.0f,1.0f,1.0f };  // RGBA(0.0~1.0)
    dc->ClearRenderTargetView(rtv, color);
    ID3D11DepthStencilView* dsv = Graphics::Instance().GetDepthStencilView();
    dc->OMSetRenderTargets(1, &rtv, dsv);
  }

  // SceneView�̕`��
  {
    ImGui::Begin("SceneView");
    ImVec2 pos = { ImGui::GetWindowPos().x,ImGui::GetWindowPos().y + ImGui::GetCurrentWindow()->TitleBarHeight() };

    ImVec2 size = ImGui::GetWindowSize();
    size.y -= ImGui::GetCurrentWindow()->TitleBarHeight();
    if (size.y * 1.778f <= size.x)size.x = size.y * 1.778f;
    if (size.x * 0.563f <= size.y)size.y = size.x * 0.563f;

    auto drawlist = ImGui::GetWindowDrawList();

    size = { pos.x + size.x,pos.y + size.y };
    drawlist->AddImage(graphics.GetDebugRenderTarget()->GetShaderResourceView().Get(), pos, size);

    ImGui::End();
  }

  // ImGui�̕`��
  if (ImGui::BeginMainMenuBar())
  {
    // �t�@�C�����j���[
    if (ImGui::BeginMenu("MENU"))
    {
      bool check = false;

      if (ImGui::MenuItem("Save Texture", "", &check))
      {
        SaveTextureFile();
      }

      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  ImGuiRender();

  modelRenderer.GetShadowMapRenderer()->DrawImGui();

  ImGui::Begin("SceneProperty");
  LightManager::Instance().DrawDebugGUI();

  if (ImGui::TreeNode("PostProcess"))
  {
    if (ImGui::TreeNode("Bloom"))
    {
      ImGui::SliderFloat("Max Luminance", &postContext.luminanceExtractionData.maxLuminance, 5.0f, 20.0f);
      ImGui::SliderFloat("threshold", &postContext.luminanceExtractionData.threshold, 0.0f, postContext.luminanceExtractionData.maxLuminance);
      ImGui::SliderFloat("intensity", &postContext.luminanceExtractionData.intensity, 0.0f, 10.0f);
      ImGui::SliderFloat("deviation", &postContext.gaussianFilterData.deviation, 0.1f, 50.0f);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("ColorGrading"))
    {
      ImGui::SliderFloat("hueShift", &postContext.colorGradingData.hueShift, 0.0f, 360.0f);
      ImGui::SliderFloat("saturation", &postContext.colorGradingData.saturation, 0.0f, 2.0f);
      ImGui::SliderFloat("brightness", &postContext.colorGradingData.brightness, 0.0f, 2.0f);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("DistanceFog"))
    {
      ImGui::ColorEdit3("FogColor", &postContext.distanceFogData.color.x);
      ImGui::DragFloat("ForDistance", &postContext.distanceFogData.color.w, 0.05f, -1, 1);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("DoF"))
    {
      ImGui::DragFloat("Near", &postContext.dofData._near, 0.01f, 0, 1);
      ImGui::DragFloat("Far", &postContext.dofData._far, 0.01f, 0, 1);
      ImGui::TreePop();
    }

    ImGui::TreePop();
  }
  ImGui::End();

  sceneRenderer.RenderImGui();

#endif
}

#ifdef _DEBUG
#include "System/Dialog.h"
#include <filesystem>
#include <../DirectXTex/ScreenGrab11.h>

void Scene::SaveTextureFile()
{
  ID3D11Resource* res = nullptr;
  Graphics& graphics = Graphics::Instance();

  RenderTarget* rtvs[] = {
    SceneRenderer::Instance().GetG_Color(),
    SceneRenderer::Instance().GetG_Normal(),
    SceneRenderer::Instance().GetG_Pos(),
    SceneRenderer::Instance().GetG_MRAO()
  };

  // �����o����ݒ�
  char filename[256] = { 0 };
  DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), nullptr, nullptr, "png", Graphics::Instance().GetHwnd());

  for (int i = 0; i < ARRAYSIZE(rtvs); ++i) {
    // ���\�[�X�擾
    rtvs[i]->GetRenderTargetView()->GetResource(&res);

    if (result == DialogResult::OK)
    {
      namespace fs = std::filesystem;
      std::filesystem::path path = filename;
      path.replace_filename(std::to_wstring(i) + path.filename().wstring());

      // png�摜�̏����o��
      HRESULT h = DirectX::SaveWICTextureToFile(graphics.GetDeviceContext(),
        res, GUID_ContainerFormatDds, path.c_str());
      assert(!FAILED(h));
    }
    res->Release();
  }
}

#endif // _DEBUG