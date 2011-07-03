"struct VS_OUTPUT\n"
"{\n"
"    float4 position : POSITION0;\n"
"#ifdef PNORMAL\n"
"    float3 normal : TEXCOORD1;\n"
"#endif\n"
"#ifdef PCOLOR\n"
"    float3 color : COLOR0;\n"
"#endif\n"
"#ifdef PTEX0\n"
"    float2 tex0 : TEXCOORD0;\n"
"#endif\n"
"#ifdef PPOS\n"
"    float3 pos0 : TEXCOORD2;\n"
"#endif\n"
"};\n"
"static const float c_fzero = 0.0;\n"
"static const float c_fone = 1.0;\n"
"static const float PI = 3.14159265358979323846;\n"
"static const float PI2 = 6.28318530717958647692;\n"
"static const float INV_PI = 0.31830988618379067153;\n"
"static const float INV_PI2 = 0.15915494309189533576;\n"
"uniform float3 dlDir;\n"
"uniform float4 dlColor;\n"
"uniform float3 camPos;\n"
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
"//#elif defined(P_NORMAL)\n"
"//static const float c_ftwo = 2.0\n"
"//uniform float4 specular;\n"
"#endif\n"
"//マテリアル設宁E\n"
"uniform float4 diffuse;\n"
"uniform float3 ambient;\n"
"uniform float3 eimissive;\n"
"//チE��スチャ\n"
"#ifdef PTEX0\n"
"sampler texAlbedo;\n"
"sampler texAlbedo2;\n"
"#endif\n"
"float4 main(VS_OUTPUT input) : COLOR\n"
"{\n"
"    // 色計箁E\n"
"    //-------------\n"
"    float4 color;\n"
"#if defined(TEXALBEDO)\n"
"    //チE��フューズチE��スチャ参�E\n"
"    color = tex2D(texAlbedo, input.tex0);\n"
"    color *= diffuse;\n"
"#else\n"
"    color = diffuse;\n"
"#endif\n"
"#ifdef FRESNEL\n"
"    float3 L = dlDir;\n"
"    float3 N = normalize(input.normal);\n"
"    float3 E = normalize(camPos - input.pos0);\n"
"    float3 H = normalize(L+E); //ハ�Eフ�Eクトル\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosEH = max(c_fzero, dot(E,H));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    float fd, fs;\n"
"    calc_fresnel(fd, fs, fresnel, cosEH, cosNL);\n"
"    float shininess = specular.w;\n"
"    float rs = (c_fresnel0 * shininess + c_fresnel1) * fs * pow(cosNH, shininess);\n"
"    float3 rd = (float3(c_fone, c_fone, c_fone) - specular.xyz*fd) * INV_PI;\n"
"    color.xyz = (color.xyz * rd * cosNL + specular.xyz * rs) * dlColor.xyz + ambient * rd;\n"
"#elif defined(TEXSHADE)\n"
"    float3 N = normalize(input.normal);\n"
"    float3 L = dlDir;\n"
"    float3 E = normalize(camPos - input.pos0);\n"
"    //float3 E = normalize(input.pos0 - camPos);\n"
"    float3 H = normalize(L+E); //ハ�Eフ�Eクトル\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosEH = max(c_fzero, dot(E,H));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    float cosNE = max(c_fzero, dot(N,E));\n"
"    float2 sample;\n"
"    sample.x = cosNE;\n"
"    sample.y = cosNL;\n"
"    float3 c = tex2D(texAlbedo2, sample).xyz;\n"
"    float fd, fs;\n"
"    calc_fresnel(fd, fs, cosEH, cosNL);\n"
"    float shininess = specular.w;\n"
"    float rs = (c_fresnel0 * shininess + c_fresnel1) * fs * pow(cosNH, shininess);\n"
"    float3 rd = (float3(c_fone, c_fone, c_fone) - specular.xyz*fd);\n"
"    color.xyz = (color.xyz * c * rd + specular.xyz * rs) * dlColor.xyz + ambient * color.xyz;\n"
"#elif defined(PNORMAL)\n"
"    float3 N = normalize(input.normal);\n"
"    float3 L = dlDir;\n"
"    float3 E = normalize(camPos - input.pos0);\n"
"    float3 H = normalize(L+E); //ハ�Eフ�Eクトル\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosEH = max(c_fzero, dot(E,H));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    float fd, fs;\n"
"    calc_fresnel(fd, fs, cosEH, cosNL);\n"
"    float shininess = specular.w;\n"
"    float rs = (c_fresnel0 * shininess + c_fresnel1) * fs * pow(cosNH, shininess);\n"
"    float3 rd = (float3(c_fone, c_fone, c_fone) - specular.xyz*fd);\n"
"    color.xyz = (color.xyz * rd * cosNH + specular.xyz * rs) * dlColor.xyz + ambient * color.xyz;\n"
"#else\n"
"    color.xyz *= dlColor.xyz;\n"
"#endif\n"
"#ifdef PCOLOR\n"
"    color.xyz *= input.color;\n"
"#endif\n"
"#ifdef EMISSIVE\n"
"    color.xyz += emissive;\n"
"#endif\n"
"    return color;\n"
"}\n"
