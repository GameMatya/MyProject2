#include "Math/OriginMath.h"
#include <iostream>

DirectX::XMFLOAT2 Mathf::Lerp(const DirectX::XMFLOAT2& start, const DirectX::XMFLOAT2& end, const float& rate)
{
  DirectX::XMFLOAT2 result;
  result.x = std::lerp(start.x, end.x, rate);
  result.y = std::lerp(start.y, end.y, rate);
  return result;
}

DirectX::XMFLOAT3 Mathf::Lerp(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const float& rate)
{
  DirectX::XMFLOAT3 result;
  result.x = std::lerp(start.x, end.x, rate);
  result.y = std::lerp(start.y, end.y, rate);
  result.z = std::lerp(start.z, end.z, rate);
  return result;
}

DirectX::XMFLOAT4 Mathf::Lerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const float& rate)
{
  DirectX::XMFLOAT4 result;
  result.x = std::lerp(start.x, end.x, rate);
  result.y = std::lerp(start.y, end.y, rate);
  result.z = std::lerp(start.z, end.z, rate);
  result.w = std::lerp(start.w, end.w, rate);
  return result;
}

DirectX::XMFLOAT4 Mathf::Slerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const float& rate)
{
  DirectX::XMFLOAT4 result;

  DirectX::XMVECTOR Start = DirectX::XMLoadFloat4(&start);
  DirectX::XMVECTOR End = DirectX::XMLoadFloat4(&end);

  DirectX::XMStoreFloat4(&result, DirectX::XMQuaternionSlerp(Start, End, rate));

  return result;
}

DirectX::XMFLOAT4 Mathf::LookAt(const DirectX::XMFLOAT3& source, const DirectX::XMFLOAT3& target)
{
  // LookPositionに向くための回転軸と回転量を計算
  DirectX::XMVECTOR RotationAxis;
  float rotationAngle;
  {
    // 仮の前方向
    DirectX::XMFLOAT3 sourceForward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
    // 向くべき方向
    DirectX::XMFLOAT3 targetForward = target - source;
    targetForward = Normalize(targetForward);

    // 回転用の任意軸を作成
    RotationAxis = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&sourceForward), DirectX::XMLoadFloat3(&targetForward));
    // 回転量を計算
    float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&sourceForward), DirectX::XMLoadFloat3(&targetForward)));
    rotationAngle = acosf(dotProduct);
  }

  // クォータニオンを作成
  DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionRotationAxis(RotationAxis, rotationAngle);
  DirectX::XMFLOAT4 quaternion;
  DirectX::XMStoreFloat4(&quaternion, Quaternion);

  return quaternion;
}

float Mathf::Dot(const DirectX::XMFLOAT2& first, const DirectX::XMFLOAT2& second)
{
  return first.x * second.x + first.y * second.y;
}

float Mathf::Dot(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second)
{
  return first.x * second.x + first.y * second.y + first.z * second.z;
}

float Mathf::Dot(const DirectX::XMFLOAT4& first, const DirectX::XMFLOAT4& second)
{
  return first.x * second.x + first.y * second.y + first.z * second.z + first.w * second.w;
}

float Mathf::Dot(const DirectX::XMFLOAT3& value)
{
  return value.x * value.x + value.y * value.y + value.z * value.z;
}

DirectX::XMFLOAT3 Mathf::Cross(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second)
{
  return DirectX::XMFLOAT3(
    first.y * second.z - first.z * second.y,
    first.z * second.x - first.x * second.z,
    first.x * second.y - first.y * second.x
  );
}

DirectX::XMFLOAT2 Mathf::Normalize(const DirectX::XMFLOAT2& value)
{
  return value / sqrtf(Dot(value, value));
}

DirectX::XMFLOAT3 Mathf::Normalize(const DirectX::XMFLOAT3& value)
{
  return value / sqrtf(Dot(value, value));
}

DirectX::XMFLOAT4 Mathf::Normalize(const DirectX::XMFLOAT4& value)
{
  return value / sqrtf(Dot(value, value));
}

DirectX::XMFLOAT2 Mathf::Normalize(const DirectX::XMFLOAT2& value, float& length)
{
  length = sqrtf(Dot(value, value));
  return value / length;
}

DirectX::XMFLOAT3 Mathf::Normalize(const DirectX::XMFLOAT3& value, float& length)
{
  length = sqrtf(Dot(value, value));
  return value / length;
}

DirectX::XMFLOAT4 Mathf::Normalize(const DirectX::XMFLOAT4& value, float& length)
{
  length = sqrtf(Dot(value, value));
  return value / length;
}

float Mathf::RandomRange(const float& min, const float& max)
{
  float range = max - min;
  return min + ((float)rand() / (float)RAND_MAX) * range;
}

int Mathf::GetSign(const float& value)
{
  return (value > 0) - (value < 0);
}

int Mathf::GetSign(const int& value)
{
  return (value > 0) - (value < 0);
}
