#include <algorithm>
#include "Camera/Camera.h"
#include "Graphics/Graphics.h"

using namespace DirectX;

Camera::Camera()
{
}

void Camera::createFrustum()
{
  // ビュープロジェクション行列を取得する
  XMMATRIX matrix = {};
  XMMATRIX viewMat = XMLoadFloat4x4(&view);
  XMMATRIX projMat = XMLoadFloat4x4(&projection);
  matrix = viewMat * projMat;

  // ビュープロジェクション行列の逆行列
  XMMATRIX inv_matrix = XMMatrixInverse(nullptr, matrix);

  // ビュープロジェクション内の頂点算出用位置ベクトル
  XMVECTOR verts[8] = {
    // near plane corners
    { -1, -1, 0 },	// [0]:左下
    {  1, -1, 0 },	// [1]:右下
    {  1,  1, 0 },	// [2]:右上
    { -1,  1 ,0 },	// [3]:左上

    // far plane corners.
    { -1, -1, 1 },	// [4]:左下
    {  1, -1, 1 },	// [5]:右下
    {  1,  1, 1 },	// [6]:右上
    { -1,  1, 1 } 	// [7]:左上
  };

  // ビュープロジェクション行列の逆行列を用いて、各頂点を算出する(クリッピング空間→グローバル空間)
#pragma region 
  verts[0] = DirectX::XMVector3TransformCoord(verts[0], inv_matrix);// [0]:左下
  verts[6] = DirectX::XMVector3TransformCoord(verts[6], inv_matrix);// [6]:右上

#ifdef _DEBUG // デバッグライン用
  verts[1] = DirectX::XMVector3TransformCoord(verts[1], inv_matrix);// [1]:右下
  verts[2] = DirectX::XMVector3TransformCoord(verts[2], inv_matrix);// [2]:右上
  verts[3] = DirectX::XMVector3TransformCoord(verts[3], inv_matrix);// [3]:左上

  verts[4] = DirectX::XMVector3TransformCoord(verts[4], inv_matrix);// [4]:左下
  verts[5] = DirectX::XMVector3TransformCoord(verts[5], inv_matrix);// [5]:右下
  verts[7] = DirectX::XMVector3TransformCoord(verts[7], inv_matrix);// [7]:左上
#endif // DEBUG
#pragma endregion

  XMFLOAT4X4 matrix4X4 = {};
  XMStoreFloat4x4(&matrix4X4, matrix);

  // 視錐台（フラスタム）を構成する６平面を算出する
  // 0:左側面, 1:右側面, 2:下側面, 3:上側面, 4:手前側面, 5:奥側面
  // 全ての面の法線は内側を向くように設定
#pragma region ビュープロジェクション行列から法線を取得
  // 左側面
  frustum.plane[0].n = { matrix4X4._14 + matrix4X4._11 ,matrix4X4._24 + matrix4X4._21 ,matrix4X4._34 + matrix4X4._31 };
  // 右側面
  frustum.plane[1].n = { matrix4X4._14 - matrix4X4._11 ,matrix4X4._24 - matrix4X4._21 ,matrix4X4._34 - matrix4X4._31 };
  // 下側面
  frustum.plane[2].n = { matrix4X4._14 + matrix4X4._12,matrix4X4._24 + matrix4X4._22,matrix4X4._34 + matrix4X4._32 };
  // 上側面
  frustum.plane[3].n = { matrix4X4._14 - matrix4X4._12,matrix4X4._24 - matrix4X4._22,matrix4X4._34 - matrix4X4._32 };
  // 手前側面
  frustum.plane[4].n = { -matrix4X4._14 + matrix4X4._13,-matrix4X4._24 + matrix4X4._23 ,-matrix4X4._34 + matrix4X4._33 };
  // 奥側面
  frustum.plane[5].n = { -matrix4X4._14 - matrix4X4._13,-matrix4X4._24 - matrix4X4._23,-matrix4X4._34 - matrix4X4._33 };
#pragma endregion

#pragma region 法線を正規化
  for (int i = 0; i < 6; ++i) {
    XMStoreFloat3(&frustum.plane[i].n,
      XMVector3Normalize(XMLoadFloat3(&frustum.plane[i].n)));
  }
#pragma endregion

  // 各平面に含まれる頂点の位置ベクトルと単位法線の内積を取って、原点からの距離を求める
#pragma region
  // i = 0:左側面, 1 : 右側面, 2 : 下側面, 3 : 上側面, 4 : 手前側面, 5 : 奥側面
  for (int i = 0; i < 6; ++i) {
    XMVECTOR dist = XMVector3Dot(XMLoadFloat3(&frustum.plane[i].n),
      // 0 or 6 : 左下頂点(手前)と右上頂点(奥)
      verts[(i + 1 % 2 == 0) * 6]);
    frustum.plane[i].d = XMVectorGetX(dist);
  }
#pragma endregion

#ifdef _DEBUG
  DirectX::XMVECTOR Fm_Norm[6] = {};

  for (int i = 0; i < 6; ++i) {
    Fm_Norm[i] = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&frustum.plane[i].n));
  }

  // 各境界線outLineNormを面の法線の外積から求めて正規化する
  // 左下境界線	
  XMStoreFloat3(&debugLineNorm[0], XMVector3Normalize(XMVector3Cross(Fm_Norm[0], Fm_Norm[2])));
  // 右下境界線
  XMStoreFloat3(&debugLineNorm[1], XMVector3Normalize(XMVector3Cross(Fm_Norm[2], Fm_Norm[1])));
  // 右上境界線
  XMStoreFloat3(&debugLineNorm[2], XMVector3Normalize(XMVector3Cross(Fm_Norm[1], Fm_Norm[3])));
  // 左上境界線
  XMStoreFloat3(&debugLineNorm[3], XMVector3Normalize(XMVector3Cross(Fm_Norm[3], Fm_Norm[0])));
#endif // DEBUG

}

//bool Camera::hitFrustum(AABB aabb) {
//  // 視錐台の作成
//  createFrustum();
//
//  //④各平面の法線の成分を用いてAABBの８頂点の中から最近点と最遠点を求める
//  for (int i = 0; i < 6; i++)
//  {
//    XMFLOAT3 NegaPos = aabb.position;	// 最近点
//    XMFLOAT3 PosiPos = aabb.position;	// 最遠点
//
//    if (frustum.plane[i].n.x >= 0)
//    {
//      PosiPos.x += aabb.size.x;
//    }
//    else
//    {
//      PosiPos.x -= aabb.size.x;
//    }
//    if (frustum.plane[i].n.y >= 0)
//    {
//      PosiPos.y += aabb.size.y;
//    }
//    else
//    {
//      PosiPos.y -= aabb.size.y;
//    }
//    if (frustum.plane[i].n.z >= 0)
//    {
//      PosiPos.z += aabb.size.z;
//    }
//    else
//    {
//      PosiPos.z -= aabb.size.z;
//    }
//
//    if (frustum.plane[i].n.x >= 0)
//    {
//      //NegaPos.x -= aabb.r.x;
//      NegaPos.x = PosiPos.x - aabb.size.x * 2.0f;
//    }
//    else
//    {
//      //NegaPos.x += aabb.r.x;
//      NegaPos.x = PosiPos.x + aabb.size.x * 2.0f;
//    }
//    if (frustum.plane[i].n.y >= 0)
//    {
//      //NegaPos.y -= aabb.r.y;
//      NegaPos.y = PosiPos.y - aabb.size.y * 2.0f;
//    }
//    else
//    {
//      //NegaPos.y += aabb.r.y;
//      NegaPos.y = PosiPos.y + aabb.size.y * 2.0f;
//    }
//    if (frustum.plane[i].n.z >= 0)
//    {
//      //NegaPos.z -= aabb.r.z;
//      NegaPos.z = PosiPos.z - aabb.size.z * 2.0f;
//    }
//    else
//    {
//      //NegaPos.z += aabb.r.z;
//      NegaPos.z = PosiPos.z + aabb.size.z * 2.0f;
//    }
//
//    //⑤各平面との内積を計算し、交差・内外判定(表裏判定)を行う
//    //  外部と分かれば処理をbreakし確定させる
//    //  交差状態であれば、ステータスを変更してから次の平面とのチェックに続ける
//    //  内部であれば、そのまま次の平面とのチェックに続ける
//    XMVECTOR PosiDot = XMVector3Dot(XMLoadFloat3(&PosiPos), XMVector3Normalize(XMLoadFloat3(&frustum.plane[i].n)));
//    XMVECTOR NegaDot = XMVector3Dot(XMLoadFloat3(&NegaPos), XMVector3Normalize(XMLoadFloat3(&frustum.plane[i].n)));
//
//    float posidot = XMVectorGetX(PosiDot);
//    float negadot = XMVectorGetX(NegaDot);
//
//    if (posidot <= frustum.plane[i].d && negadot <= frustum.plane[i].d)
//    {
//      return false;
//    }
//  }
//
//  return true;
//}

void Camera::debugDraw()
{
  // LineRenderer* line= Graphics::Instance().GetLineRenderer();
  // DebugRenderer* debug = Graphics::Instance().GetDebugRenderer();

  //for (int i = 0; i < 4; ++i) {
  //  // 境界線描画
  //  line->AddVertex(eye, { 1.0f, 1.0f, 1.0f, 1.0f });
  //  line->AddVertex({ eye.x + debugLineNorm[i].x * 40.0f, eye.y + debugLineNorm[i].y * 40.0f,eye.z + debugLineNorm[i].z * 40.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

  //  // Nearの四角形とFarの四角形の描画
  //  if (i != 3)
  //  {
  //    line->AddVertex(debugFar[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugFar[i + 1], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[i + 1], { 1.0f,1.0f,1.0f,1.0f });
  //  }
  //  else
  //  {
  //    line->AddVertex(debugFar[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugFar[0], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[i], { 1.0f,1.0f,1.0f,1.0f });
  //    line->AddVertex(debugNear[0], { 1.0f,1.0f,1.0f,1.0f });
  //  }

  //  // Nearの点とFarの点の描画
  //  debug->DrawSphere(debugNear[i], 0.02f, { 1.0f,1.0f,0.0f,1.0f });
  //  debug->DrawSphere(debugFar[i], 0.1f, { 0.0f,0.0f,1.0f,1.0f });
  //}
}

// 指定方向に向く
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
  // 視点、注視点、上方向からビュー行列を作成
  XMVECTOR Eye = XMLoadFloat3(&eye);
  XMVECTOR Focus = XMLoadFloat3(&focus);
  XMVECTOR Up = XMLoadFloat3(&up);
  XMMATRIX View = XMMatrixLookAtLH(Eye, Focus, Up);
  XMStoreFloat4x4(&view, View);

  // ビューを逆行列化し、ワールド行列に戻す
  XMMATRIX World = XMMatrixInverse(nullptr, View);
  XMFLOAT4X4 world;
  XMStoreFloat4x4(&world, World);

  // カメラの方向を取り出す
  this->right.x = world._11;
  this->right.y = world._12;
  this->right.z = world._13;

  this->up.x = world._21;
  this->up.y = world._22;
  this->up.z = world._23;

  this->forward.x = world._31;
  this->forward.y = world._32;
  this->forward.z = world._33;

  // 視点、注視点を保存
  this->eye = eye;
  this->focus = focus;
}

void Camera::SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& up)
{
  // 視点、注視点、上方向からビュー行列を作成
  XMVECTOR Eye = XMLoadFloat3(&eye);
  XMVECTOR Direction = XMLoadFloat3(&direction);
  XMVECTOR Up = XMLoadFloat3(&up);
  XMMATRIX View = XMMatrixLookToLH(Eye, Direction, Up);
  XMStoreFloat4x4(&view, View);

  // ビューを逆行列化し、ワールド行列に戻す
  XMMATRIX World = XMMatrixInverse(nullptr, View);
  XMFLOAT4X4 world;
  XMStoreFloat4x4(&world, World);

  // カメラの方向を取り出す
  this->right.x = world._11;
  this->right.y = world._12;
  this->right.z = world._13;

  this->up.x = world._21;
  this->up.y = world._22;
  this->up.z = world._23;

  this->forward.x = world._31;
  this->forward.y = world._32;
  this->forward.z = world._33;

  // 視点、注視点を保存
  this->eye = eye;
  this->focus = focus;
}

void Camera::SetLookTo(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT4& direction, const DirectX::XMFLOAT3& up)
{
  // 視点、注視点、上方向からビュー行列を作成
  XMVECTOR Eye = XMLoadFloat3(&eye);
  XMVECTOR Direction = XMLoadFloat4(&direction);
  XMVECTOR Up = XMLoadFloat3(&up);
  XMMATRIX View = XMMatrixLookToLH(Eye, Direction, Up);
  XMStoreFloat4x4(&view, View);

  // ビューを逆行列化し、ワールド行列に戻す
  XMMATRIX World = XMMatrixInverse(nullptr, View);
  XMFLOAT4X4 world;
  XMStoreFloat4x4(&world, World);

  // カメラの方向を取り出す
  this->right.x = world._11;
  this->right.y = world._12;
  this->right.z = world._13;

  this->up.x = world._21;
  this->up.y = world._22;
  this->up.z = world._23;

  this->forward.x = world._31;
  this->forward.y = world._32;
  this->forward.z = world._33;

  // 視点、注視点を保存
  this->eye = eye;
  this->focus = focus;
}

//パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
  // 画角、画面比率、クリップ距離からプロジェクション行列を作成
  XMMATRIX Projection = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
  XMStoreFloat4x4(&projection, Projection);

  //	パラメーターだけ保存
  this->fovY = fovY;
  this->aspect = aspect;
  this->nearZ = nearZ;
  this->farZ = farZ;
}

// カメラ移動
DirectX::XMFLOAT3 Camera::MovieMove(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, float timer)
{
  XMVECTOR Start = XMLoadFloat3(&start);
  XMVECTOR End = XMLoadFloat3(&end);

  XMFLOAT3 Value = {};
  XMStoreFloat3(&Value, XMVectorLerp(Start, End, timer));

  return Value;
}

// 0.001までの誤差は認める比較
inline bool Equal(DirectX::XMFLOAT3 left, DirectX::XMFLOAT3 right)
{
  if (fabs(left.x - right.x) > 0.001 * fmax(1, fmax(fabs(left.x), fabs(right.x)))) return false;
  if (fabs(left.y - right.y) > 0.001 * fmax(1, fmax(fabs(left.y), fabs(right.y)))) return false;
  if (fabs(left.z - right.z) > 0.001 * fmax(1, fmax(fabs(left.z), fabs(right.z)))) return false;

  return true;
}

bool Camera::Movie(
  DirectX::XMFLOAT3 startEye, DirectX::XMFLOAT3 endEye,
  DirectX::XMFLOAT3 startFocus, DirectX::XMFLOAT3 endFocus,
  float timer, DirectX::XMFLOAT3 cameraUp
) {
  startEye = MovieMove(startEye, endEye, min(1.0f, timer));
  startFocus = MovieMove(startFocus, endFocus, min(1.0f, timer));

  SetLookAt(startEye, startFocus, cameraUp);

  return (Equal(startEye, endEye) && Equal(startFocus, endFocus));
}

void Camera::CollideStage(StageManager& manager, DirectX::XMFLOAT3& eye, DirectX::XMFLOAT3& focus)
{
  HitResult result = {};
  if (manager.Collision(focus, eye, &result)) {
    eye = result.position;
  }
}
