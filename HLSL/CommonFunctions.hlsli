#ifndef	__COMMONFUNCTIONS_HLSLI__
#define	__COMMONFUNCTIONS_HLSLI__
#include	"Constants.hlsli"

//--------------------------------------------
//	�����~���}�@�ɑ�����UV���W�v�Z�֐�
//--------------------------------------------
// v		: �x�N�g��
// �Ԃ��l	: UV���W
float2 EquirectangularProjection(float3 v)
{
    float2 texcoord;
    texcoord = float2(1.0f - atan2(v.z, v.x) / 2.0f, -atan2(v.y, length(v.xz)));
    texcoord = texcoord / PI + 0.5f;
    return texcoord;
}

#endif // #endif __COMMONFUNCTIONS_HLSLI__
