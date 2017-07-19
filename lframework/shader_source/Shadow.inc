#ifndef INC_SHADOW_INC_
#define INC_SHADOW_INC_
#include "Utility.inc"

static const float4 PCFSamples[64] =
{
float4(0.195146, -0.107968, 0.389143, 0.223022),
float4(-0.804854, 0.892032, 0.193845, 1.201462),
float4(0.695146, 0.392032, 0.290138, 0.798071),
float4(-0.304854, -0.607968, 0.316567, 0.680119),
float4(0.445146, 0.642032, 0.294017, 0.781255),
float4(-0.554854, -0.357968, 0.320799, 0.660306),
float4(0.945146, -0.857968, 0.176640, 1.276483),
float4(-0.054854, 0.142032, 0.394345, 0.152257),
float4(0.070146, 0.017032, 0.397904, 0.072184),
float4(-0.929854, -0.982968, 0.159715, 1.353091),
float4(0.570146, -0.482968, 0.301767, 0.747211),
float4(-0.429854, 0.517032, 0.318228, 0.672381),
float4(0.320146, -0.732968, 0.289730, 0.799835),
float4(-0.679854, 0.267032, 0.305535, 0.730416),
float4(0.820146, 0.767032, 0.212370, 1.122932),
float4(-0.179854, -0.232968, 0.382033, 0.294316),
float4(0.132646, 0.954532, 0.250749, 0.963704),
float4(-0.867354, -0.045468, 0.273590, 0.868545),
float4(0.632646, -0.545468, 0.281443, 0.835330),
float4(-0.367354, 0.454532, 0.336313, 0.584421),
float4(0.382646, -0.295468, 0.354943, 0.483445),
float4(-0.617354, 0.704532, 0.257256, 0.936745),
float4(0.882646, 0.204532, 0.264639, 0.906034),
float4(-0.117354, -0.795468, 0.288746, 0.804078),
float4(0.007646, -0.920468, 0.261166, 0.920500),
float4(-0.992354, 0.079532, 0.243051, 0.995536),
float4(0.507646, 0.579532, 0.296497, 0.770429),
float4(-0.492354, -0.420468, 0.323505, 0.647461),
float4(0.257646, 0.329532, 0.365523, 0.418297),
float4(-0.742354, -0.670468, 0.241896, 1.000309),
float4(0.757646, -0.170468, 0.295088, 0.776586),
float4(-0.242354, 0.829532, 0.274620, 0.864210),
float4(0.226396, 0.485782, 0.345571, 0.535947),
float4(-0.773604, -0.514218, 0.259142, 0.928915),
float4(0.726396, -0.014218, 0.306400, 0.726535),
float4(-0.273604, 0.985782, 0.236395, 1.023047),
float4(0.476396, -0.764218, 0.265955, 0.900546),
float4(-0.523604, 0.235782, 0.338302, 0.574242),
float4(0.976396, 0.735782, 0.188945, 1.222589),
float4(-0.023604, -0.264218, 0.385150, 0.265270),
float4(0.101396, -0.389218, 0.367944, 0.402209),
float4(-0.898604, 0.610782, 0.221084, 1.086528),
float4(0.601396, 0.110782, 0.330909, 0.611514),
float4(-0.398604, -0.889218, 0.248147, 0.974471),
float4(0.351396, 0.860782, 0.258942, 0.929744),
float4(-0.648604, -0.139218, 0.320148, 0.663377),
float4(0.851396, -0.639218, 0.226349, 1.064648),
float4(-0.148604, 0.360782, 0.369701, 0.390188),
float4(0.163896, -0.576718, 0.333314, 0.599554),
float4(-0.836104, 0.423282, 0.257160, 0.937143),
float4(0.663896, 0.923282, 0.208975, 1.137193),
float4(-0.336104, -0.076718, 0.375925, 0.344749),
float4(0.413896, 0.173282, 0.360737, 0.448705),
float4(-0.586104, -0.826718, 0.238728, 1.013401),
float4(0.913896, -0.326718, 0.249097, 0.970541),
float4(-0.086104, 0.673282, 0.316859, 0.678765),
float4(0.038896, 0.548282, 0.343008, 0.549660),
float4(-0.961104, -0.451718, 0.226996, 1.061965),
float4(0.538896, -0.951718, 0.219363, 1.093698),
float4(-0.461104, 0.048282, 0.358290, 0.463625),
float4(0.288896, -0.201718, 0.374931, 0.352351),
float4(-0.711104, 0.798282, 0.225283, 1.069076),
float4(0.788896, 0.298282, 0.279542, 0.843403),
float4(-0.211104, -0.701718, 0.305006, 0.732785),
};

float3 getSlopeScaledOffset(in float NdotL, in float3 N)
{
    return slopeScaledDepthBias * (1.0 - NdotL) * N;
}

float sampleShadowMap(Texture2DArray texShadow, SamplerComparisonState samShadow, float2 baseUV, float du, float dv, float depth, int cascade)
{
    float2 uv = baseUV + float2(du,dv) * invShadowMapSize;
    return texShadow.SampleCmpLevelZero(samShadow, float3(uv, cascade), depth).r;
}

float sampleShadowMapOne(Texture2DArray texShadow, SamplerComparisonState samShadow, float4 tex, int cascade)
{
    return texShadow.SampleCmpLevelZero(samShadow, float3(tex.xy, cascade), tex.z).r;
}

float sampleShadowMapOne(Texture2DArray texShadow, SamplerComparisonState samShadow, float2 uv, float depth, int cascade)
{
    return texShadow.SampleCmpLevelZero(samShadow, float3(uv, cascade), depth).r;
}

float sampleShadowPCF(Texture2DArray texShadow, SamplerComparisonState samShadow, float4 tex, int cascade)
{
    float2 pos = tex.xy*shadowMapSize;
    float2 baseUV = floor(pos+float2(0.5,0.5));
    float2 dpos = pos + float2(0.5,0.5) - baseUV;
    baseUV = (baseUV - float2(0.5,0.5))*invShadowMapSize;

#if PCF_FILTER_SIZE == 3
    float2 dpos2 = dpos*2;
    float2 weight0 = float2(3,3)-dpos2;
    float2 weight1 = float2(1,1)+dpos2;
    float2 duv0 = (float2(2,2)-dpos)/weight0 - float2(1,1);
    float2 duv1 = dpos/weight1 + 1;

    float sum = 0;
    sum += weight0.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv0.y, tex.z, cascade);
    sum += weight1.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv0.y, tex.z, cascade);
    sum += weight0.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv1.y, tex.z, cascade);
    sum += weight1.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv1.y, tex.z, cascade);
    return sum * (1.0/16);

#elif PCF_FILTER_SIZE == 5
    float2 weight0 = float2(4,4) - 3*dpos;
    float2 weight1 = 7;
    float2 weight2 = float2(1,1) + 3*dpos;

    float2 duv0 = (float2(3,3) - 2*dpos)/weight0 - float2(2,2);
    float2 duv1 = (float2(3,3) + dpos)/weight1;
    float2 duv2 = dpos/weight2 + float2(2,2);

    float sum = 0;
    sum += weight0.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv0.y, tex.z, cascade);
    sum += weight1.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv0.y, tex.z, cascade);
    sum += weight2.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv2.x, duv0.y, tex.z, cascade);

    sum += weight0.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv1.y, tex.z, cascade);
    sum += weight1.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv1.y, tex.z, cascade);
    sum += weight2.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv2.x, duv1.y, tex.z, cascade);

    sum += weight0.x * weight2.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv2.y, tex.z, cascade);
    sum += weight1.x * weight2.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv2.y, tex.z, cascade);
    sum += weight2.x * weight2.y * sampleShadowMap(texShadow, samShadow, baseUV, duv2.x, duv2.y, tex.z, cascade);
    return sum * (1.0/144);

 #else
    float2 weight0 = 5*dpos - float2(6,6);
    float2 weight1 = 11*dpos - float2(28,28);
    float2 weight2 = -11*dpos - float2(17,17);
    float2 weight3 = -5*dpos - float2(1,1);

    float2 duv0 = (4*dpos-float2(5,5))/weight0 - float2(3,3);
    float2 duv1 = (4*dpos-float2(16,16))/weight1 - float2(1,1);
    float2 duv2 = (-7*dpos-float2(5,5))/weight2 + float2(1,1);
    float2 duv3 = -dpos/weight3 + float2(3,3);

    float sum = 0;
    sum += weight0.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv0.y, tex.z, cascade);
    sum += weight1.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv0.y, tex.z, cascade);
    sum += weight2.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv2.x, duv0.y, tex.z, cascade);
    sum += weight3.x * weight0.y * sampleShadowMap(texShadow, samShadow, baseUV, duv3.x, duv0.y, tex.z, cascade);

    sum += weight0.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv1.y, tex.z, cascade);
    sum += weight1.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv1.y, tex.z, cascade);
    sum += weight2.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv2.x, duv1.y, tex.z, cascade);
    sum += weight3.x * weight1.y * sampleShadowMap(texShadow, samShadow, baseUV, duv3.x, duv1.y, tex.z, cascade);

    sum += weight0.x * weight2.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv2.y, tex.z, cascade);
    sum += weight1.x * weight2.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv2.y, tex.z, cascade);
    sum += weight2.x * weight2.y * sampleShadowMap(texShadow, samShadow, baseUV, duv2.x, duv2.y, tex.z, cascade);
    sum += weight3.x * weight2.y * sampleShadowMap(texShadow, samShadow, baseUV, duv3.x, duv2.y, tex.z, cascade);

    sum += weight0.x * weight3.y * sampleShadowMap(texShadow, samShadow, baseUV, duv0.x, duv3.y, tex.z, cascade);
    sum += weight1.x * weight3.y * sampleShadowMap(texShadow, samShadow, baseUV, duv1.x, duv3.y, tex.z, cascade);
    sum += weight2.x * weight3.y * sampleShadowMap(texShadow, samShadow, baseUV, duv2.x, duv3.y, tex.z, cascade);
    sum += weight3.x * weight3.y * sampleShadowMap(texShadow, samShadow, baseUV, duv3.x, duv3.y, tex.z, cascade);

    return sum * (1.0/2704);
 #endif
}

float sampleShadowRandomPCF(Texture2DArray texShadow, SamplerComparisonState samShadow, float4 tex, int cascade)
{
    float2 pos = tex.xy*shadowMapSize;
    float2 baseUV = floor(pos+float2(0.5,0.5));
    float2 dpos = pos + float2(0.5,0.5) - baseUV;
    float halfPixel = (0.5 * PCF_FILTER_SIZE) * invShadowMapSize;
    baseUV = baseUV * invShadowMapSize - halfPixel;
    uint index = 0;//hashUInt(tex.xy);

    float sum = 0;
    [loop]
    for(uint i=0; i<PCF_FILTER_SAMPLES; ++i){
        uint n = (i+index)&63;
        float2 uv = baseUV + halfPixel * PCFSamples[n].xy;
        sum += PCFSamples[n].z*sampleShadowMapOne(texShadow, samShadow, uv, tex.z, cascade);
    }
    return sum;
}

float linstep(float low, float high, float value)
{
    return saturate((value-low)/(high-low));
}

float reduceLightBleeding(float maxVal, float amount)
{
    return linstep(amount, 1.0, maxVal);
}

#if 0
float4 decodeOptimizedMoments(in float4 optimizedMoments)
{
    optimizedMoments[0] -= 0.035955884801f;
    return mul(optimizedMoments, float4x4(0.2227744146f, 0.1549679261f, 0.1451988946f, 0.163127443f,
                                          0.0771972861f, 0.1394629426f, 0.2120202157f, 0.2591432266f,
                                          0.7926986636f, 0.7963415838f, 0.7258694464f, 0.6539092497f,
                                          0.0319417555f,-0.1722823173f,-0.2758014811f,-0.3376131734f));
}

float computeMSMHausdorff(float4 moments, float depth, float momentBias, float depthBias)
{
    // Bias input data to avoid artifacts
    float4 b = lerp(moments, float4(0.5,0.5,0.5,0.5), momentBias);
    float3 z;
    z[0] = depth-depthBias;

    // Compute a Cholesky factorization of the Hankel matrix B storing only non trivial entries or related products
	float L32D22=mad(-b[0],b[1],b[2]);
	float D22=mad(-b[0],b[0], b[1]);
	float squaredDepthVariance=mad(-b[1],b[1], b[3]);
	float D33D22=dot(float2(squaredDepthVariance,-L32D22),float2(D22,L32D22));
	float InvD22=1.0f/D22;
	float L32=L32D22*InvD22;

	// Obtain a scaled inverse image of bz=(1,z[0],z[0]*z[0])^T
	float3 c=float3(1.0f,z[0],z[0]*z[0]);
	// Forward substitution to solve L*c1=bz
	c[1]-=b.x;
	c[2]-=b.y+L32*c[1];
	// Scaling to solve D*c2=c1
	c[1]*=InvD22;
	c[2]*=D22/D33D22;
	// Backward substitution to solve L^T*c3=c2
	c[1]-=L32*c[2];
	c[0]-=dot(c.yz,b.xy);
	// Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions z[1] 
	// and z[2]
	float p=c[1]/c[2];
	float q=c[0]/c[2];
	float D=(p*p*0.25)-q;
	float r=sqrt(D);
	z[1]=-p*0.5-r;
	z[2]=-p*0.5+r;

    float shadowIntensity;

	// Use a solution made of four deltas if the solution with three deltas is invalid
	if(z[1]<0.0f || z[2]>1.0f){
		float zFree=((b[2]-b[1])*z[0]+b[2]-b[3])/((b[1]-b[0])*z[0]+b[1]-b[2]);
		float w1Factor=(z[0]>zFree)? 1 : 0;
		shadowIntensity=(b[1]-b[0]+(b[2]-b[0]-(zFree+1.0f)*(b[1]-b[0]))*(zFree-w1Factor-z[0])
		                                        /(z[0]*(z[0]-zFree)))/(zFree-w1Factor)+1.0f-b[0];
	}
	// Use the solution with three deltas
	else{
		float4 switchVal = (z[2]<z[0])?float4(z[1],z[0],1.0,1.0):
            ((z[1]<z[0])?float4(z[0],z[1],0.0,1.0):
			float4(0,0,0,0));
		float quotient=(switchVal[0]*z[2]-b[0]*(switchVal[0]+z[2])+b[1])/((z[2]-switchVal[1])*(z[0]-z[1]));
		shadowIntensity=switchVal[2]+switchVal[3]*quotient;
	}

	return 1 - saturate(shadowIntensity);
}

float computeMSMHamburger(float4 moments, float depth, float momentBias, float depthBias)
{
    // Bias input data to avoid artifacts
    float4 b = lerp(moments, float4(0.5,0.5,0.5,0.5), momentBias);
    float3 z;
    z[0] = depth-depthBias;

    // Compute a Cholesky factorization of the Hankel matrix B storing only non trivial entries or related products
    float L32D22 = mad(-b[0], b[1], b[2]);
    float D22 = mad(-b[0], b[0], b[1]);
    float squaredDepthVariance = mad(-b[1], b[1], b[3]);
    float D33D22 = dot(float2(squaredDepthVariance, -L32D22), float2(D22, L32D22));
    float InvD22 = 1.0f / D22;
    float L32 = L32D22 * InvD22;

    // Obtain a scaled inverse image of bz = (1,z[0],z[0]*z[0])^T
    float3 c = float3(1.0f, z[0], z[0] * z[0]);

    // Forward substitution to solve L*c1=bz
    c[1] -= b.x;
    c[2] -= b.y + L32 * c[1];

    // Scaling to solve D*c2=c1
    c[1] *= InvD22;
    c[2] *= D22 / D33D22;

    // Backward substitution to solve L^T*c3=c2
    c[1] -= L32 * c[2];
    c[0] -= dot(c.yz, b.xy);

    // Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions
    // z[1] and z[2]
    float p = c[1] / c[2];
    float q = c[0] / c[2];
    float D = (p * p * 0.25f) - q;
    float r = sqrt(D);
    z[1] =- p * 0.5f - r;
    z[2] =- p * 0.5f + r;

    // Compute the shadow intensity by summing the appropriate weights
    float4 switchVal = (z[2] < z[0]) ? float4(z[1], z[0], 1.0f, 1.0f) :
                      ((z[1] < z[0]) ? float4(z[0], z[1], 0.0f, 1.0f) :
                      float4(0.0f,0.0f,0.0f,0.0f));
    float quotient = (switchVal[0] * z[2] - b[0] * (switchVal[0] + z[2]) + b[1])/((z[2] - switchVal[1]) * (z[0] - z[1]));
    float shadowIntensity = switchVal[2] + switchVal[3] * quotient;
    return 1 - saturate(shadowIntensity);
}

float sampleShadowMSM(Texture2DArray texShadow, SamplerState samShadow, float4 tex, int cascade)
{
    float sample = texShadow.SampleLevel(samShadow, float3(tex.xy, cascade), 0).r;
    float sample2 = sample*sample;
    float4 moments = float4(sample, sample2, sample2*sample, sample2*sample2);
    float shadowIntensity = computeMSMHausdorff(moments, tex.z, shadowMapMomentBias, shadowMapDepthBias);
    //float shadowIntensity = computeMSMHamburger(moments, tex.z, shadowMapMomentBias, shadowMapDepthBias);
    return reduceLightBleeding(shadowIntensity, lightBleedingBias);
}

float sampleShadowOptimizedMSM(Texture2DArray texShadow, SamplerState samShadow, float4 tex, int cascade)
{
    float4 moments = decodeOptimizedMoments(texShadow.SampleLevel(samShadow, float3(tex.xy, cascade), 0));
    float shadowIntensity = computeMSMHausdorff(moments, tex.z, shadowMapMomentBias, shadowMapDepthBias);
    //float shadowIntensity = computeMSMHamburger(moments, tex.z, shadowMapMomentBias, shadowMapDepthBias);
    return reduceLightBleeding(shadowIntensity, lightBleedingBias);
}
#endif

float gatherShadowSample(Texture2DArray texShadow, float4 tex, int cascade)
{
    tex.xy = lightSpaceUV(tex.xy);
#if SHADOWMAP_TYPE==SHADOWMAP_TYPE_NOFILTER
    float shadowInfluence = sampleShadowMapOne(texShadow, samShadow, tex, cascade);

#elif SHADOWMAP_TYPE==SHADOWMAP_TYPE_PCF
    float shadowInfluence = sampleShadowPCF(texShadow, samShadow, tex, cascade);
    //float shadowInfluence = sampleShadowRandomPCF(texShadow, samShadow, tex, cascade);

#elif SHADOWMAP_TYPE==SHADOWMAP_TYPE_MSM
    float shadowInfluence = sampleShadowMSM(texShadow, samShadow, tex, cascade);

#elif SHADOWMAP_TYPE==SHADOWMAP_TYPE_OPTIMIZEDMSM
    float shadowInfluence = sampleShadowOptimizedMSM(texShadow, samShadow, tex, cascade);
#endif

    return shadowInfluence;
}

float calcShadowInfluence(Texture2DArray texShadow, float4 texS[NUM_CASCADES])
{
#if NUM_CASCADES<=1
    float4 tex = texS[0];
    return gatherShadowSample(texShadow, tex, 0);
#else
    for(int i=0; i<NUM_CASCADES; ++i){
        float4 tex = texS[i];
        if(min(tex.x, tex.y)>=-1
            && max(tex.x, tex.y)<=1)
        {
            return gatherShadowSample(texShadow, tex, i);
        }
    }
    return 0;
#endif
}

float calcShadowInfluence(Texture2DArray texShadow, in float4 worldPos, in float3 N, in float3 L, in float4x4 lvp[NUM_CASCADES])
{
    float NdotL = saturate(dot(N,L));
    float3 offset = getSlopeScaledOffset(NdotL, N);
#if NUM_CASCADES<=1
    worldPos.xyz += offset * cascadeScales[0];
    float4 tex = mul(worldPos, lvp[0]);
    tex.z += depthBias;
    return gatherShadowSample(texShadow, tex, 0);
#else
    float4 wpos = worldPos;
    for(int i=0; i<NUM_CASCADES; ++i){
        wpos.xyz = worldPos.xyz + offset * cascadeScales[i];
        float4 tex = mul(wpos, lvp[i]);
        if(min(tex.x, tex.y)>=-1
            && max(tex.x, tex.y)<=1)
        {
            tex.z += depthBias;
            return gatherShadowSample(texShadow, tex, i);
        }
    }
    return 0;
#endif
}

#endif //INC_SHADOW_INC_
