//precision mediump float;
precision highp float;
precision mediump int;

#define MP mediump
//#define MP
#define NUM_PALETTE_MATRICES 36

const MP int c_izero = 0;
const MP int c_ione = 1;
const MP int c_itwo = 2;
const MP int c_ithree = 3;
const MP float f_1_100 = 0.01;
const MP float c_fzero = 0.0;
const MP float c_fone = 1.0;
const MP float c_fhalf = 0.5;

uniform mat4 mwvp; //World * View * Projection Matrix
uniform mat4 mwv; //World * View Matrix

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

vec3 skinning_normal(vec3 normal, MP ivec3 index)
{
    vec3 ret;
    ret.x = dot( normal, palette[index.x].xyz );
    ret.y = dot( normal, palette[index.y].xyz );
    ret.z = dot( normal, palette[index.z].xyz );
    return ret;
}

vec3 skinning_position(vec4 position, MP ivec3 index)
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
varying vec4 v_tex3;
#endif

#endif //LIGHTVS

void main()
{
    vec4 position;
    vec4 normal;

#ifdef SKINNING

    MP ivec3 index0;
    index0.x = int(a_indices0.x) * c_ithree;
    index0.yz = ivec2(index0.x + c_ione, index0.x + c_itwo);

    MP ivec3 index1;
    index1.x = int(a_indices0.y) * c_ithree;
    index1.yz = ivec2(index1.x + c_ione, index1.x + c_itwo);

    MP float weight = a_indices0.z * f_1_100;

    position = vec4(a_pos0.xyz, c_fone);
    vec3 p0 = skinning_position(position, index0);
    vec3 p1 = skinning_position(position, index1);
    position.xyz = mix(p1, p0, weight);

#if defined(PNORMAL) || (defined(LIGHTVS) && defined(VNORMAL))
    vec3 n0 = skinning_normal(a_normal0.xyz, index0);
    vec3 n1 = skinning_normal(a_normal0.xyz, index1);
    normal = vec4(mix(n1, n0, weight), c_fzero);

#endif //PNORMAL

#else //SKINNING

    position = vec4(a_pos0.xyz, c_fone);
#ifdef VNORMAL
    normal = vec4(a_normal0.xyz, c_fzero);
#endif //VNORMAL

#endif //SKINNING


#ifdef PPOS
    v_pos0 = position.xyz;
#endif

#ifdef LIGHTVS
    vec3 posLighting = position.xyz;
#endif

    position *= mwvp;
    normal *= mwv;

//linear-z
    position.z *= position.w;

    gl_Position = position;


#ifdef PNORMAL
    v_normal = normal.xyz;
#endif

#ifdef PTEX0
    v_tex0 = a_tex0;
#endif

#ifdef PTEX1
    v_tex1 = a_tex1;
#endif

#if defined(VDIFFUSE) && defined(PCOLOR)
    //vec3 L = dlDir;
    //v_color0 = ambient + dlColor.w * max(dlColor.xyz * dot(normal, L), float(c_izero));//Irradiance
#endif

///////////////////////////////////////////////////
#ifdef LIGHTVS

#ifdef VNORMAL

#ifdef TEXSHADE
    normal.xyz = normalize(normal.xyz);
    MP vec3 E = normalize(camPos - posLighting);
    MP vec3 H = normalize(dlDir+E);

    MP vec4 tex3;
    tex3.xy = vec2(c_fone + dot(normal.xyz, dlDir));
    tex3.zw = vec2(normal.xy + vec2(c_fone, c_fone));
    v_tex3 = tex3 * c_fhalf;

    MP float cosNH = max(c_fzero, dot(normal.xyz, H));
    v_specular0.xyz = specular.xyz * pow(cosNH, specular.w);
    v_specular0.w = cosNH;

#else //TEXSHADE
    normal.xyz = normalize(normal.xyz);
    MP vec3 E = normalize(camPos - posLighting);
    MP vec3 H = normalize(dlDir+E);

    MP float cosNH = max(c_fzero, dot(normal.xyz, H));

    v_specular0.xyz = specular.xyz *  pow(cosNH, specular.w);
    v_specular0.w = cosNH;
#endif //TEXSHADE

#else
    v_specular0.xyz = vec3(c_fzero);
    v_specular0.w = c_fone;
#endif

#endif //LIGHTVS
}
