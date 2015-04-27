/**
@file ShaderCompiler.cpp
@author t-sakai
@date 2014/12/18 create
*/
#include "ShaderCompiler.h"
#include <lgraphics/api/ShaderRef.h>
#include <lgraphics/api/InputLayoutRef.h>

namespace lscene
{
    ShaderCompiler::ShaderCompiler()
        :size_(DefaultSize)
    {
        buffer_ = LIME_NEW u8[DefaultSize];
    }

    ShaderCompiler::~ShaderCompiler()
    {
        LIME_DELETE_ARRAY(buffer_);
    }

    // シェーダコンパイル
    bool ShaderCompiler::compileShaderVS(lgraphics::VertexShaderRef& vs, u32 compressedSize, const u8* compressed)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }
        vs = lgraphics::Shader::createVertexShaderFromBinary(buffer_, size);
        return vs.valid();
    }

    // シェーダコンパイル
    bool ShaderCompiler::compileShaderGS(lgraphics::GeometryShaderRef& gs, u32 compressedSize, const u8* compressed)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }
        gs = lgraphics::Shader::createGeometryShaderFromBinary(buffer_, size);
        return gs.valid();
    }

    // シェーダコンパイル
    bool ShaderCompiler::compileShaderGSWithStreamOutput(
        lgraphics::GeometryShaderRef& gs,
        u32 compressedSize,
        const u8* compressed,
        const lgraphics::StreamOutputDeclarationEntry* entries,
        u32 numEntries,
        const u32* strides,
        u32 numStrides)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }

        gs = lgraphics::Shader::createGeometryShaderWithStreamOutputFromBinary(
            buffer_,
            size,
            entries,
            numEntries,
            strides,
            numStrides,
            0);
        return gs.valid();
    }

    // シェーダコンパイル
    bool ShaderCompiler::compileShaderPS(lgraphics::PixelShaderRef& ps, u32 compressedSize, const u8* compressed)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }
        ps = lgraphics::Shader::createPixelShaderFromBinary(buffer_, size);
        return ps.valid();
    }

    // シェーダコンパイル
    bool ShaderCompiler::compileShaderCS(lgraphics::ComputeShaderRef& cs, u32 compressedSize, const u8* compressed)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }
        cs = lgraphics::Shader::createComputeShaderFromBinary(buffer_, size);
        return cs.valid();
    }

    // シェーダコンパイル
    bool ShaderCompiler::compileShaderDS(lgraphics::DomainShaderRef& ds, u32 compressedSize, const u8* compressed)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }
        ds = lgraphics::Shader::createDomainShaderFromBinary(buffer_, size);
        return ds.valid();
    }

    // シェーダコンパイル
    bool ShaderCompiler::compileShaderHS(lgraphics::HullShaderRef& hs, u32 compressedSize, const u8* compressed)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }
        hs = lgraphics::Shader::createHullShaderFromBinary(buffer_, size);
        return hs.valid();
    }

    bool ShaderCompiler::createInputLayout(
        lgraphics::InputLayoutRef& inputLayout,
        D3D11_INPUT_ELEMENT_DESC* elements,
        u32 numElements,
        u32 compressedSize,
        const u8* compressed)
    {
        LASSERT(NULL != compressed);

        u32 size = decompress(compressedSize, compressed);
        if(size<=0){
            return false;
        }
        inputLayout = lgraphics::InputLayout::create(elements, numElements, buffer_, size);
        return inputLayout.valid();
    }

    void ShaderCompiler::resize(u32 size)
    {
        if(size_<size){
            size_ = size;
            LIME_DELETE_ARRAY(buffer_);
            buffer_ = LIME_NEW u8[size_];
        }
    }

    u32 ShaderCompiler::decompress(u32 size, const u8* data)
    {
        lgraphics::ShaderDecompresser decompresser(size, data);
        u32 decompSize = decompresser.getDecompressedSize();
        resize(decompSize);
        return decompresser.decompress(buffer_);
    }
}
