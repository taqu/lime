#ifndef INC_LGRAPHICS_DX11_SAMPLERSTATEREF_H__
#define INC_LGRAPHICS_DX11_SAMPLERSTATEREF_H__
/**
@file SamplerStateRef.h
@author t-sakai
@date 2012/07/23 create
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"

struct ID3D11SamplerState;

namespace lgraphics
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

        SamplerStateRef()
            :state_(NULL)
        {}

        SamplerStateRef(const SamplerStateRef& rhs);

        ~SamplerStateRef()
        {
            destroy();
        }

        void destroy();

        pointer_type getSampler(){ return state_;}
        pointer_type const* get(){ return &state_;}
        bool valid() const{ return (NULL != state_);}

        void setVS(ContextRef& context, u32 start);
        void setGS(ContextRef& context, u32 start);
        void setPS(ContextRef& context, u32 start);
        void setCS(ContextRef& context, u32 start);

        void swap(SamplerStateRef& rhs)
        {
            lcore::swap(state_, rhs.state_);
        }

        SamplerStateRef& operator=(const SamplerStateRef& rhs)
        {
            SamplerStateRef(rhs).swap(*this);
            return *this;
        }

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
    };
}
#endif //INC_LGRAPHICS_DX11_SAMPLERSTATEREF_H__
