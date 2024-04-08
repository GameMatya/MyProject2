#include "../Constants.hlsli"

// éQçl : https://qiita.com/ScreenPocket/items/6bd9896314dc78f3b170
inline float LinearDepth(float z)
{
    return 1.0 / ((1 - _cameraScope.y / _cameraScope.x) * z + (_cameraScope.y / _cameraScope.x));
}