#ifndef INC_CONSTANTS_INC_
#define INC_CONSTANTS_INC_

#define MAX_DIRECTIONAL_LIGHTS (4)
#define MAX_POINT_LIGHTS (128)
#define MAX_SPOT_LIGHTS (8)

#define MAX_CASCADES (4)
#define NUM_CASCADES (2)
#define NUM_MATRICES (255)
#define SHADOWMAP_TYPE_NOFILTER (0)
#define SHADOWMAP_TYPE_PCF (1)
#define SHADOWMAP_TYPE_MSM (2)
#define SHADOWMAP_TYPE_OPTIMIZEDMSM (3)

#define SHADOWMAP_TYPE SHADOWMAP_TYPE_PCF

#define PCF_FILTER_SIZE (5)
#define PCF_FILTER_SAMPLES (PCF_FILTER_SIZE*PCF_FILTER_SIZE)

#define NUMTHREADS (8)
#define EPSILON (1.0e-6)

cbuffer PerFrameConstant : register(b0)
{
    float velocityScale;
    float velocityMaxMagnitude;
    float reserved0;
    float reserved1;
};

cbuffer PerLightConstant : register(b1)
{
    float4 dlDir;
    float4 dlColor;

    float specularMapMipLevels;
    float lightBleedingBias;
    int clusterGridScale;
    float reserved_b1_0;
};

cbuffer PerCameraConstant : register(b2)
{
    float4x4 mview;
    float4x4 minvview;
    float4x4 mproj;
    float4x4 minvproj;
    float4x4 mvp0; //previous vew projection
    float4x4 mvp1; //current view projection
    float4x4 minvvp1;
    float4 cameraPos;
    int2 screenSize; //(screenWidth, screenHeight)
    float2 screenInvSize; //(screenInvWidth, screenInvHeight)
    float4 linearZParam; //znear - zfar
}

cbuffer PerShadowMapConstant : register(b3)
{
    float4x4 mlvp[NUM_CASCADES];
    float4 cascadeScales;
    float4 shadowMapLightDir;

    int shadowMapSize;
    float invShadowMapSize;
    float depthBias;
    float slopeScaledDepthBias;
};

cbuffer PerModelConstant0 : register(b4)
{
    float4x4 mworld0;
    float4x4 mworld1;
}

cbuffer PerModelConstant1 : register(b5)
{
    float4 matrices0[NUM_MATRICES*3]; //previous skinning matrices
    float4 matrices1[NUM_MATRICES*3]; //current skinning matrices
}

cbuffer MaterialConstant : register(b4)
{
    float4 diffuse;
    float4 specular;
    float4 ambient;
    float4 shadow;
}

cbuffer LightClusterConstant : register(b5)
{
    int4 lightClusterSize;
    float4 lightClusterScale;
    float4 lightClusterBias;
}

#endif //INC_CONSTANTS_INC_
