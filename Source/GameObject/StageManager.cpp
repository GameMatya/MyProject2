#include "StageManager.h"

bool StageManager::Collision(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult* hitResult)
{
  bool result = false;

  // スタートと交点の距離を保存する
  float length = FLT_MAX;

  // hit の複製
  HitResult hit2 = {};

  for (int i = 0; i < stages.size(); ++i)
  {
    // レイキャスト
    if (stages.at(i)->RayCast(start, end, &hit2)) {
      if (hitResult == nullptr)return true;

      result = true;

      //  二点間距離を求める
      DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
      DirectX::XMVECTOR Point = DirectX::XMLoadFloat3(&hit2.position);
      DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Point, Start);

      // 処理を軽くするためにLengthSqを使う ( 正確な距離は要らない為 ) 
      float vecLength = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));

      // 今までの距離より短ければ代入
      if (vecLength < length)
      {
        // 最短距離の更新
        length = vecLength;

        // 最短距離のヒット情報を代入
        *hitResult = hit2;
      }
    }
  }

  return result;
}
