#ifndef INC_TONEMAP_INC_
#define INC_TONEMAP_INC_

static const float A = 0.15 //ShoulderStrength
static const float B = 0.50; //LinearStrength
static const float C = 0.10; //LinearAngle
static const float D = 0.20; //ToeStrength
static const float E = 0.02; //ToeNumerator
static const float F = 0.30; //ToeDenominator
static const float W = 11.2; //LinearWhitePoint

float tonemap1_Uncharted2()
{
    return ((W*(A*W + C*B) + D*E)/(W*(A*W+B) + D*F)) - E/F;
}

float3 tonemap_Uncharted2(float3 hdr)
{
    hdr *= 16.0*2.0;
    float3 color = ((hdr*(A*hdr + C*B) + D*E)/(hdr*(A*hdr+B) + D*F)) - E/F;
    float3 whiteScale = 1.0/tonemap1_Uncharted2();
    return pow(color*whiteScale, 1/2.2);
}

float3 tonemap_Filmic(float3 hdr)
{
    hdr = max(0, hdr-0.004);
    return (hdr*(6.2*hdr+0.5))/(hdr*(6.2*hdr+1.7)+0.06);
}

float3 tonemap_Reinhard(float3 hdr)
{
    hdr *= 16.0;
    hdr = hdr/(hdr+1);
    return pow(hdr, 1/2.2);
}

#endif //INC_TONEMAP_INC_
