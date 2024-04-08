#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

// エフェクト
class Effect
{
public:
  Effect(const char* filename);
  ~Effect() {}

  // 再生
  Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);

  // 再生中か取得
  static bool IsPlay(const Effekseer::Handle& handle);

  // 停止
  static void Stop(const Effekseer::Handle& handle);

  // 座標設定
  static void SetPosition(const Effekseer::Handle& handle, const DirectX::XMFLOAT3& position);

  // スケール設定
  static void SetScale(const Effekseer::Handle&, const float& scale);

  //回転設定
  static void SetRotation(const Effekseer::Handle&, const DirectX::XMFLOAT3& angle);

private:
  Effekseer::EffectRef effekseerEffect;

};
