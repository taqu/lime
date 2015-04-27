#ifndef INC_LGRAPHICS_DX11_BLENDSTATEREF_H__
#define INC_LGRAPHICS_DX11_BLENDSTATEREF_H__
/**
@file BlendStateRef.h
@author t-sakai
@date 2012/07/24 create
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"

struct ID3D11BlendState;

namespace lgraphics
{
    class ContextRef;

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

        BlendStateRef()
            :state_(NULL)
        {}

        BlendStateRef(const BlendStateRef& rhs);

        ~BlendStateRef()
        {
            destroy();
        }

        void destroy();

        BlendStateRef& operator=(const BlendStateRef& rhs)
        {
            BlendStateRef(rhs).swap(*this);
            return *this;
        }

        pointer_type getBlendState(){ return state_;}
        pointer_type const* get(){ return &state_;}
        bool valid() const{ return (NULL != state_);}

        void attach(ContextRef& context);

        void swap(BlendStateRef& rhs)
        {
            lcore::swap(state_, rhs.state_);
        }

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
#endif //INC_LGRAPHICS_DX11_BLENDSTATEREF_H__
