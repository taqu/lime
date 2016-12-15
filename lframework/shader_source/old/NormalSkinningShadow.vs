#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
    uint2 bone : BLENDINDICES;
    float2 weight : BLENDWEIGHT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
    float3 viewPos : TEXCOORD3;
    float4 texS[NUM_CASCADES] : TEXCOORD4;
};


VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float3x4 finalMatrix = transformMatrix(input.bone, input.weight, matrices1);

    float4 position = float4(input.position, 1);
    position = float4(mul(finalMatrix, position), 1);
    float3 normal = mul(finalMatrix, input.normal);

    float4x4 mwvp = mul(mworld1, mvp1);
    output.position = mul(position, mwvp);
    output.normal = normalize( mul(normal, (float3x3)mworld1) );
    output.uv = input.uv;

    float4 worldPos = mul(position, mworld1);
    output.worldPos.xyz = worldPos.xyz;
    //output.worldPos.w = distance(worldPos.xyz, cameraPos.xyz);
    output.viewPos.xyz = mul(worldPos, mview).xyz;
    calcShadowTexcoord(output.texS, worldPos, mlvp);
    return output;
}
