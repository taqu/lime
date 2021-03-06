#ifndef INC_LGRAPHICS_DX11_RASTERIZERSTATEREF_H_
#define INC_LGRAPHICS_DX11_RASTERIZERSTATEREF_H_
/**
@file RasterizerStateRef.h
@author t-sakai
@date 2017/01/15 create
*/
#include "lgraphics.h"
#include "Enumerations.h"

namespace lgfx
{
    //--------------------------------------
    //---
    //--- RasterizerStateRef
    //---
    //--------------------------------------
    class RasterizerStateRef
    {
    public:
        typedef ID3D11RasterizerState element_type;
        typedef ID3D11RasterizerState* pointer_type;
        typedef pointer_type const* pointer_array_type;

        RasterizerStateRef()
            :state_(NULL)
        {}
        RasterizerStateRef(const RasterizerStateRef& rhs);
        RasterizerStateRef(RasterizerStateRef&& rhs);
        ~RasterizerStateRef()
        {
            destroy();
        }

        void destroy();

        pointer_type get() { return state_;}
        operator pointer_type(){ return state_;}
        operator pointer_array_type(){ return &state_;}

        bool valid() const{return NULL != state_;}

        RasterizerStateRef& operator=(const RasterizerStateRef& rhs);
        RasterizerStateRef& operator=(RasterizerStateRef&& rhs);
        void swap(RasterizerStateRef& rhs);
    private:
        friend class RasterizerState;

        explicit RasterizerStateRef(pointer_type state);

        pointer_type state_;
    };

    //--------------------------------------
    //---
    //--- RasterizerState
    //---
    //--------------------------------------
    class RasterizerState
    {
    public:
        static RasterizerStateRef create(
            FillMode fillMode,
            CullMode cullMode,
            bool frontCounterClockwise,
            s32 depthBias,
            f32 depthBiasClamp,
            f32 slopeScaledDepthBias,
            bool depthClipEnable,
            bool scissorEnable,
            bool multisampleEnable,
            bool antialiasedLineEnable);
    };
}
#endif //INC_LGRAPHICS_DX11_RASTERIZERSTATEREF_H_
