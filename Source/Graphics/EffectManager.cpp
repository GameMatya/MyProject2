#include "Graphics/Graphics.h"
#include "EffectManager.h"
#include "Camera/Camera.h"

std::shared_ptr<Effect> EffectManager::LoadEffekseer(const char* filename) 
{
  std::string name = filename;
  EffectMap::const_iterator it = effectMap.find(name);
  // ���f������
  if (effectMap.find(name) != effectMap.end()) {
    return (*effectMap.find(name)).second;
  }

  // �V�K���f�����\�[�X�̍쐬�E�ǂݍ���
  std::shared_ptr<Effect> effect = std::make_shared<Effect>(filename);

  // �}�b�v�ɓo�^
  effectMap.emplace(filename, effect);

  return effect;
}

// ������
void EffectManager::Initialize()
{
  Graphics& graphics = Graphics::Instance();

  // Effekseer�����_������
  effekseerRenderer = EffekseerRendererDX11::
    Renderer::Create(graphics.GetDevice(), graphics.GetDeviceContext(), 2048);

  // Effekseer�}�l�[�W���[����
  effekseerManager = Effekseer::Manager::Create(2048);

  // Effekseer�����_���̊e��ݒ�
  effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
  effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
  effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
  effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
  effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
  effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
  effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
  effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

  // Effekseer��������W�n�Ōv�Z����
  effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// �X�V����
void EffectManager::Update(float elapsedTime)
{
  // �G�t�F�N�g�X�V����( �����Ɍo�ߎ��Ԃ�n��)
  effekseerManager->Update(elapsedTime * 60.0f);
}

// �`�揈��
void EffectManager::Render()
{
  Camera& camera = Camera::Instance();

  // �r���[���v���W�F�N�V�����s���Effekseer�����_���ɐݒ�
  effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&camera.GetView()));
  effekseerRenderer->SetProjectionMatrix
  (*reinterpret_cast<const Effekseer::Matrix44*>(&camera.GetProjection()));

  // Effekseer�`��J�n
  effekseerRenderer->BeginRendering();

  // Effeseer�`����s
  effekseerManager->Draw();

  // Effekseer�`��I��
  effekseerRenderer->EndRendering();

}