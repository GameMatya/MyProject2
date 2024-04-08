#pragma once

#include "Math/OriginMath.h"
#include "GameObject/StageManager.h"

//カメラ
class Camera
{
private:
  Camera();
  ~Camera() {}

  // 視錐台の作成
  void createFrustum();

public:
  // 唯一のインスタンス取得
  static Camera& Instance()
  {
    static Camera camera;
    return camera;
  }

  // 指定位置に向く
  void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus,
    const DirectX::XMFLOAT3& up);

  // 指定方向に向く
  void SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& up);
  void SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT4& direction,
    const DirectX::XMFLOAT3& up);

  // パースペクティブ設定
  void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

  // カメラの移動 ( ただのLerp関数 )
  DirectX::XMFLOAT3 MovieMove(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, float timer);

  // カメラの移動を使ったムービー処理
  bool Movie(DirectX::XMFLOAT3 startEye, DirectX::XMFLOAT3 endEye,
    DirectX::XMFLOAT3 startFocus, DirectX::XMFLOAT3 endFocus,
    float timer, DirectX::XMFLOAT3 cameraAttitude = { 0,1,0 });

  // ステージとの当たり判定
  void CollideStage(StageManager& manager, DirectX::XMFLOAT3& eye, DirectX::XMFLOAT3& focus);

  //// 視錐台との当たり判定
  //bool hitFrustum(AABB aabb);
  //// 視錐台との当たり判定( AABBを使わない版 )
  //bool hitFrustum(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 boxSize) { return hitFrustum({ position ,boxSize }); }

  void debugDraw();

  // ビュー行列取得
  const DirectX::XMFLOAT4X4& GetView()const { return view; }
  // プロジェクション行列取得
  const DirectX::XMFLOAT4X4& GetProjection()const { return projection; }

  // 視点取得
  const DirectX::XMFLOAT3& GetEye()const { return eye; }
  // 注視点取得
  const DirectX::XMFLOAT3& GetFocus()const { return focus; }
  // 上方向取得
  const DirectX::XMFLOAT3& GetUp()const { return up; }
  // 前方向取得
  const DirectX::XMFLOAT3& GetForward()const { return forward; }
  // 右方向取得
  const DirectX::XMFLOAT3& GetRight()const { return right; }

  // 視野角取得
  const	float& GetFovY() { return	fovY; }
  // アスペクト比取得
  const	float& GetAspect() { return	aspect; }
  // 近クリップ面までの距離を取得
  const	float& GetNearZ() { return	nearZ; }
  // 遠クリップ面までの距離をっ取得
  const	float& GetFarZ() { return	farZ; }

private:
  struct Plane
  {
    DirectX::XMFLOAT3	n;	// 法線
    float				d;	// 原点からの最短距離
  };
  struct Frustum
  {
    Plane plane[6] = {};
  };
  Frustum frustum;

  DirectX::XMFLOAT4X4  view = {};
  DirectX::XMFLOAT4X4  projection = {};

  DirectX::XMFLOAT3    eye = {};
  DirectX::XMFLOAT3    focus = {};

  DirectX::XMFLOAT3    up = {};
  DirectX::XMFLOAT3    forward = {};
  DirectX::XMFLOAT3    right = {};

  float					fovY;
  float					aspect;
  float					nearZ;
  float					farZ;

  DirectX::XMFLOAT3	debugLineNorm[4] = {};

};
