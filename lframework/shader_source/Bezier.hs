#include "Common.hs"

#define INPUT_PATCH_SIZE 4
#define OUTPUT_PATCH_SIZE 4
#define DOMAIN "quad"
//#define USE_VIEW_FRUSTUM_CULLING
//#define USE_BACK_FACE_CULLING
//#define USE_SCREEN_SPACE_ADAPTIVE_TESSELLATION
//#define USE_DISTANCE_ADAPTIVE_TESSELLATION
//#define USE_SCREEN_RESOLUTION_ADAPTIVE_TESSELLATION
//#define USE_ORIENTATION_ADAPTIVE_TESSELLATION

cbuffer ConstantBuffer0 : register(b1)
{
    float tesselationFactor;
    float minDistance;
    float invRange;
    float tesselationMinFactor;
}

struct VSOutput
{
    float3 position : POSITION;
    float3 normal : TEXCOORD0;
};

struct HSOutput
{
    float3 position : POSITION;
    float3 normal : TEXCOORD0;
};

struct HSConstantOutput
{
    float edgeFactor[4] : SV_TessFactor;
    float insideFactor[2] : SV_InsideTessFactor;
};

HSConstantOutput bezierConstant(
    InputPatch<VSOutput, INPUT_PATCH_SIZE> patches,
    uint patchID : SV_PrimitiveID)
{
    HSConstantOutput output;

    float factor;
    factor = calcDistanceScaleFactor(cameraPos.xyz, patches[0].position, patches[1].position, minDistance, invRange);
    output.edgeFactor[0] = lerp(tesselationFactor, 1, factor);

    factor = calcDistanceScaleFactor(cameraPos.xyz, patches[1].position, patches[2].position, minDistance, invRange);
    output.edgeFactor[1] = lerp(tesselationFactor, 1, factor);

    factor = calcDistanceScaleFactor(cameraPos.xyz, patches[2].position, patches[3].position, minDistance, invRange);
    output.edgeFactor[2] = lerp(tesselationFactor, 1, factor);

    factor = calcDistanceScaleFactor(cameraPos.xyz, patches[0].position, patches[3].position, minDistance, invRange);
    output.edgeFactor[3] = tesselationFactor;//lerp(tesselationFactor, 1, factor);

    output.insideFactor[0] = (output.edgeFactor[0] + output.edgeFactor[2]) * 0.5;
    output.insideFactor[1] = (output.edgeFactor[1] + output.edgeFactor[3]) * 0.5;

    return output;
}

[domain(DOMAIN)]
[partitioning("integer")]
//[partitioning("fractional_even")]
//[partitioning("fractional_odd")]
//[partitioning("pow2")]
[outputtopology("triangle_cw")]
[patchconstantfunc("bezierConstant")]
[outputcontrolpoints(OUTPUT_PATCH_SIZE)]
HSOutput main(InputPatch<VSOutput, INPUT_PATCH_SIZE> patches, 
    uint id : SV_OutputControlPointID,
    uint patchID : SV_PrimitiveID)
{
    HSOutput output;
    output.position = patches[id].position;
    output.normal = patches[id].normal;
    return output;
}
