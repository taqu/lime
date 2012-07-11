/**
@file IORenderState.cpp
@author t-sakai
@date 2010/05/06 create

*/
#include "IORenderState.h"

#include "SectionID.h"

#include "../api/RenderStateRef.h"
#include "../api/EnumerationCodes.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        enum BitFlag
        {
            BitFlag_AlphaTest = 0x01U,
            //BitFlag_Lighting =  0x02U,
            BitFlag_MultiSampleAlias =  0x02U,
            BitFlag_ZEnable =  0x04U,
            BitFlag_ZWriteEnable =  0x08U,
            BitFlag_AlphaBlendEnable = 0x10U,

            BitFlag_Num = 6,
        };

        struct RenderStateInfo
        {
            u32 id_;
            u32 flag_;
            u32 alphaTestFunc_;
            //s32 alphaTestRef_;
            u32 cullMode_;
            u32 alphaBlendSrc_;
            u32 alphaBlendDst_;
        };

        inline bool test(u32 flag, BitFlag bit)
        {
            return ((flag & bit) != 0);
        }
    }


    bool IORenderState::read(lcore::istream& is, RenderStateRef& state)
    {
        RenderStateInfo info;
        lcore::io::read(is, info);

        state.setAlphaTest(        test(info.flag_, BitFlag_AlphaTest) );
        //RenderStateRef::setLighting(         test(info.flag_, BitFlag_Lighting) );
        state.setMultiSampleAlias( test(info.flag_, BitFlag_MultiSampleAlias) );
        state.setZEnable(          test(info.flag_, BitFlag_ZEnable) );
        state.setZWriteEnable(     test(info.flag_, BitFlag_ZWriteEnable) );
        state.setAlphaBlendEnable( test(info.flag_, BitFlag_AlphaBlendEnable) );

        state.setCullMode( decodeCullMode(static_cast<CodeCullMode>( info.cullMode_ )) );

#if defined(LIME_GLES1)
        RenderStateRef::setAlphaTestFunc( decodeCmpFunc(static_cast<CodeCmpFunc>( info.alphaTestFunc_ )), info.alphaTestRef_);
        RenderStateRef::setAlphaBlend( decodeBlendType(static_cast<CodeBlendType>( info.alphaBlendSrc_ )), decodeBlendType(static_cast<CodeBlendType>( info.alphaBlendDst_ )));

#else
        state.setAlphaTestFunc( decodeCmpFunc(static_cast<CodeCmpFunc>( info.alphaTestFunc_ )) );
        //state.setAlphaTestRef( info.alphaTestRef_ );

        BlendType blendSrc = decodeBlendType(static_cast<CodeBlendType>( info.alphaBlendSrc_ ));
        BlendType blendDst = decodeBlendType(static_cast<CodeBlendType>( info.alphaBlendDst_ ));
        state.setAlphaBlend(blendSrc, blendDst);
#endif

        return true;
    }

    bool IORenderState::write(lcore::ostream& os, RenderStateRef& state)
    {
        state.apply();

        u32 bitFlag = 0;
        bitFlag |= (state.getAlphaTest())? BitFlag_AlphaTest : 0;
        //bitFlag |= (state.getLighting())? BitFlag_Lighting : 0;
        bitFlag |= (state.getMultiSampleAlias())? BitFlag_MultiSampleAlias : 0;
        bitFlag |= (state.getZEnable())? BitFlag_ZEnable : 0;
        bitFlag |= (state.getZWriteEnable())? BitFlag_ZWriteEnable : 0;
        bitFlag |= (state.getAlphaBlendEnable())? BitFlag_AlphaBlendEnable : 0;

        RenderStateInfo info;

        info.id_ = format::RenderStateID;
        info.flag_ = bitFlag;

        info.alphaTestFunc_ = static_cast<u32>( encodeCmpFunc( state.getAlphaTestFunc() ) );
        //info.alphaTestRef_ = state.getAlphaTestRef();

        info.cullMode_ = static_cast<u32>( encodeCullMode( state.getCullMode() ) );
        info.alphaBlendSrc_ = static_cast<u32>( encodeBlendType( state.getAlphaBlendSrc() ) );
        info.alphaBlendDst_ = static_cast<u32>( encodeBlendType( state.getAlphaBlendDst() ) );

        lcore::io::write(os, info);

        return true;
    }
}
}
