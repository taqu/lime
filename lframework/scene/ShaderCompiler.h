#ifndef INC_LSCENE_SHADERCOMPILER_H__
#define INC_LSCENE_SHADERCOMPILER_H__
/**
@file ShaderCompiler.h
@author t-sakai
@date 2014/12/18 create
*/
#include "lscene.h"

struct D3D11_INPUT_ELEMENT_DESC;

namespace lgraphics
{
    class VertexShaderRef;
    class GeometryShaderRef;
    class PixelShaderRef;
    class ComputeShaderRef;
    class DomainShaderRef;
    class HullShaderRef;

    struct StreamOutputDeclarationEntry;

    class InputLayoutRef;
}

namespace lscene
{
    class ShaderCompiler
    {
    public:
        static const u32 DefaultSize = 2048;

        ShaderCompiler();
        ~ShaderCompiler();

        /// �V�F�[�_�R���p�C��
        bool compileShaderVS(lgraphics::VertexShaderRef& vs, u32 compressedSize, const u8* compressed);

        /// �V�F�[�_�R���p�C��
        bool compileShaderGS(lgraphics::GeometryShaderRef& gs, u32 compressedSize, const u8* compressed);

        /// �V�F�[�_�R���p�C��
        bool compileShaderGSWithStreamOutput(
            lgraphics::GeometryShaderRef& gs,
            u32 compressedSize,
            const u8* compressed,
            const lgraphics::StreamOutputDeclarationEntry* entries,
            u32 numEntries,
            const u32* strides,
            u32 numStrides);

        /// �V�F�[�_�R���p�C��
        bool compileShaderPS(lgraphics::PixelShaderRef& ps, u32 compressedSize, const u8* compressed);

        /// �V�F�[�_�R���p�C��
        bool compileShaderCS(lgraphics::ComputeShaderRef& cs, u32 compressedSize, const u8* compressed);

        /// �V�F�[�_�R���p�C��
        bool compileShaderDS(lgraphics::DomainShaderRef& ds, u32 compressedSize, const u8* compressed);

        /// �V�F�[�_�R���p�C��
        bool compileShaderHS(lgraphics::HullShaderRef& hs, u32 compressedSize, const u8* compressed);

        bool createInputLayout(
            lgraphics::InputLayoutRef& inputLayout,
            D3D11_INPUT_ELEMENT_DESC* elements,
            u32 numElements,
            u32 compressedSize,
            const u8* compressed);

    private:
        ShaderCompiler(const ShaderCompiler&);
        ShaderCompiler& operator=(const ShaderCompiler&);

        void resize(u32 size);
        u32 decompress(u32 size, const u8* data);

        u32 size_;
        u8* buffer_;
    };
}
#endif //INC_LSCENE_SHADERCOMPILER_H__
