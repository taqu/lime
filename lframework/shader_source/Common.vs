#ifndef INC_COMMON_VS
#define INC_COMMON_VS

#include "Constants.inc"
#include "Utility.inc"

#define MOTIONVELOCITY

float3x4 transformMatrix(in uint2 bone, in float2 weight, in float4 mat[NUM_MATRICES*3])
{
    float3x4 finalMatrix;

    uint index = bone.x * 3;
    float w = weight.x;
    finalMatrix[0] = w * mat[index+0];
    finalMatrix[1] = w * mat[index+1];
    finalMatrix[2] = w * mat[index+2];

    index = bone.y * 3;
    w = weight.y;
    finalMatrix[0] += w * mat[index+0];
    finalMatrix[1] += w * mat[index+1];
    finalMatrix[2] += w * mat[index+2];
    return finalMatrix;
}

float2 calcVelocity(float4 pos0, float4 pos1)
{
    float2 uv0 = pos0.xy/pos0.w;
    float2 uv1 = pos1.xy/pos1.w;
    float2 velocity = uv1-uv0; //velocity in projection
    return velocity;
}

#endif //INC_COMMON_VS
