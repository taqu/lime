"precision highp float;\n"
"precision mediump int;\n"
"#ifdef PNORMAL\n"
"varying vec3 v_normal;\n"
"#endif\n"
"#ifdef PCOLOR\n"
"varying  vec3 v_color0;\n"
"#endif\n"
"#ifdef PTEX0\n"
"varying vec2 v_tex0;\n"
"#endif\n"
"#ifdef PPOS\n"
"varying vec3 v_pos0;\n"
"#endif\n"
"const float c_fzero = 0.0;\n"
"const float c_fone = 1.0;\n"
"const float PI = 3.14159265358979323846;\n"
"const float PI2 = 6.28318530717958647692;\n"
"const float INV_PI = 0.31830988618379067153;\n"
"const float INV_PI_2 = 0.63661977236758134306;\n"
"uniform vec3 dlDir;\n"
"uniform vec4 dlColor;\n"
"uniform vec3 camPos;\n"
"#ifdef FRESNEL\n"
"const float c_ffive = 5.0;\n"
"const float c_fresnel0 = 0.0397436;\n"
"const float c_fresnel1 = 0.0856832;\n"
"uniform vec4 specular;\n"
"uniform float fresnel;\n"
"// calculate fresnel term for diffuse and specular\n"
"void calc_fresnel(out float fd, out float fs, float F, float eh, float nl)\n"
"{\n"
"  float F2 = c_fone - F;\n"
"  fs = F + F2*pow((c_fone - eh), c_ffive);\n"
"  fd = F + F2*pow((c_fone - nl), c_ffive);\n"
"}\n"
"#elif defined(P_NORMAL)\n"
"const float c_ftwo = 2.0\n"
"uniform vec4 specular;\n"
"#endif\n"
"uniform vec4 diffuse;\n"
"uniform vec3 ambient;\n"
"uniform vec3 emissive;\n"
"#if defined(PTEX0)\n"
"uniform sampler2D texAlbedo;\n"
"#endif\n"
"void main()\n"
"{\n"
"    //-------------\n"
"    vec4 color;\n"
"#if defined(PTEX0)\n"
"    color = texture2D(texAlbedo, v_tex0);\n"
"#else\n"
"    color = diffuse;\n"
"#endif\n"
"    color.xyz *= dlColor.xyz;\n"
"#ifdef FRESNEL\n"
"    //F0: vertical reflectance\n"
"    //N: normal, L:light vector, E: eye ector, H: normalize(L+E)\n"
"    //Fd(F0) = F0 + (1-F0)(1-NL)^5   Fs(F0) = F0 + (1-F0)*(1-EH)^5\n"
"    //approximation (shininess+2)/(4*pi*(2-2^*(-shininess/2)) = (shininess+2.04)/8*pi = 0.0397436 * shininess + 0.0856832\n"
"    //Rd*(1.0/pi)*(1.0 - Rs*Fd(F0))*(NL) + Rs * (shininess+2)/(4*pi*(2-2^(-shininess/2)) * Fs(F0) * NH^shininess\n"
"    vec3 L = dlDir;\n"
"    vec3 N = normalize(v_normal);\n"
"    vec3 E = normalize(camPos - v_pos0);\n"
"    vec3 H = normalize(L+E);\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosEH = max(c_fzero, dot(E,H));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    float fd, fs;\n"
"    calc_fresnel(fd, fs, fresnel, cosEH, cosNL);\n"
"    float shininess = specular.w;\n"
"    float rs = (c_fresnel0 * shininess + c_fresnel1) * fs * pow(cosNH, shininess);\n"
"    vec3 rd = (vec3(c_fone, c_fone, c_fone) - specular.xyz*fd) * INV_PI;\n"
"    color.xyz *= (color.xyz * rd * cosNL + specular.xyz * rs);\n"
"    color.xyz += ambient * rd;\n"
"#elif defined(TEXSHADE)\n"
"#elif defined(PNORMAL)\n"
"    //Rd * (1/pi) cosNL + Rs * (shininess+2)/(2*pi) * NH^shininess\n"
"    vec3 L = dlDir;\n"
"    vec3 N = normalize(v_normal);\n"
"    vec3 E = normalize(camPos - v_pos0);\n"
"    vec3 H = normalize(L+E);\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    float shininess = specular.w;\n"
"    float rs = (shininess+c_ftwo) * PI2 * pow(cosNH, shininess);\n"
"    color.xyz *= (color.xyz * cosNL * INV_PI + specular.xyz * rs );\n"
"    color.xyz += ambient * INV_PI;\n"
"#endif\n"
"#if defined(PCOLOR)\n"
"    color.xyz *= v_color0;\n"
"#endif\n"
"#ifdef EMISSIVE\n"
"    color.xyz += emissive;\n"
"#endif\n"
"    gl_FragColor = color;\n"
"}\n"
