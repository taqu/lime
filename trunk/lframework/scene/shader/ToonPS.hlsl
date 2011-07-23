"struct VS_OUTPUT\n"
"{\n"
"    float4 position : POSITION0;\n"
"#ifdef PNORMAL\n"
"    float3 normal : TEXCOORD0;\n"
"#endif\n"
"#ifdef PCOLOR\n"
"    float4 color : COLOR0;\n"
"#endif\n"
"#ifdef PTEX0\n"
"    float4 tex0 : TEXCOORD1;\n"
"#endif\n"
"#ifdef PPOS\n"
"    float3 pos0 : TEXCOORD2;\n"
"#endif\n"
"#ifdef LIGHTVS\n"
"    float4 specular0 : TEXCOORD3;\n"
"#ifdef TEXSHADE\n"
"    float2 tex2 : TEXCOORD4;\n"
"#endif\n"
"#endif //LIGHTVS\n"
"};\n"
"static const float c_fzero = 0.0;\n"
"static const float c_fone = 1.0;\n"
"static const float PI = 3.14159265358979323846;\n"
"static const float PI2 = 6.28318530717958647692;\n"
"static const float INV_PI = 0.31830988618379067153;\n"
"static const float INV_PI2 = 0.15915494309189533576;\n"
"////////////////////////////////////\n"
"#ifdef LIGHTVS\n"
"uniform float3 ambient;\n"
"uniform float4 dlColor;\n"
"uniform float4 diffuse;\n"
"uniform float3 emissive;\n"
"#else //LIGHTVS\n"
"uniform float3 dlDir;\n"
"uniform float4 dlColor;\n"
"uniform float3 camPos;\n"
"uniform float4 diffuse;\n"
"uniform float3 ambient;\n"
"uniform float3 emissive;\n"
"#endif //LIGHTVS\n"
"// textures\n"
"#ifdef PTEX0\n"
"sampler texAlbedo;\n"
"sampler texAlbedo2;\n"
"#endif\n"
"#ifdef FRESNEL\n"
"static const float c_ffive = 5.0;\n"
"static const float c_fresnel0 = 0.0397436;\n"
"static const float c_fresnel1 = 0.0856832;\n"
"uniform float4 specular;\n"
"uniform float fresnel;\n"
"// calculate fresnel term for diffuse and specular\n"
"void calc_fresnel(out float fd, out float fs, float F, float eh, float nl)\n"
"{\n"
"  float F2 = c_fone - F;\n"
"  fs = F + F2*pow((c_fone - eh), c_ffive);\n"
"  fd = F + F2*pow((c_fone - nl), c_ffive);\n"
"}\n"
"#elif defined(TEXSHADE) || defined(PNORMAL)\n"
"static const float c_fhalf = 0.5;\n"
"static const float c_fratio = 0.75;\n"
"static const float c_ffive = 5.0;\n"
"static const float c_fresnel0 = 0.0397436;\n"
"static const float c_fresnel1 = 0.0856832;\n"
"static const float c_fresnel = 1.5;\n"
"uniform float4 specular;\n"
"// calculate fresnel term for diffuse and specular\n"
"void calc_fresnel(out float fd, out float fs, float eh, float nl)\n"
"{\n"
"  float F2 = c_fone - c_fresnel;\n"
"  fs = c_fresnel + F2*pow((c_fone - eh), c_ffive);\n"
"  fd = c_fresnel + F2*pow((c_fone - nl), c_ffive);\n"
"}\n"
"#endif\n"
"float4 main(VS_OUTPUT input) : COLOR\n"
"{\n"
"    // č²čØē®E\n"
"    //-------------\n"
"    float4 color;\n"
"#ifdef TEXALBEDO\n"
"    //ćE£ćć„ć¼ćŗćEÆć¹ćć£åēE\n"
"    color = tex2D(texAlbedo, input.tex0.xy);\n"
"#else\n"
"    color = diffuse;\n"
"#endif\n"
"#ifdef PCOLOR\n"
"    color *= input.color0;\n"
"#endif\n"
"#ifdef LIGHTVS\n"
"#ifdef TEXSHADE\n"
"    float3 c = tex2D(texAlbedo2, input.tex2).xyz;\n"
"    float3 power = min(dlColor.xyz * diffuse.xyz + ambient, c_fone);\n"
"    color.xyz = (color.xyz * c) * power + dlColor.xyz*input.specular0.xyz;\n"
"#else\n"
"    float3 power = min(dlColor.xyz * diffuse.xyz + ambient, c_fone);\n"
"    color.xyz = (color.xyz * input.specular0.w) * power + dlColor.xyz*input.specular0.xyz;\n"
"#endif //TEXSHADE\n"
"#elif defined(TEXSHADE)\n"
"    float3 N = normalize(input.normal);\n"
"    float3 L = dlDir;\n"
"    float3 E = normalize(camPos - input.pos0);\n"
"    float3 H = normalize(L+E); //half vector\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    //float cosNE = max(c_fzero, dot(N,E));\n"
"    float2 sample;\n"
"    //sample.x = cosNE; //ćć”ćåEćÆć«ćE\n"
"    sample.x = cosNL;\n"
"    sample.y = cosNL;\n"
"    float3 c = tex2D(texAlbedo2, sample).xyz;\n"
"    float shininess = specular.w;\n"
"    float rs = pow(cosNH, shininess);\n"
"    color.xyz = (color.xyz * c + specular.xyz * rs) * dlColor.xyz + ambient * color.xyz;\n"
"#elif defined(PNORMAL)\n"
"    float3 N = normalize(v_normal);\n"
"    float3 L = dlDir;\n"
"    float3 E = normalize(camPos - v_pos0);\n"
"    float3 H = normalize(L+E); //half vector\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    //float cosNE = max(c_fzero, dot(N,E));\n"
"    float shininess = specular.w;\n"
"    float rs = pow(cosNH, shininess);\n"
"    color.xyz = (color.xyz * cosNH + specular.xyz * rs) * dlColor.xyz + ambient * color.xyz;\n"
"#else\n"
"    color.xyz *= dlColor.xyz;\n"
"#endif\n"
"#ifdef EMISSIVE\n"
"    color.xyz += emissive;\n"
"#endif\n"
"    return color;\n"
"}\n"
