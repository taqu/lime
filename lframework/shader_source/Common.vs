#define MAX_CASCADES 4
#define NUM_CASCADES 2

#define NumMatrices (240)

#define MOTIONVELOCITY

cbuffer PerFrameConstant : register(b0)
{
    float velocityScale;
    float velocityMaxMagnitude;
    float reserved0;
    float reserved1;
};

cbuffer PerCameraConstant : register(b1)
{
    float4x4 mview;
    float4x4 minvview;
    float4x4 mproj;
    float4x4 minvproj;
    float4x4 mvp0; //previous vew projection
    float4x4 mvp1; //current view projection
    float4 cameraPos;
    int2 screenSize; //(screenWidth, screenHeight)
    float2 screenInvSize; //(screenInvWidth, screenInvHeight)
}

#ifndef MOTIONVELOCITY
cbuffer PerModelConstant0 : register(b2)
{
    float4x4 mworld1;
}

cbuffer PerModelConstant1 : register(b3)
{
    float4 matrices1[NumMatrices*3];
}

#else
cbuffer PerModelConstant0 : register(b2)
{
    float4x4 mworld0;
    float4x4 mworld1;
}

cbuffer PerModelConstant1 : register(b3)
{
    float4 matrices0[NumMatrices*3]; //previous skinning matrices
    float4 matrices1[NumMatrices*3]; //current skinning matrices
}
#endif

cbuffer PerShadowMapConstant : register(b4)
{
    float4x4 mlvp[MAX_CASCADES];
};

void calcShadowTexcoord(out float4 texS[NUM_CASCADES], in float4 worldPos, in float4x4 lvp[MAX_CASCADES])
{
    [unroll(MAX_CASCADES)]
    for(int i=0; i<NUM_CASCADES; ++i){
        texS[i] = mul(worldPos, lvp[i]);
    }
}

float3x4 transformMatrix(in uint2 bone, in float2 weight, in float4 mat[NumMatrices*3])
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


//float4 calcCameraSpaceNormalLinearZ(in float3 worldPos, in float3 worldNormal)
//{
//    float3 normal = mul(worldNormal, (float3x3)mv);
//    float linearZ = 2.0 * saturate((distance(worldPos, cameraPos.xyz) - znear) * invzrange) - 1.0;
//    return float4(linearZ, normal.x, normal.y, normal.z);
//}

float2 calcVelocity(float4 pos0, float4 pos1)
{
    float2 velocity = ((pos1.xy/pos1.w) - (pos0.xy/pos0.w)) * velocityScale; //velocity in screen space
    float mag = length(velocity);
    mag = (mag<0.001)? 0.0 : min(mag, velocityMaxMagnitude);
    return velocity * mag;
}
