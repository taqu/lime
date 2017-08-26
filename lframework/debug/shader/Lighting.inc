
struct DirectionalLight
{
    float4 color;
    float4 direction;
};

struct PointLight
{
    float4 color;
    float4 position; //position, radius
};

struct SpotLight
{
    float4 color;
    float4 position; //position, angle
    float4 direction; //direction, falloffAngle
};

struct IncidentLight
{
    float3 color;
    float3 direction;
};

struct Material
{
    float3 albedo;
    float3 specular;
    float roughness;
    float metalic;
    float ambientOcclusion;
};

#if 0
half3 gatherLights(float3 worldPosition, float3 viewPosition, float3 normal)
{
    int4 clusterPosition = int4(viewPosition * lightClusterScale.xyz + lightClusterBias.xyz, 0);

    half3 lit = (half3)0;

    uint2 cluster = lightClusters.Load(clusterPosition);
    uint lightIndex = cluster.x;
    uint lightCount = cluster.y;
    uint2 lightIndexCoord;
    for(uint pl=0; pl<lightCount; ++pl){
        lightIndexCoord.x = lightIndex & 63;
        lightIndexCoord.y = lightIndex >> 6;
        uint index = lightIndices[lightIndexCoord];
        ++lightIndex;

        const PointLight light = lightBuffer[index];

		// Lighting vectors
		float3 vec = light.position - worldPosition;
		float3 dir = normalize(vec);

		float attenuation = saturate(1.0 - dot(vec, vec) * rcp(light.attenuation1 * light.attenuation1));
        attenuation *= attenuation;

		// Lighting
		float diffuse = saturate(dot(dir, normal));

		lit += half3(light.color * (attenuation * diffuse));
    }
    return lit;
}
#endif

//
float3 fresnel(in float3 F0, in float3 F, in float x)
{
    return F0 + (F-F0)*pow(1.0-x,5.0);
}

float3 envFresnel(float3 F0, float cosNV, float alpha)
{
    float3 F1 = max(1-alpha, F0)- F0;
    return F0 + F1*pow(1.0-cosNV, 5.0);
}

float diffuseLambert(in float cosNL)
{
    return cosNL*(1.0/PI);
}

float diffuseBurley(in float cosNL, in float cosNV, in float cosLH, in float roughness)
{
    float F = 0.5 + 2.0 * roughness * cosLH * cosLH;
    return fresnel(1.0, F, cosNL).x * fresnel(1.0, F, cosNV).x * (1.0/PI);
}

//Generalized-Trowbridge-Reitz. alpha = roughness^2
float normalDistributionGGX(in float cosNH, in float alpha)
{
    float alpha2 = alpha*alpha;
    float div = cosNH*cosNH*(alpha2-1.0) + 1.0;
    return alpha2/(PI*div*div);
}

// Optimized Smith GGX. alpha = roughness^2
// Smith GGX= G(l,v,h)/4(n,l)(n,v)
float visibilitySmith(in float cosNL, in float cosNE, in float alpha)
{
    float k = 0.5*alpha;
    float k1 = 1-k;
    float GL = (cosNL*k1 + k);
    float GV = (cosNE*k1 + k);
    return 0.25/(GL*GV+EPSILON);
}

float visibilitySmithHable(in float cosLH, in float alpha)
{
    float k = 0.5*alpha;
    float k2 = k*k;
    return rcp(cosLH*cosLH*(1.0-k2) + k2 + EPSILON);
}

float3 specularBRDF(
    in float alpha,
    in float3 color,
    in float cosNV,
    in float cosNL,
    in float cosLH,
    in float cosNH)
{
    float D = normalDistributionGGX(cosNH, alpha);
    float3 F = fresnel(color, 1, cosLH);
    float G = visibilitySmithHable(cosLH, alpha);
    return D*F*G;
}

float3 irradianceDiffuse(TextureCube<float3> texIrradiance, SamplerState samCube, in float3 N)
{
    return texIrradiance.SampleLevel(samCube, N, 0).xyz;
}

float3 radianceSpecular(
    TextureCube<float3> texRadiance,
    SamplerState samCube,
    in float3 N,
    in float3 V,
    in float lod,
    in float mipLevels)
{
    return texRadiance.SampleLevel(samCube, reflect(-V,N), lod*mipLevels).xyz;
}

float3 lightSurface(
    in float3 N,
    in float3 V,
    in IncidentLight incident,
    in Material material,
    in float shadowAttenuation,
    in EnvironmentLight envLight)
{
    const float3 diffuseColor = lerp(material.albedo, float3(0,0,0), material.metalic) * material.ambientOcclusion;
    //const float3 specularColor = lerp(0.04, material.albedo, material.metalic) * material.ambientOcclusion;
    const float3 specularColor = material.specular * lerp(float3(0.04,0.04,0.04), material.albedo, material.metalic) * material.ambientOcclusion;
    float alpha = material.roughness*material.roughness;

    incident.color *= PI;
    const float cosNV = saturate(dot(N,V));
    float3 accum = (float3)0;
    float3 L = incident.direction;
    float3 H = normalize(L+V);
    float cosNL = saturate(dot(N,L));
    float cosNH = saturate(dot(N,H)); 
    float cosLH = saturate(dot(L,H));

    float diffuse = diffuseLambert(cosNL);
    float3 specular = specularBRDF(alpha, specularColor, cosNV, cosNL, cosLH, cosNH);
    accum += cosNL * incident.color * (diffuseColor*diffuse + specular);
    accum += diffuseColor * envLight.diffuse_;
    accum += specularColor * envLight.specular_;
    shadowAttenuation *= step(0,cosNL);
    return accum*shadowAttenuation;
}

void irradianceDirectionalLight(out IncidentLight incident, in DirectionalLight dlight)
{
    incident.color = dlight.color.xyz;
    incident.direction = dlight.direction.xyz;
}

void irradiancePointLight(out IncidentLight incident, in PointLight pligt, float3 position)
{
    float3 d = pligt.position.xyz - position;
    float distance = length(d);
    incident.direction = d/distance;

    float attenuation = saturate(1.0-distance/pligt.position.w);
    incident.color = pligt.color.xyz * (attenuation*attenuation);
}

void irradianceSpotLight(out IncidentLight incident, in SpotLight spot, float3 position)
{
    float3 d = spot.position.xyz - position;
    float distance = length(d);
    incident.direction = d/distance;

    float cosine = dot(-incident.direction, spot.direction.xyz);
    float attenuation = step(cosine, spot.position.w);
    float falloff = 1.0 - smoothstep(spot.direction.w, spot.position.w, cosine);
    incident.color = spot.color.xyz * falloff * (attenuation*attenuation);
}
