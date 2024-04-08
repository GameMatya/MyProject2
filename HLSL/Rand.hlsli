/*
 * Xorshift32を用いて32bitの擬似乱数を生成する
 */
uint XOrShift32(uint value)
{
    value = value ^ (value << 13);
    value = value ^ (value >> 17);
    value = value ^ (value << 5);
    return value;
}

/*
 * 整数の値を1未満の小数にマッピングする
 */
float MapToFloat(uint value)
{
    const float precion = 100000000.0;
    return (value % precion) * rcp(precion);
}

// 1次元のランダムな値を算出する
float random(float n)
{
    return frac(sin(n) * 43758.5453123);
}

float random(float2 seed)
{
    uint h = XOrShift32(asuint(seed.x));
    h = XOrShift32(h ^ asuint(seed.y));
    return asfloat((h & 0x007FFFFF) | 0x40000000) - 3.0;
}

// 2次元のランダムな値を算出する
float2 Random2(uint2 src, int seed)
{
    uint2 random;
    random.x = XOrShift32(mad(src.x, src.y, 1));
    random.y = XOrShift32(mad(random.x, 1, src.x) + seed);

    return float2(MapToFloat(random.x), MapToFloat(random.y));
}

// 3次元のランダムな値を算出する
float3 Random3(uint3 src, int seed)
{
    uint3 random;
    random.x = XOrShift32(mad(src.x, src.y, src.z));
    random.y = XOrShift32(mad(random.x, src.z, src.x) + seed);
    random.z = XOrShift32(mad(random.y, src.x, src.y) + seed);
    random.x = XOrShift32(mad(random.z, src.y, src.z) + seed);

    return float3(MapToFloat(random.x), MapToFloat(random.y), MapToFloat(random.z));
}