
float calcEdgeDot(float3 edgeNormal0, float3 edgeNormal1, float3 viewVector)
{
    float3 edgeNormal = normalize(edgeNormal0 + edgeNormal1);
    return dot(edgeNormal, viewVector);
}

float2 screenSpacePosition(float3 position, float4x4 viewProj, float2 screenSize)
{
    float4 p = mul(float4(position, 1), viewProj);
    float2 screenSpace = p.xy/p.w;
    return (screenSpace * 0.5 + float2(0.5,0.5))*screenSize;
}


float distanceFromPlane(float3 pos, float4 plane)
{
    return dot(float4(pos,1), plane);
}

float testEdgePlane(float3 edgePos0, float3 edgePos1, float3 edgePos2, float4 plane, float epsilon)
{
    return (distanceFromPlane(edgePos0, plane)> -epsilon)
        + (distanceFromPlane(edgePos1, plane)> -epsilon)
        + (distanceFromPlane(edgePos2, plane)> -epsilon);
}

bool backFaceCull(float edgeDot0, float edgeDot1, float edgeDot2, float epsilon)
{
    float3 eq;
    eq.x = (edgeDot0<=-epsilon);
    eq.y = (edgeDot1<=-epsilon);
    eq.z = (edgeDot2<=-epsilon);
    return all(eq);
}

bool viewFrustumCull(float3 edgePos0, float3 edgePos1, float3 edgePos2, float4 viewFrustumPlanes[4], float epsilon)
{
    float4 test;
    test.x = testEdgePlane(edgePos0, edgePos1, edgePos2, viewFrustumPlanes[0], epsilon);
    test.y = testEdgePlane(edgePos0, edgePos1, edgePos2, viewFrustumPlanes[1], epsilon);
    test.z = testEdgePlane(edgePos0, edgePos1, edgePos2, viewFrustumPlanes[2], epsilon);
    test.w = testEdgePlane(edgePos0, edgePos1, edgePos2, viewFrustumPlanes[3], epsilon);
    return !all(test);
}


/*!
@param eye ... position of camera
@param edgePos0 ... position of the first
@param edgePos1 ... position of the second
@param minDistance ... minimum distance that maximum tesselation factors be applied at
@param range ... range beyond the minimum distance where tessellation will scale down to the minimum scaling factor
*/
float calcDistanceScaleFactor(float3 eye, float3 edgePos0, float3 edgePos1, float minDistance, float invRange)
{
    float3 midPos = (edgePos0 + edgePos1) * 0.5;
    float d = distance(midPos, eye) - minDistance;
    return saturate(d*invRange);
}

float calcDistanceScaleFactorOne(float3 eye, float3 position, float minDistance, float invRange)
{
    float d = distance(position, eye) - minDistance;
    return saturate(d*invRange);
}

float calcOrientationScaleFactor(float edgeDot, float epsilon)
{
    float scale = 1 - abs(edgeDot);
    return saturate((scale-epsilon)/(1-epsilon));
}

float calcScreenResolutionScaleFactor(float width, float height, float maxWidth, float maxHeight)
{
    float maxArea = maxWidth * maxHeight;
    float area = width * height;
    return saturate(area/maxArea);
}

/*!
@param edgeScreenPos0 ... screen space position of the first
@param edgeScreenPos1 ... screen space position of the second
@param maxEdgeFactor ... maximum edge tessellation factor
@param edgePrimitiveSize ...desired primitive edge size in pixels
*/
float calcScreenSpaceScaleFactor(float2 edgeScreenPos0, float2 edgeScreenPos1, float maxEdgeFactor, float edgePrimitiveSize)
{
    float edgeLength = distance(edgeScreenPos0, edgeScreenPos1);
    float tessellationFactor = edgeLength/edgePrimitiveSize;
    return saturate(tessellationFactor/maxEdgeFactor);
}
