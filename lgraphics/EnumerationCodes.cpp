/**
@file EnumerationCodes.cpp
@author t-sakai
@date 2010/06/15 create

*/
#include "../lgraphicscore.h"
#include "EnumerationCodes.h"

namespace lgraphics
{
#define CASE_ENTRY(name) case name: return Code##name; break
#define CASE_BREAK(name) case name: break

    //-------------------------------------------------------
    //---
    //--- Pool
    //---
    //-------------------------------------------------------
    CodePool encodePool(Pool type)
    {
#if defined(LIME_DX9)
        switch(type)
        {
            CASE_ENTRY(Pool_Default);
            CASE_ENTRY(Pool_Managed);
            CASE_ENTRY(Pool_SystemMem);
            CASE_ENTRY(Pool_Scratch);
            CASE_ENTRY(Pool_UserMem);
        };
#else
        switch(type)
        {
            CASE_ENTRY(Pool_Default);
            CASE_ENTRY(Pool_SystemMem);
            CASE_BREAK(Pool_FORCEDWORD);
        };
#endif
        LASSERT(false);
        return CodePool_Default;
    }

    Pool decodePool(CodePool type)
    {
        static Pool decodeTable[]=
        {
            Pool_Default,
            Pool_Managed,
            Pool_SystemMem,
            Pool_Scratch,
            Pool_UserMem,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- Resource
    //---
    //-------------------------------------------------------
    CodeResourceType encodeResource(ResourceType type)
    {
        switch(type)
        {
            CASE_ENTRY(Resource_Surface);
            CASE_ENTRY(Resource_Volume);
            CASE_ENTRY(Resource_Texture);
            CASE_ENTRY(Resource_VolumeTexture);
            CASE_ENTRY(Resource_CubeTexture);
            CASE_ENTRY(Resource_VertexBuffer);
            CASE_ENTRY(Resource_IndexBuffer);
            CASE_BREAK(Resource_FORCEDWORD);
        };
        LASSERT(false);
        return CodeResource_Surface;
    }


    ResourceType decodeResource(CodeResourceType type)
    {
        static ResourceType decodeTable[]=
        {
            Resource_Surface,
            Resource_Volume,
            Resource_Texture,
            Resource_VolumeTexture,
            Resource_CubeTexture,
            Resource_VertexBuffer,
            Resource_IndexBuffer,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- Usage
    //---
    //-------------------------------------------------------
    CodeUsage encodeUsage(Usage type)
    {
        switch(type)
        {
            CASE_ENTRY(Usage_None);
            CASE_ENTRY(Usage_AutoGenMipMap);
            CASE_ENTRY(Usage_DepthStenceil);
            CASE_ENTRY(Usage_Displacement);
            CASE_ENTRY(Usage_DoNotClip);
            CASE_ENTRY(Usage_Dynamic);
            CASE_ENTRY(Usage_RenderTarget);
        };
        LASSERT(false);
        return CodeUsage_None;
    }

    Usage decodeUsage(CodeUsage type)
    {
        static Usage decodeTable[]=
        {
            Usage_None,
            Usage_AutoGenMipMap,
            Usage_DepthStenceil,
            Usage_Displacement,
            Usage_DoNotClip,
            Usage_Dynamic,
            Usage_RenderTarget,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- Lock
    //---
    //-------------------------------------------------------
    CodeLock encodeLock(Lock type)
    {
        switch(type)
        {
            CASE_ENTRY(Lock_None);
            CASE_ENTRY(Lock_Discard);
            CASE_ENTRY(Lock_NoOverwrite);
        };
        LASSERT(false);
        return CodeLock_None;
    }

    Lock decodeLock(CodeLock type)
    {
        static Lock decodeTable[]=
        {
            Lock_None,
            Lock_Discard,
            Lock_NoOverwrite,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- DeclType
    //---
    //-------------------------------------------------------
    CodeDeclType encodeDeclType(DeclType type)
    {
        switch(type)
        {
            CASE_ENTRY(DeclType_Float1);
            CASE_ENTRY(DeclType_Float2);
            CASE_ENTRY(DeclType_Float3);
            CASE_ENTRY(DeclType_Float4);
            CASE_ENTRY(DeclType_Color);
            CASE_ENTRY(DeclType_UB4);
            CASE_ENTRY(DeclType_Short2);
            CASE_ENTRY(DeclType_Short4);
            CASE_ENTRY(DeclType_UB4N);
            CASE_ENTRY(DeclType_Short2N);
            CASE_ENTRY(DeclType_Short4N);
            CASE_ENTRY(DeclType_UShort2N);
            CASE_ENTRY(DeclType_UShort4N);
            CASE_ENTRY(DeclType_UDec3N);
            CASE_ENTRY(DeclType_Dec3N);
            CASE_ENTRY(DeclType_Float16_2);
            CASE_ENTRY(DeclType_Float16_4);
            CASE_ENTRY(DeclType_UnUsed);
        };
        LASSERT(false);
        return CodeDeclType_UnUsed;
    }

    DeclType decodeDeclType(CodeDeclType type)
    {
        static DeclType decodeTable[]=
        {
            DeclType_Float1,
            DeclType_Float2,
            DeclType_Float3,
            DeclType_Float4,
            DeclType_Color,
            DeclType_UB4,
            DeclType_Short2,
            DeclType_Short4,
            DeclType_UB4N,
            DeclType_Short2N,
            DeclType_Short4N,
            DeclType_UShort2N,
            DeclType_UShort4N,
            DeclType_UDec3N,
            DeclType_Dec3N,
            DeclType_Float16_2,
            DeclType_Float16_4,
            DeclType_UnUsed,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- DeclUsage
    //---
    //-------------------------------------------------------
    CodeDeclUsage encodeDeclUsage(DeclUsage type)
    {
        switch(type)
        {
            CASE_ENTRY(DeclUsage_Position);
            CASE_ENTRY(DeclUsage_BlendWeight);
            CASE_ENTRY(DeclUsage_BlendIndicies);
            CASE_ENTRY(DeclUsage_Normal);
            CASE_ENTRY(DeclUsage_PSize);
            CASE_ENTRY(DeclUsage_Texcoord);
            CASE_ENTRY(DeclUsage_Tangent);
            CASE_ENTRY(DeclUsage_Binormal);
            CASE_ENTRY(DeclUsage_TessFactor);
            CASE_ENTRY(DeclUsage_PositionT);
            CASE_ENTRY(DeclUsage_Color);
            CASE_ENTRY(DeclUsage_Fog);
            CASE_ENTRY(DeclUsage_Depth);
            CASE_ENTRY(DeclUsage_Sample);
            CASE_BREAK(DeclUsage_Num);
        };
        LASSERT(false);
        return CodeDeclUsage_Position;
    }

    DeclUsage decodeDeclUsage(CodeDeclUsage type)
    {
        static DeclUsage decodeTable[]=
        {
            DeclUsage_Position,
            DeclUsage_BlendWeight,
            DeclUsage_BlendIndicies,
            DeclUsage_Normal,
            DeclUsage_PSize,
            DeclUsage_Texcoord,
            DeclUsage_Tangent,
            DeclUsage_Binormal,
            DeclUsage_TessFactor,
            DeclUsage_PositionT,
            DeclUsage_Color,
            DeclUsage_Fog,
            DeclUsage_Depth,
            DeclUsage_Sample,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- PrimitiveType
    //---
    //-------------------------------------------------------
    CodePrimitiveType encodePrimitive(PrimitiveType type)
    {
        switch(type)
        {
            CASE_ENTRY(Primitive_PointList);
            CASE_ENTRY(Primitive_LineList);
            CASE_ENTRY(Primitive_LineStrip);
            CASE_ENTRY(Primitive_TriangleList);
            CASE_ENTRY(Primitive_TriangleStrip);
            CASE_ENTRY(Primitive_TriangleFan);
        };
        LASSERT(false);
        return CodePrimitive_TriangleList;
    }

    PrimitiveType decodePrimitive(CodePrimitiveType type)
    {
        static PrimitiveType decodeTable[]=
        {
            //Primitive_PointList,
            Primitive_LineList,
            Primitive_LineStrip,
            Primitive_TriangleList,
            Primitive_TriangleStrip,
            Primitive_TriangleFan,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- BufferFormat
    //---
    //-------------------------------------------------------
    CodeBufferFormat encodeBufferFormat(BufferFormat type)
    {
        switch(type)
        {
            CASE_ENTRY(Buffer_Unknown);
            CASE_ENTRY(Buffer_D32);
            CASE_ENTRY(Buffer_D24S8);
            CASE_ENTRY(Buffer_D16);
            CASE_ENTRY(Buffer_Index16);
            CASE_ENTRY(Buffer_Index32);
            CASE_ENTRY(Buffer_Vertex);

            CASE_ENTRY(Buffer_DXT1);
            CASE_ENTRY(Buffer_DXT2);
            CASE_ENTRY(Buffer_DXT3);
            CASE_ENTRY(Buffer_DXT4);
            CASE_ENTRY(Buffer_DXT5);

            CASE_ENTRY(Buffer_A16B16G16R16F);

            CASE_ENTRY(Buffer_R32F);
            CASE_ENTRY(Buffer_G32R32F);
            CASE_ENTRY(Buffer_A32B32G32R32F);

            CASE_ENTRY(Buffer_A8);
            CASE_ENTRY(Buffer_R8G8B8);
#if defined(LIME_GL)
            CASE_ENTRY(Buffer_B8G8R8);
#endif
            CASE_ENTRY(Buffer_A8R8G8B8);
            CASE_ENTRY(Buffer_X8R8G8B8);
            CASE_ENTRY(Buffer_A8B8G8R8);
            CASE_ENTRY(Buffer_X8B8G8R8);
            CASE_ENTRY(Buffer_A16B16G16R16);
            CASE_ENTRY(Buffer_A2R10G10B10);
            CASE_ENTRY(Buffer_A2B10G10R10);
            CASE_ENTRY(Buffer_G16R16);
            CASE_ENTRY(Buffer_L8);
            CASE_ENTRY(Buffer_L16);

            CASE_BREAK(Buffer_FORCEDWORD);
        };
        LASSERT(false);
        return CodeBuffer_Unknown;
    }

    BufferFormat decodeBufferFormat(CodeBufferFormat type)
    {
        static BufferFormat decodeTable[]=
        {
            Buffer_Unknown,
            Buffer_D32,
            Buffer_D24S8,
            Buffer_Index16,
            Buffer_Index32,
            Buffer_Vertex,

            Buffer_DXT1,
            Buffer_DXT2,
            Buffer_DXT3,
            Buffer_DXT4,
            Buffer_DXT5,

            Buffer_R32F,
            Buffer_G32R32F,
            Buffer_A32B32G32R32F,

            Buffer_A8,
            Buffer_R8G8B8,
            Buffer_A8R8G8B8,
            Buffer_X8R8G8B8,
            Buffer_A8B8G8R8,
            Buffer_X8B8G8R8,
            Buffer_A16B16G16R16,
            Buffer_A2R10G10B10,
            Buffer_A2B10G10R10,
            Buffer_G16R16,
            Buffer_L8,
            Buffer_L16,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- MutiSampleType
    //---
    //-------------------------------------------------------
    CodeMutiSampleType encodeMutiSample(MutiSampleType type)
    {
        switch(type)
        {
            CASE_ENTRY(MutiSample_None);
            CASE_ENTRY(MutiSample_NonMaskable);
            CASE_ENTRY(MutiSample_2);
            CASE_ENTRY(MutiSample_3);
            CASE_ENTRY(MutiSample_4);
            CASE_ENTRY(MutiSample_5);

            CASE_ENTRY(MutiSample_6);
            CASE_ENTRY(MutiSample_7);
            CASE_ENTRY(MutiSample_8);
            CASE_ENTRY(MutiSample_9);
            CASE_ENTRY(MutiSample_10);

            CASE_ENTRY(MutiSample_11);
            CASE_ENTRY(MutiSample_12);
            CASE_ENTRY(MutiSample_13);

            CASE_ENTRY(MutiSample_14);
            CASE_ENTRY(MutiSample_15);
            CASE_ENTRY(MutiSample_16);

            CASE_BREAK(MutiSample_FORCEDWORD);
        };
        LASSERT(false);
        return CodeMutiSample_None;
    }

    MutiSampleType decodeMutiSample(CodeMutiSampleType type)
    {
        static MutiSampleType decodeTable[]=
        {
            MutiSample_None,
            MutiSample_NonMaskable,
            MutiSample_2,
            MutiSample_3,
            MutiSample_4,
            MutiSample_5,
            MutiSample_6,
            MutiSample_7,
            MutiSample_8,
            MutiSample_9,
            MutiSample_10,
            MutiSample_11,
            MutiSample_12,
            MutiSample_13,
            MutiSample_14,
            MutiSample_15,
            MutiSample_16,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- TextureAddress
    //---
    //-------------------------------------------------------
    CodeTextureAddress encodeTextureAddress(TextureAddress type)
    {
#if defined(LIME_DX9)
        switch(type)
        {
            CASE_ENTRY(TexAddress_Wrap);
            CASE_ENTRY(TexAddress_Mirror);
            CASE_ENTRY(TexAddress_Clamp);
            CASE_ENTRY(TexAddress_Border);
            CASE_ENTRY(TexAddress_Mirroronce);
            CASE_ENTRY(TexAddress_None);
        };
#else
        switch(type)
        {
            CASE_ENTRY(TexAddress_Wrap);
            CASE_ENTRY(TexAddress_Mirror);
            CASE_ENTRY(TexAddress_Clamp);
        };
#endif
        LASSERT(false);
        return CodeTexAddress_None;
    }

    TextureAddress decodeTextureAddress(CodeTextureAddress type)
    {
        static TextureAddress decodeTable[]=
        {
            TexAddress_Wrap,
            TexAddress_Mirror,
            TexAddress_Clamp,
            TexAddress_Border,
            TexAddress_Mirroronce,
            TexAddress_None,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- TextureFilterType
    //---
    //-------------------------------------------------------
    CodeTextureFilterType encodeTextureFilter(TextureFilterType type)
    {
#if defined(LIME_DX9)
        switch(type)
        {
            CASE_ENTRY(TexFilter_None);
            CASE_ENTRY(TexFilter_Point);
            CASE_ENTRY(TexFilter_Linear);
            CASE_ENTRY(TexFilter_Anisotropic);
            CASE_ENTRY(TexFilter_PyramidalQuad);
            CASE_ENTRY(TexFilter_GaussianQuad);
            CASE_ENTRY(TexFilter_ConvolutionMono);
        };

#else
        switch(type)
        {
            CASE_ENTRY(TexFilter_Point);
            CASE_ENTRY(TexFilter_Linear);

            CASE_BREAK(TexFilter_Num);
        };
#endif

        LASSERT(false);
        return CodeTexFilter_None;
    }

    TextureFilterType decodeTextureFilter(CodeTextureFilterType type)
    {
#if defined(LIME_DX9)
        static TextureFilterType decodeTable[]=
        {
            TexFilter_None,
            TexFilter_Point,
            TexFilter_Linear,
            TexFilter_Anisotropic,
            TexFilter_PyramidalQuad,
            TexFilter_GaussianQuad,
            TexFilter_ConvolutionMono,
        };

#else
        static TextureFilterType decodeTable[]=
        {
            TexFilter_None,
            TexFilter_Point,
            TexFilter_Linear,
            TexFilter_Linear,
            TexFilter_Linear,
            TexFilter_Linear,
            TexFilter_Linear,
        };
#endif
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- CullMode
    //---
    //-------------------------------------------------------
    CodeCullMode encodeCullMode(CullMode type)
    {
        switch(type)
        {
            CASE_ENTRY(CullMode_None);
            CASE_ENTRY(CullMode_CW);
            CASE_ENTRY(CullMode_CCW);
        };
        LASSERT(false);
        return CodeCullMode_None;
    }

    CullMode decodeCullMode(CodeCullMode type)
    {
        static CullMode decodeTable[]=
        {
            CullMode_None,
            CullMode_CW,
            CullMode_CCW,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- BlendType
    //---
    //-------------------------------------------------------
    CodeBlendType encodeBlendType(BlendType type)
    {
        switch(type)
        {
            CASE_ENTRY(Blend_Zero);
            CASE_ENTRY(Blend_One);
            CASE_ENTRY(Blend_SrcAlpha);
            CASE_ENTRY(Blend_InvSrcAlpha);
            CASE_ENTRY(Blend_DestAlpha);
            CASE_ENTRY(Blend_InvDestAlpha);
        };
        LASSERT(false);
        return CodeBlend_One;
    }

    BlendType decodeBlendType(CodeBlendType type)
    {
        static BlendType decodeTable[]=
        {
            Blend_Zero,
            Blend_One,
            Blend_SrcAlpha,
            Blend_InvSrcAlpha,
            Blend_DestAlpha,
            Blend_InvDestAlpha,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

    //-------------------------------------------------------
    //---
    //--- CmpFunc
    //---
    //-------------------------------------------------------
    CodeCmpFunc encodeCmpFunc(CmpFunc type)
    {
        switch(type)
        {
            CASE_ENTRY(Cmp_Never);
            CASE_ENTRY(Cmp_Less);
            CASE_ENTRY(Cmp_Equal);
            CASE_ENTRY(Cmp_LessEqual);
            CASE_ENTRY(Cmp_Greater);
            CASE_ENTRY(Cmp_NotEqual);
            CASE_ENTRY(Cmp_GreaterEqual);
            CASE_ENTRY(Cmp_Always);
        };
        LASSERT(false);
        return CodeCmp_Less;
    }

    CmpFunc decodeCmpFunc(CodeCmpFunc type)
    {
        static CmpFunc decodeTable[]=
        {
            Cmp_Never,
            Cmp_Less,
            Cmp_Equal,
            Cmp_LessEqual,
            Cmp_Greater,
            Cmp_NotEqual,
            Cmp_GreaterEqual,
            Cmp_Always,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }


    //-------------------------------------------------------
    //---
    //--- VertexBlend
    //---
    //-------------------------------------------------------
    CodeVertexBlend encodeVertexBlend(VertexBlend type)
    {
        switch(type)
        {
            CASE_ENTRY(VertexBlend_Disable);
            CASE_ENTRY(VertexBlend_1WEIGHTS);
            CASE_ENTRY(VertexBlend_2WEIGHTS);
            CASE_ENTRY(VertexBlend_3WEIGHTS);
        };
        LASSERT(false);
        return CodeVertexBlend_Disable;
    }

    VertexBlend decodeVertexBlend(CodeVertexBlend type)
    {
        static VertexBlend decodeTable[]=
        {
            VertexBlend_Disable,
            VertexBlend_1WEIGHTS,
            VertexBlend_2WEIGHTS,
            VertexBlend_3WEIGHTS,
        };
        LASSERT(0<=type && type<(sizeof(decodeTable)/sizeof(decodeTable[0])));
        return decodeTable[type];
    }

#undef CASE_ENTRY
#undef CASE_BREAK
}
