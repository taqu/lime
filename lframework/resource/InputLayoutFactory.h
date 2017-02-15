#ifndef INC_LFRAMEWORK_INPUTLAYOUTFACTORY_H__
#define INC_LFRAMEWORK_INPUTLAYOUTFACTORY_H__
/**
@file InputLayoutFactory.h
@author t-sakai
@date 2016/11/12 create
*/
#include <lcore/Array.h>
#include <lgraphics/InputLayoutRef.h>
#include "../lframework.h"

struct D3D11_INPUT_ELEMENT_DESC;

namespace lfw
{
    enum InputLayoutType
    {
        InputLayout_P =0,
        InputLayout_PN,
        InputLayout_PU,
        InputLayout_PC,
        InputLayout_PNTB,
        InputLayout_PNU,
        InputLayout_PNCU,
        InputLayout_PNTBU,
        InputLayout_PNUBone,
        InputLayout_PNTUBone,

        InputLayout_Sprite2D,
        InputLayout_Particle,
        InputLayout_VolumeParticle,

        InputLayout_UI,
        InputLayout_Num,

        InputLayout_User = InputLayout_Num,
    };

    class InputLayoutFactory
    {
    public:
        InputLayoutFactory();
        ~InputLayoutFactory();

        void loadDefaults();

        void resize(s32 size);
        bool set(s32 id, lgfx::InputLayoutRef& layoutRef);

        lgfx::InputLayoutRef& get(s32 id);
    private:
        InputLayoutFactory(const InputLayoutFactory&);
        InputLayoutFactory& operator=(const InputLayoutFactory&);

        static const s32 IncSize = 16;
        typedef lcore::Array<lgfx::InputLayoutRef, lcore::array_static_inc_size<IncSize> > LayoutArray;

        LayoutArray layouts_;
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
        static D3D11_INPUT_ELEMENT_DESC layoutSprite2D[];
        static D3D11_INPUT_ELEMENT_DESC layoutParticle[];
        static D3D11_INPUT_ELEMENT_DESC layoutVolumeParticle[];
        static D3D11_INPUT_ELEMENT_DESC layoutUI[];

        static u32 numElements_[];
        static D3D11_INPUT_ELEMENT_DESC* elements_[];
    };

    class DefaultInputLayoutCompiledBytes
    {
    public:
        static s32 getSize(s32 id);
        static const u8* getBytes(s32 id);
    private:
        static const u8 bytesP[];
        static const u8 bytesPN[];
        static const u8 bytesPU[];
        static const u8 bytesPC[];
        static const u8 bytesPNTB[];
        static const u8 bytesPNU[];
        static const u8 bytesPNCU[];
        static const u8 bytesPNTBU[];
        static const u8 bytesPNUBone[];
        static const u8 bytesPNTUBone[];
        static const u8 bytesSprite2D[];
        static const u8 bytesParticle[];
        static const u8 bytesVolumeParticle[];
        static const u8 bytesUI[];

        static const u8* bytes_[];
        static const s32 size_[];
    };

}
#endif //INC_LFRAMEWORK_INPUTLAYOUTFACTORY_H__
