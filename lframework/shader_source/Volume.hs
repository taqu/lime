#include "Common.hs"
#include "Volume.inc"

#define INPUT_PATCH_SIZE 1
#define OUTPUT_PATCH_SIZE 4
#define DOMAIN "quad"

cbuffer ConstantBuffer0 : register(b1)
{
    float tesselationFactor;
    float minDistance;
    float invRange;
    float tesselationMinFactor;
}

struct HSConstantOutput
{
    float edgeFactor[4] : SV_TessFactor;
    float insideFactor[2] : SV_InsideTessFactor;
};

HSConstantOutput calcHSPatchConstants(
    InputPatch<InOut, INPUT_PATCH_SIZE> patches,
    uint patchID : SV_PrimitiveID)
{
    float3 position = vertexBuffer[patches[0].vertexID].position;

    float scaleFactor = calcDistanceScaleFactorOne(cameraPos.xyz, position, minDistance, invRange);
    float factor = lerp(tesselationFactor, tesselationMinFactor, scaleFactor);
    HSConstantOutput output;
    output.edgeFactor[0] = factor;
    output.edgeFactor[1] = factor;
    output.edgeFactor[2] = factor;
    output.edgeFactor[3] = factor;
    output.insideFactor[0] = factor;
    output.insideFactor[1] = factor;
    return output;
}

[domain(DOMAIN)]
[partitioning("integer")]
//[partitioning("fractional_even")]
//[partitioning("fractional_odd")]
//[partitioning("pow2")]
[outputtopology("triangle_ccw")]
[patchconstantfunc("calcHSPatchConstants")]
[outputcontrolpoints(OUTPUT_PATCH_SIZE)]
InOut2 main(InputPatch<InOut, INPUT_PATCH_SIZE> patches, 
    uint id : SV_OutputControlPointID,
    uint patchID : SV_PrimitiveID)
{
    Vertex v = vertexBuffer[patches[id].vertexID];
    float3 coordu,coordv;
    calcCoordinate(coordu, coordv, v.vector0);
    InOut2 output;
    output.vertexID = patches[id].vertexID;
    output.coordu = coordu;
    output.coordv = coordv;
    return output;
}
