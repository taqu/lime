#ifndef INC_LSCENE_LRENDER_INPUTLAYOUTMANAGER_H__
#define INC_LSCENE_LRENDER_INPUTLAYOUTMANAGER_H__
/**
@file InputLayoutManager.h
@author t-sakai
@date 2014/12/09 create
*/
#include <lcore/Vector.h>
#include <lgraphics/api/InputLayoutRef.h>
#include "render.h"

struct D3D11_INPUT_ELEMENT_DESC;

namespace lscene
{
namespace lrender
{
    enum Layout
    {
        Layout_P =0,
        Layout_PN,
        Layout_PU,
        Layout_PC,
        Layout_PNTB,
        Layout_PNU,
        Layout_PNCU,
        Layout_PNTBU,
        Layout_PNUBone,
        Layout_PNTUBone,
        Layout_Num,

        Layout_User = 1000,
    };

    class InputLayoutManager : public InputLayoutManagerAllocator
    {
    public:
        static void initialize();
        static void terminate();
        static InputLayoutManager& getInstance(){ return *instance_;}

        bool add(s32 id, lgraphics::InputLayoutRef& layoutRef);

        lgraphics::InputLayoutRef* find(s32 id);
        lgraphics::InputLayoutRef& get(s32 id){ return *find(id);}
    private:
        InputLayoutManager(const InputLayoutManager&);
        InputLayoutManager& operator=(const InputLayoutManager&);

        InputLayoutManager();
        ~InputLayoutManager();

        struct Layout
        {
            s32 id_;
            lgraphics::InputLayoutRef layout_;
        };

        typedef lcore::vector_arena<Layout, lscene::SceneAllocator, lcore::vector_arena_static_inc_size<16> > LayoutVector;

        struct CompFunc
        {
            s32 operator()(const Layout& v0, const s32& id) const
            {
                return v0.id_ - id;
            }
        };

        static InputLayoutManager* instance_;

        LayoutVector layouts_;
    };

    class DefaultInputLayoutElement
    {
    public:
        static u32 getNumElements(s32 id);
        static D3D11_INPUT_ELEMENT_DESC* getElements(s32 id);

    private:
        static D3D11_INPUT_ELEMENT_DESC layoutP[];
        static D3D11_INPUT_ELEMENT_DESC layoutPN[];
        static D3D11_INPUT_ELEMENT_DESC layoutPU[];
        static D3D11_INPUT_ELEMENT_DESC layoutPC[];
        static D3D11_INPUT_ELEMENT_DESC layoutPNTB[];
        static D3D11_INPUT_ELEMENT_DESC layoutPNU[];
        static D3D11_INPUT_ELEMENT_DESC layoutPNCU[];
        static D3D11_INPUT_ELEMENT_DESC layoutPNTBU[];
        static D3D11_INPUT_ELEMENT_DESC layoutPNUBone[];
        static D3D11_INPUT_ELEMENT_DESC layoutPNTUBone[];

        static u32 numElements_[];
        static D3D11_INPUT_ELEMENT_DESC* elements_[];
    };

    template<s32 T>
    class DefaultInputLayoutSource
    {
    public:
        static const Char* getSource(s32 id);
    private:
        static const Char sourceP[];
        static const Char sourcePN[];
        static const Char sourcePU[];
        static const Char sourcePC[];
        static const Char sourcePNTB[];
        static const Char sourcePNU[];
        static const Char sourcePNCU[];
        static const Char sourcePNTBU[];
        static const Char sourcePNUBone[];
        static const Char sourcePNTUBone[];

        static const Char* sources_[];
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourceP[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "};"
        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePN[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "};"
        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePC[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 color : COLOR;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePNTB[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 tangent : TANGENT;"
        "float4 binormal : BINORMAL;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePNU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePNCU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 color : COLOR;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"

    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePNTBU[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 tangent : TANGENT;"
        "float4 binormal : BINORMAL;"
        "float2 tex0 : TEXCOORD0;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePNUBone[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float2 uv : TEXCOORD0;"
        "uint2 bone : BLENDINDICES;"
        "float2 weight : BLENDWEIGHT;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char DefaultInputLayoutSource<T>::sourcePNTUBone[] =
    {
        "struct VSInput"
        "{"
        "float3 position : POSITION;"
        "float4 normal : NORMAL;"
        "float4 tangent : TANGENT;"
        "float2 uv : TEXCOORD0;"
        "uint2 bone : BLENDINDICES;"
        "float2 weight : BLENDWEIGHT;"
        "};"

        "float4 main( VSInput input ) : SV_POSITION "
        "{"
        "return float4(input.position, 1);"
        "}"
    };

    template<s32 T>
    const Char* DefaultInputLayoutSource<T>::getSource(s32 id)
    {
        LASSERT(0<=id && id<Layout_Num);
        return sources_[id];
    }

    template<s32 T>
    const Char* DefaultInputLayoutSource<T>::sources_[] =
    {
        sourceP,
        sourcePN,
        sourcePU,
        sourcePC,
        sourcePNTB,
        sourcePNU,
        sourcePNCU,
        sourcePNTBU,
        sourcePNUBone,
        sourcePNTUBone,
    };
}
}
#endif //INC_LSCENE_LRENDER_INPUTLAYOUTMANAGER_H__
