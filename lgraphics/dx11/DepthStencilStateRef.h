#ifndef INC_LGRAPHICS_DX11_DEPTHSTENCILSTATEREF_H_
#define INC_LGRAPHICS_DX11_DEPTHSTENCILSTATEREF_H_
/**
@file DepthStencilStateRef.h
@author t-sakai
@date 2017/01/15 create
*/
#include "lgraphics.h"
#include "Enumerations.h"

namespace lgfx
{
    //--------------------------------------
    //---
    //--- StencilOPDesc
    //---
    //--------------------------------------
    struct StencilOPDesc
    {
        StencilOp failOp_;
        StencilOp depthFailOp_;
        StencilOp passOp_;
        CmpFunc cmpFunc_;
    };

    //--------------------------------------
    //---
    //--- DepthStencilStateRef
    //---
    //--------------------------------------
    class DepthStencilStateRef
    {
    public:
        typedef ID3D11DepthStencilState element_type;
        typedef ID3D11DepthStencilState* pointer_type;
        typedef pointer_type const* pointer_array_type;

        DepthStencilStateRef();
        DepthStencilStateRef(const DepthStencilStateRef& rhs);
        DepthStencilStateRef(DepthStencilStateRef&& rhs);
        ~DepthStencilStateRef();

        void destroy();

        pointer_type get() { return state_;}
        operator pointer_type(){ return state_;}
        operator pointer_array_type(){ return &state_;}

        bool valid() const;

        DepthStencilStateRef& operator=(const DepthStencilStateRef& rhs);
        DepthStencilStateRef& operator=(DepthStencilStateRef&& rhs);
        void swap(DepthStencilStateRef& rhs);
    private:
        friend class DepthStencilState;

        explicit DepthStencilStateRef(pointer_type state);

        pointer_type state_;
    };

    //--------------------------------------
    //---
    //--- DepthStencilState
    //---
    //--------------------------------------
    class DepthStencilState
    {
    public:
        static DepthStencilStateRef create(
            bool depthEnable,
            DepthWriteMask depthWriteMask,
            CmpFunc depthFunc,
            bool stencilEnable,
            u8 stencilReadMask,
            u8 stencilWriteMask,
            const StencilOPDesc& frontFace,
            const StencilOPDesc& backFace);
    };
}

#endif //INC_LGRAPHICS_DX11_DEPTHSTENCILSTATEREF_H_
