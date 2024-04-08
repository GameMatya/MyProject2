// カプコンのGameCreatorsConference 2018の資料を参考 ( 96ページ以降 )
// https://www.docswell.com/s/CAPCOM_RandD/Z7DPJK-2022-07-15-133910#p96

// if文はコンパイル時に取り除かれる
static const uint READ = 1; // UAVから読み込み
static const uint WRITE = 2; // UAVに書き込み

// 読み書き可能なストリームを定義
// 読み取り専用はSRVを使用
#define STREAM_R_RW(STREAM_NAME,uav)\
void STREAM_NAME(uint rw,inout uint offset,inout float  value){ if(rw == READ){StreamRead1(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout float2 value){ if(rw == READ){StreamRead2(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout float3 value){ if(rw == READ){StreamRead3(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout float4 value){ if(rw == READ){StreamRead4(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout uint   value){ if(rw == READ){StreamRead1(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout uint2  value){ if(rw == READ){StreamRead2(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout uint3  value){ if(rw == READ){StreamRead3(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout uint4  value){ if(rw == READ){StreamRead4(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout int    value){ if(rw == READ){StreamRead1(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout int2   value){ if(rw == READ){StreamRead2(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout int3   value){ if(rw == READ){StreamRead3(uav,offset,value); }else StreamWrite(uav,offset,value); }\
void STREAM_NAME(uint rw,inout uint offset,inout int4   value){ if(rw == READ){StreamRead4(uav,offset,value); }else StreamWrite(uav,offset,value); }

// StreamRead
#if(1)
// float
void StreamRead1(RWByteAddressBuffer buffer, inout uint offset, out float value)
{
    value = asfloat(buffer.Load(offset));
    offset += 4 * 1;
}
void StreamRead2(RWByteAddressBuffer buffer, inout uint offset, out float2 value)
{
    value = asfloat(buffer.Load2(offset));
    offset += 4 * 2;
}
void StreamRead3(RWByteAddressBuffer buffer, inout uint offset, out float3 value)
{
    value = asfloat(buffer.Load3(offset));
    offset += 4 * 3;
}
void StreamRead4(RWByteAddressBuffer buffer, inout uint offset, out float4 value)
{
    value = asfloat(buffer.Load4(offset));
    offset += 4 * 4;
}

// uint
void StreamRead1(RWByteAddressBuffer buffer, inout uint offset, out uint value)
{
    value = asuint(buffer.Load(offset));
    offset += 4 * 1;
}
void StreamRead2(RWByteAddressBuffer buffer, inout uint offset, out uint2 value)
{
    value = asuint(buffer.Load2(offset));
    offset += 4 * 2;
}
void StreamRead3(RWByteAddressBuffer buffer, inout uint offset, out uint3 value)
{
    value = asuint(buffer.Load3(offset));
    offset += 4 * 3;
}
void StreamRead4(RWByteAddressBuffer buffer, inout uint offset, out uint4 value)
{
    value = asuint(buffer.Load4(offset));
    offset += 4 * 4;
}

// int
void StreamRead1(RWByteAddressBuffer buffer, inout uint offset, out int value)
{
    value = asint(buffer.Load(offset));
    offset += 4 * 1;
}
void StreamRead2(RWByteAddressBuffer buffer, inout uint offset, out int2 value)
{
    value = asint(buffer.Load2(offset));
    offset += 4 * 2;
}
void StreamRead3(RWByteAddressBuffer buffer, inout uint offset, out int3 value)
{
    value = asint(buffer.Load3(offset));
    offset += 4 * 3;
}
void StreamRead4(RWByteAddressBuffer buffer, inout uint offset, out int4 value)
{
    value = asint(buffer.Load4(offset));
    offset += 4 * 4;
}

#endif

// StreamWrite
#if(1)
// float
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in float value)
{
    buffer.Store(offset, asuint(value));
    offset += 4 * 1;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in float2 value)
{
    buffer.Store2(offset, asuint(value));
    offset += 4 * 2;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in float3 value)
{
    buffer.Store3(offset, asuint(value));
    offset += 4 * 3;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in float4 value)
{
    buffer.Store4(offset, asuint(value));
    offset += 4 * 4;
}

// uint
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in uint value)
{
    buffer.Store(offset, value);
    offset += 4 * 1;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in uint2 value)
{
    buffer.Store2(offset, value);
    offset += 4 * 2;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in uint3 value)
{
    buffer.Store3(offset, value);
    offset += 4 * 3;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in uint4 value)
{
    buffer.Store4(offset, value);
    offset += 4 * 4;
}

// int
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in int value)
{
    buffer.Store(offset, asuint(value));
    offset += 4 * 1;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in int2 value)
{
    buffer.Store2(offset, asuint(value));
    offset += 4 * 2;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in int3 value)
{
    buffer.Store3(offset, asuint(value));
    offset += 4 * 3;
}
void StreamWrite(RWByteAddressBuffer buffer, inout uint offset, in int4 value)
{
    buffer.Store4(offset, asuint(value));
    offset += 4 * 4;
}

#endif
