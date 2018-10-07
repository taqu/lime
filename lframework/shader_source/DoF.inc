static const int NumBokehSamples = 16;

float calcCoCWeight(float2 centerDepthCoC, float2 sampleDepthCoC, float bleedingBias, float bleedingScale)
{
    float weight = (sampleDepthCoC.x<centerDepthCoC.x)? sampleDepthCoC.y * bleedingScale : 1;
    weight = ((sampleDepthCoC.y+bleedingBias)<centerDepthCoC.y)? weight : 1;
    weight = saturate(weight);
    return weight;
}

float4 dofFilter(
    in Texture2D<float4> texColor,
    in Texture2D<float2> texDepthCoC,
    in SamplerState samp,
    in float2 uv,
    in float bleedingBias,
    in float bleedingScale,
    in float4 offsets[NumBokehSamples/2])
{
    float4 centerColor = texColor.SampleLevel(samp, uv, 0);
    float2 centerDepthCoC = texDepthCoC.SampleLevel(samp, uv, 0);

    float4 color = (float4)0;
    float totalWeight = 0;
    for(int i=0; i<(NumBokehSamples/2); ++i){
        float2 sampleUV = uv + offsets[i].xy*centerDepthCoC.y;
        float4 sampleColor = texColor.SampleLevel(samp, sampleUV, 0);
        float2 sampleDepthCoC = texDepthCoC.SampleLevel(samp, sampleUV, 0);

        float weight = calcCoCWeight(centerDepthCoC, sampleDepthCoC, bleedingBias, bleedingScale);
        color += sampleColor * weight;
        totalWeight += weight;

        sampleUV = uv + offsets[i].zw*centerDepthCoC.y;
        sampleColor = texColor.SampleLevel(samp, sampleUV, 0);
        sampleDepthCoC = texDepthCoC.SampleLevel(samp, sampleUV, 0);

        weight = calcCoCWeight(centerDepthCoC, sampleDepthCoC, bleedingBias, bleedingScale);
        color += sampleColor * weight;
        totalWeight += weight;
    }
    return color/max(totalWeight, 0.0001);
}
