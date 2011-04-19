//precision mediump float;
precision highp float;
precision mediump int;

#define NUM_PALETTE_MATRICES 36

const int c_izero = 0;
const int c_ione = 1;
const int c_itwo = 2;
const int c_ithree = 3;
const float f_1_100 = 0.01;

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

void skinning_position(inout vec4 ret, in vec4 position, float weight, int index)
{
    vec4 tmp;
    tmp.x = dot(position, palette[index      ]);
    tmp.y = dot(position, palette[index+c_ione]);
    tmp.z = dot(position, palette[index+c_itwo]);
    tmp.w = position.w;

    ret += weight * tmp;
}

void skinning_normal(inout vec3 ret, in vec3 normal, float weight, int index)
{
    vec3 tmp;
    tmp.x = dot(normal, palette[index      ].xyz);
    tmp.y = dot(normal, palette[index+c_ione].xyz);
    tmp.z = dot(normal, palette[index+c_itwo].xyz);

    ret += weight * tmp;
}

#if defined(PNORMAL) || (defined(LIGHTVS) && defined(VNORMAL))
void skinning(in vec4 position, in vec3 normal, out vec4 retPosition, out vec3 retNormal)
{
    retPosition = vec4(float(c_izero));
    retNormal = vec3(float(c_izero));

    float weight = a_indices0.z * f_1_100;
    int index = int(a_indices0.x) * c_ithree;
    skinning_position(retPosition, position, weight, index);
    skinning_normal(retNormal, normal, weight, index);

    weight = float(c_ione) - weight;
    index = int(a_indices0.y) * c_ithree;
    skinning_position(retPosition, position, weight, index);
    skinning_normal(retNormal, normal, weight, index);
}
#else

void skinning(in vec4 position, out vec4 retPosition)
{
    retPosition = vec4(float(c_izero));

    float weight = a_indices0.z * f_1_100;
    int index = int(a_indices0.x) * c_ithree;
    skinning_position(retPosition, position, weight, index);

    weight = float(c_ione) - weight;
    index = int(a_indices0.y) * c_ithree;
    skinning_position(retPosition, position, weight, index);
}
#endif

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

#if defined(PNORMAL) || (defined(LIGHTVS) && defined(VNORMAL))
    skinning(vec4(a_pos0.xyz, float(c_ione)), a_normal0.xyz, position, normal);

#else
    skinning(vec4(a_pos0.xyz, float(c_ione)), position);
#endif //PNORMAL

#else //SKINNING

    position = vec4(a_pos0.xyz, float(c_ione));
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

    float cosNL = max(float(c_izero), dot(N,L));
    float cosNH = max(float(c_izero), dot(N,H));

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

    float cosNL = max(float(c_izero), dot(N,L));
    float cosNH = max(float(c_izero), dot(N,H));

    float shininess = specular.w;

    float rs = pow(cosNH, shininess);
    v_specular0.xyz = specular.xyz * rs;
    v_specular0.w = cosNL;
#endif //TEXSHADE

#else
    v_specular0.xyz = vec3(float(c_izero));
    v_specular0.w = float(c_ione);
#endif

#endif //LIGHTVS
}
