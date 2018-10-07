#include "Common.hs"
#include "Volume.inc"

#define INPUT_PATCH_SIZE 1
#define OUTPUT_PATCH_SIZE 4
#define DOMAIN "quad"

struct HSConstantOutput
{
    float edgeFactor[4] : SV_TessFactor;
    float insideFactor[2] : SV_InsideTessFactor;
};

HSConstantOutput calcHSPatchConstants(
    InputPatch<VSInOut, INPUT_PATCH_SIZE> patches,
    uint patchID : SV_PrimitiveID)
{
    float3 position = patches[0].position;

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
VSInOut main(InputPatch<VSInOut, INPUT_PATCH_SIZE> patches, 
    uint id : SV_OutputControlPointID,
    uint patchID : SV_PrimitiveID)
{
    return patches[id];
}
