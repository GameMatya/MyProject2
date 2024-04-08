#pragma once

#include <map>
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "Effect.h"

// �G�t�F�N�g�}�l�[�W���[
class EffectManager
{
private:
  EffectManager() {}
  ~EffectManager() {}

public:
  // �B��̃C���X�^���X�擾
  static EffectManager& Instance()
  {
    static EffectManager instance;
    return instance;
  }

  // �G�t�F�N�g���\�[�X�ǂݍ���
  std::shared_ptr<Effect> LoadEffekseer(const char* filename);

  // ������
  void Initialize();

  void Clear() { effectMap.clear(); }

  // �X�V����
  void Update(float elapsedTime);

  // �`�揈��
  void Render();

  // Effekseer�}�l�[�W���[�̎擾
  Effekseer::ManagerRef GetEffekseerManager() { return effekseerManager; }

private:
  Effekseer::ManagerRef effekseerManager;
  EffekseerRenderer::RendererRef effekseerRenderer;

  // ���\�[�X�R���e�i
  using EffectMap = std::map<std::string, std::shared_ptr<Effect>>;
  EffectMap effectMap;

};
