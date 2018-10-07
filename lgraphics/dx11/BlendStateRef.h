#ifndef INC_LGRAPHICS_DX11_BLENDSTATEREF_H_
#define INC_LGRAPHICS_DX11_BLENDSTATEREF_H_
/**
@file BlendStateRef.h
@author t-sakai
@date 2012/07/24 create
*/
#include "../lgraphics.h"
#include "Enumerations.h"

struct ID3D11BlendState;

namespace lgfx
{
    class ContextRef;
    typedef D3D11_BLEND_DESC BlendStateDesc;

    //------------------------------------------------------------
    //---
    //--- BlendStateRef
    //---
    //------------------------------------------------------------
    class BlendStateRef
    {
    public:
        typedef ID3D11BlendState element_type;
        typedef ID3D11BlendState* pointer_type;
        typedef pointer_type const* pointer_array_type;

        BlendStateRef()
            :state_(NULL)
        {}

        BlendStateRef(const BlendStateRef& rhs);
        BlendStateRef(BlendStateRef&& rhs);

        ~BlendStateRef()
        {
            destroy();
        }

        void destroy();

        pointer_type get(){ return state_;}
        operator pointer_type(){ return state_; }
        operator pointer_array_type(){ return &state_;}

        bool valid() const{ return (NULL != state_);}

        void attach(ContextRef& context);

        BlendStateRef& operator=(const BlendStateRef& rhs);
        BlendStateRef& operator=(BlendStateRef&& rhs);

        void swap(BlendStateRef& rhs)
        {
            lcore::swap(state_, rhs.state_);
        }

        bool getDesc(BlendStateDesc& desc);
    private:
        friend class BlendState;

        explicit BlendStateRef(pointer_type state)
            :state_(state)
        {}

        pointer_type state_;
    };

    //------------------------------------------------------------
    //---
    //--- BlendState
    //---
    //------------------------------------------------------------
    class BlendState
    {
    public:
        static ID3D11BlendState* createRaw(
            s32 alphaToCoverage,
            s32 blendEnable,
            BlendType src,
            BlendType dst,
            BlendOp op,
            BlendType srcAlpha,
            BlendType dstAlpha,
            BlendOp opAlpha,
            u8 writeMask);

        inline static BlendStateRef create(
            s32 alphaToCoverage,
            s32 blendEnable,
            BlendType src,
            BlendType dst,
            BlendOp op,
            BlendType srcAlpha,
            BlendType dstAlpha,
            BlendOp opAlpha,
            u8 writeMask)
        {
            ID3D11BlendState* state = createRaw(
                alphaToCoverage,
                blendEnable,
                src,
                dst,
                op,
                srcAlpha,
                dstAlpha,
                opAlpha,
                writeMask);
            
            return BlendStateRef(state);
        }

        static BlendStateRef createIndipendent(
            s32 alphaToCoverage,
            const s32* blendEnable,
            const BlendType* src,
            const BlendType* dst,
            const BlendOp* op,
            const BlendType* srcAlpha,
            const BlendType* dstAlpha,
            const BlendOp* opAlpha,
            const u8* writeMask);
    };
}
#endif //INC_LGRAPHICS_DX11_BLENDSTATEREF_H_
