#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

// �G�t�F�N�g
class Effect
{
public:
  Effect(const char* filename);
  ~Effect() {}

  // �Đ�
  Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);

  // �Đ������擾
  static bool IsPlay(const Effekseer::Handle& handle);

  // ��~
  static void Stop(const Effekseer::Handle& handle);

  // ���W�ݒ�
  static void SetPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& position);

  // �X�P�[���ݒ�
  static void SetScale(const Effekseer::Handle&, const float& scale);

  //��]�ݒ�
  static void SetRotation(const Effekseer::Handle&, const DirectX::XMFLOAT3& angle);

private:
  Effekseer::EffectRef effekseerEffect;

};
