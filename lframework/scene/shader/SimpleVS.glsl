"//precision mediump float;\n"
"precision highp float;\n"
"precision mediump int;\n"
"#define NUM_PALETTE_MATRICES 36\n"
"const int c_izero = 0;\n"
"const int c_ione = 1;\n"
"const int c_itwo = 2;\n"
"const int c_ithree = 3;\n"
"const float f_1_100 = 0.01;\n"
"const float c_fzero = 0.0;\n"
"const float c_fone = 1.0;\n"
"uniform mat4 mwvp; //World * View * Projection Matrix\n"
"#if defined(VDIFFUSE) && defined(PCOLOR)\n"
"uniform vec3 dlDir;\n"
"uniform vec4 dlColor;\n"
"uniform vec3 ambient;\n"
"#endif\n"
"//--------------------------------\n"
"attribute vec3 a_pos0;\n"
"#ifdef VNORMAL\n"
"attribute vec4 a_normal0;\n"
"#endif\n"
"#ifdef VTEX0\n"
"attribute vec2 a_tex0;\n"
"#endif\n"
"#ifdef VTEX1\n"
"attribute vec2 a_tex1;\n"
"#endif\n"
"#ifdef VBONE\n"
"attribute vec4 a_indices0;\n"
"#endif\n"
"//-------------------------------------\n"
"#ifdef PNORMAL\n"
"varying vec3 v_normal;\n"
"#endif\n"
"#ifdef PCOLOR\n"
"varying  vec4 v_color0;\n"
"#endif\n"
"#ifdef PTEX0\n"
"varying vec2 v_tex0;\n"
"#endif\n"
"#ifdef PTEX1\n"
"varying vec2 v_tex1;\n"
"#endif\n"
"#ifdef PPOS\n"
"varying vec3 v_pos0;\n"
"#endif\n"
"#ifdef SKINNING\n"
"uniform vec4 palette[NUM_PALETTE_MATRICES*3];\n"
"vec3 skinning_normal(vec3 normal, ivec3 index)\n"
"{\n"
"    vec3 ret;\n"
"    ret.x = dot( normal, palette[index.x].xyz );\n"
"    ret.y = dot( normal, palette[index.y].xyz );\n"
"    ret.z = dot( normal, palette[index.z].xyz );\n"
"    return ret;\n"
"}\n"
"vec3 skinning_position(vec4 position, ivec3 index)\n"
"{\n"
"    vec3 ret;\n"
"    ret.x = dot( position, palette[index.x].xyzw );\n"
"    ret.y = dot( position, palette[index.y].xyzw );\n"
"    ret.z = dot( position, palette[index.z].xyzw );\n"
"    return ret;\n"
"}\n"
"#endif\n"
"#ifdef LIGHTVS\n"
"uniform vec3 dlDir;\n"
"uniform vec3 camPos;\n"
"uniform vec4 specular;\n"
"varying vec4 v_specular0;\n"
"#ifdef TEXSHADE\n"
"varying vec2 v_tex3;\n"
"#endif\n"
"#endif //LIGHTVS\n"
"void main()\n"
"{\n"
"    vec4 position;\n"
"    vec3 normal;\n"
"#ifdef SKINNING\n"
"    ivec3 index0;\n"
"    index0.x = int(a_indices0.x) * c_ithree;\n"
"    index0.yz = ivec2(index0.x + c_ione, index0.x + c_itwo);\n"
"    ivec3 index1;\n"
"    index1.x = int(a_indices0.y) * c_ithree;\n"
"    index1.yz = ivec2(index1.x + c_ione, index1.x + c_itwo);\n"
"    float weight = a_indices0.z * f_1_100;\n"
"    position = vec4(a_pos0.xyz, c_fone);\n"
"    vec3 p0 = skinning_position(position, index0);\n"
"    vec3 p1 = skinning_position(position, index1);\n"
"    position.xyz = mix(p1, p0, weight);\n"
"#if defined(PNORMAL) || (defined(LIGHTVS) && defined(VNORMAL))\n"
"    vec3 n0 = skinning_normal(a_normal0.xyz, index0);\n"
"    vec3 n1 = skinning_normal(a_normal0.xyz, index1);\n"
"    normal = mix(n1, n0, weight);\n"
"#endif //PNORMAL\n"
"#else //SKINNING\n"
"    position = vec4(a_pos0.xyz, c_fone);\n"
"#ifdef VNORMAL\n"
"    normal = a_normal0.xyz;\n"
"#endif //VNORMAL\n"
"#endif //SKINNING\n"
"#ifdef PPOS\n"
"    v_pos0 = position.xyz;\n"
"#endif\n"
"#ifdef LIGHTVS\n"
"    vec3 posLighting = position.xyz;\n"
"#endif\n"
"    position = mwvp * position;\n"
"//linear-z\n"
"    position.z *= position.w;\n"
"    gl_Position = position;\n"
"#ifdef PNORMAL\n"
"    v_normal = normal;\n"
"#endif\n"
"#ifdef PTEX0\n"
"    v_tex0 = a_tex0;\n"
"#endif\n"
"#ifdef PTEX1\n"
"    v_tex1 = a_tex1;\n"
"#endif\n"
"#if defined(VDIFFUSE) && defined(PCOLOR)\n"
"    vec3 L = dlDir;\n"
"    v_color0 = ambient + dlColor.w * max(dlColor.xyz * dot(normal, L), float(c_izero));//Irradiance\n"
"#endif\n"
"///////////////////////////////////////////////////\n"
"#ifdef LIGHTVS\n"
"#ifdef VNORMAL\n"
"#ifdef TEXSHADE\n"
"    vec3 L = dlDir;\n"
"    vec3 N = normalize(normal);\n"
"    vec3 E = normalize(camPos - posLighting);\n"
"    vec3 H = normalize(L+E);\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    v_tex3 = vec2(cosNL);\n"
"    float shininess = specular.w;\n"
"    float rs = pow(cosNH, shininess);\n"
"    v_specular0.xyz = specular.xyz * rs;\n"
"    v_specular0.w = cosNL;\n"
"#else //TEXSHADE\n"
"    vec3 L = dlDir;\n"
"    vec3 N = normalize(normal);\n"
"    vec3 E = normalize(camPos - posLighting);\n"
"    vec3 H = normalize(L+E);\n"
"    float cosNL = max(c_fzero, dot(N,L));\n"
"    float cosNH = max(c_fzero, dot(N,H));\n"
"    float shininess = specular.w;\n"
"    float rs = pow(cosNH, shininess);\n"
"    v_specular0.xyz = specular.xyz * rs;\n"
"    v_specular0.w = cosNL;\n"
"#endif //TEXSHADE\n"
"#else\n"
"    v_specular0.xyz = vec3(c_fzero);\n"
"    v_specular0.w = c_fone;\n"
"#endif\n"
"#endif //LIGHTVS\n"
"}\n"
