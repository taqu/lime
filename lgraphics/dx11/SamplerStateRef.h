#ifndef INC_LGRAPHICS_DX11_SAMPLERSTATEREF_H__
#define INC_LGRAPHICS_DX11_SAMPLERSTATEREF_H__
/**
@file SamplerStateRef.h
@author t-sakai
@date 2012/07/23 create
*/
#include "../lgraphics.h"
#include "Enumerations.h"

struct ID3D11SamplerState;

namespace lgfx
{
    class ContextRef;

    //------------------------------------------------------------
    //---
    //--- SamplerStateRef
    //---
    //------------------------------------------------------------
    class SamplerStateRef
    {
    public:
        typedef ID3D11SamplerState element_type;
        typedef ID3D11SamplerState* pointer_type;
        typedef pointer_type const* pointer_array_type;

        SamplerStateRef()
            :state_(NULL)
        {}

        SamplerStateRef(const SamplerStateRef& rhs);
        SamplerStateRef(SamplerStateRef&& rhs);


        ~SamplerStateRef()
        {
            destroy();
        }

        void destroy();

        pointer_type get(){ return state_;}
        operator pointer_type(){ return state_; }
        operator pointer_array_type(){ return &state_; }

        bool valid() const{ return (NULL != state_);}

        void attachVS(ContextRef& context, u32 start);
        void attachHS(ContextRef& context, u32 start);
        void attachDS(ContextRef& context, u32 start);
        void attachGS(ContextRef& context, u32 start);
        void attachPS(ContextRef& context, u32 start);
        void attachCS(ContextRef& context, u32 start);

        SamplerStateRef& operator=(const SamplerStateRef& rhs);
        SamplerStateRef& operator=(SamplerStateRef&& rhs);

        void swap(SamplerStateRef& rhs);
    private:
        friend class SamplerState;

        explicit SamplerStateRef(pointer_type state)
            :state_(state)
        {}

        pointer_type state_;
    };

    //------------------------------------------------------------
    //---
    //--- SamplerState
    //---
    //------------------------------------------------------------
    class SamplerState
    {
    public:
        static SamplerStateRef create(
            TextureFilterType filter,
            TextureAddress addressU,
            TextureAddress addressV,
            TextureAddress addressW,
            CmpFunc compFunc,
            f32 borderColor = 0.0f);

        static SamplerStateRef create(
            TextureFilterType filter,
            TextureAddress address,
            CmpFunc compFunc,
            f32 borderColor);
    };
}
#endif //INC_LGRAPHICS_DX11_SAMPLERSTATEREF_H__
