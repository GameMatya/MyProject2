#include "Effect.h"
#include "Graphics/Graphics.h"
#include "Graphics/EffectManager.h"

// �R���X�g���N�^
Effect::Effect(const char* filename)
{
  std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

  // Effekseer�̃��\�[�X��ǂݍ���
  char16_t utf16FileName[256];
  Effekseer::ConvertUtf8ToUtf16(utf16FileName, 256, filename);

  // Effekseer::Manager���擾
  Effekseer::ManagerRef effectManager = EffectManager::Instance().GetEffekseerManager();

  // Effekseer�G�t�F�N�g��ǂݍ���
  effekseerEffect = Effekseer::Effect::Create(effectManager, (EFK_CHAR*)utf16FileName);
  assert(effekseerEffect != nullptr && u8"Effeksser �ǂݍ��ݎ��s");
}

// �Đ�
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
  if (effekseerEffect == nullptr)return -1;

  Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

  Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);

  effekseerManager->SetScale(handle, scale, scale, scale);

  return handle;
}

bool Effect::IsPlay(const Effekseer::Handle& handle)
{
  Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();
  return effekseerManager->GetShown(handle);
}

// ��~
void Effect::Stop(const Effekseer::Handle& handle)
{
  Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

  effekseerManager->StopEffect(handle);
}

void Effect::SetPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& position)
{
  Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

  effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

void Effect::SetScale(const Effekseer::Handle& handle, const float& scale)
{
  Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

  effekseerManager->SetLocation(handle, scale, scale, scale);
}

//��]�ݒ�
void Effect::SetRotation(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& angle)
{
  Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

  effekseerManager->SetRotation(handle, angle.x, angle.y, angle.z);
}
