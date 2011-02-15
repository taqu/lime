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

#ifdef VBONE
attribute vec4 a_indices0;
#endif


//-------------------------------------
#ifdef PNORMAL
varying vec3 v_normal;
#endif

#ifdef PCOLOR
varying  vec3 v_color0;
#endif

#ifdef PTEX0
varying vec2 v_tex0;
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
#endif


void main()
{
    vec4 position;
    vec3 normal;
#ifdef SKINNING
    skinning(vec4(a_pos0.xyz, float(c_ione)), a_normal0.xyz, position, normal);
#else
    position = vec4(a_pos0.xyz, float(c_ione));
#ifdef V_NORMAL
    normal = a_normal0.xyz;
#endif

#endif
    gl_Position = mwvp * position;

#ifdef PPOS
    v_pos0 = a_pos0.xyz;
#endif

#ifdef PNORMAL
    v_normal = normal;
#endif

#ifdef PTEX0
    v_tex0 = a_tex0;
#endif

#if defined(VDIFFUSE) && defined(PCOLOR)
    vec3 L = dlDir;
    v_color0 = ambient + dlColor.w * max(dlColor.xyz * dot(normal, L), float(c_izero));//Irradiance
#endif
}
