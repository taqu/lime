//precision mediump float;
precision highp float;
precision mediump int;

#define NUM_PALETTE_MATRICES 36

const int c_izero = 0;
const int c_ione = 1;
const int c_itwo = 2;
const int c_ithree = 3;
const float f_1_100 = 0.01;
const float c_fzero = 0.0;
const float c_fone = 1.0;

uniform mat4 mwvp; //World * View * Projection Matrix

#if defined(VDIFFUSE) && defined(PCOLOR)

uniform vec3 dlDir;
uniform vec4 dlColor;

uniform vec3 ambient;

#endif


//--------------------------------

attribute vec3 a_pos0;

#ifdef VNORMAL
attribute vec4 a_normal0;
#endif

#ifdef VTEX0
attribute vec2 a_tex0;
#endif

#ifdef VTEX1
attribute vec2 a_tex1;
#endif

#ifdef VBONE
attribute vec4 a_indices0;
#endif


//-------------------------------------
#ifdef PNORMAL
varying vec3 v_normal;
#endif

#ifdef PCOLOR
varying  vec4 v_color0;
#endif

#ifdef PTEX0
varying vec2 v_tex0;
#endif

#ifdef PTEX1
varying vec2 v_tex1;
#endif

#ifdef PPOS
varying vec3 v_pos0;
#endif


#ifdef SKINNING
uniform vec4 palette[NUM_PALETTE_MATRICES*3];

vec3 skinning_normal(vec3 normal, ivec3 index)
{
    vec3 ret;
    ret.x = dot( normal, palette[index.x].xyz );
    ret.y = dot( normal, palette[index.y].xyz );
    ret.z = dot( normal, palette[index.z].xyz );
    return ret;
}

vec3 skinning_position(vec4 position, ivec3 index)
{
    vec3 ret;
    ret.x = dot( position, palette[index.x].xyzw );
    ret.y = dot( position, palette[index.y].xyzw );
    ret.z = dot( position, palette[index.z].xyzw );
    return ret;
}

#endif

#ifdef LIGHTVS
uniform vec3 dlDir;
uniform vec3 camPos;

uniform vec4 specular;

varying vec4 v_specular0;

#ifdef TEXSHADE
varying vec2 v_tex3;
#endif

#endif //LIGHTVS

void main()
{
    vec4 position;
    vec3 normal;

#ifdef SKINNING

    ivec3 index0;
    index0.x = int(a_indices0.x) * c_ithree;
    index0.yz = ivec2(index0.x + c_ione, index0.x + c_itwo);

    ivec3 index1;
    index1.x = int(a_indices0.y) * c_ithree;
    index1.yz = ivec2(index1.x + c_ione, index1.x + c_itwo);

    float weight = a_indices0.z * f_1_100;

    position = vec4(a_pos0.xyz, c_fone);
    vec3 p0 = skinning_position(position, index0);
    vec3 p1 = skinning_position(position, index1);
    position.xyz = mix(p1, p0, weight);

#if defined(PNORMAL) || (defined(LIGHTVS) && defined(VNORMAL))
    vec3 n0 = skinning_normal(a_normal0.xyz, index0);
    vec3 n1 = skinning_normal(a_normal0.xyz, index1);
    normal = mix(n1, n0, weight);

#endif //PNORMAL

#else //SKINNING

    position = vec4(a_pos0.xyz, c_fone);
#ifdef VNORMAL
    normal = a_normal0.xyz;
#endif //VNORMAL

#endif //SKINNING


#ifdef PPOS
    v_pos0 = position.xyz;
#endif

#ifdef LIGHTVS
    vec3 posLighting = position.xyz;
#endif

    position = mwvp * position;

//linear-z
    position.z *= position.w;

    gl_Position = position;


#ifdef PNORMAL
    v_normal = normal;
#endif

#ifdef PTEX0
    v_tex0 = a_tex0;
#endif

#ifdef PTEX1
    v_tex1 = a_tex1;
#endif

#if defined(VDIFFUSE) && defined(PCOLOR)
    vec3 L = dlDir;
    v_color0 = ambient + dlColor.w * max(dlColor.xyz * dot(normal, L), float(c_izero));//Irradiance
#endif

///////////////////////////////////////////////////
#ifdef LIGHTVS

#ifdef VNORMAL

#ifdef TEXSHADE
    vec3 L = dlDir;
    vec3 N = normalize(normal);
    vec3 E = normalize(camPos - posLighting);
    vec3 H = normalize(L+E);

    float cosNL = max(c_fzero, dot(N,L));
    float cosNH = max(c_fzero, dot(N,H));

    v_tex3 = vec2(cosNL);

    float shininess = specular.w;

    float rs = pow(cosNH, shininess);
    v_specular0.xyz = specular.xyz * rs;
    v_specular0.w = cosNL;

#else //TEXSHADE
    vec3 L = dlDir;
    vec3 N = normalize(normal);
    vec3 E = normalize(camPos - posLighting);
    vec3 H = normalize(L+E);

    float cosNL = max(c_fzero, dot(N,L));
    float cosNH = max(c_fzero, dot(N,H));

    float shininess = specular.w;

    float rs = pow(cosNH, shininess);
    v_specular0.xyz = specular.xyz * rs;
    v_specular0.w = cosNL;
#endif //TEXSHADE

#else
    v_specular0.xyz = vec3(c_fzero);
    v_specular0.w = c_fone;
#endif

#endif //LIGHTVS
}
