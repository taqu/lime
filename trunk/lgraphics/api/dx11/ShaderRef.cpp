/**
@file ShaderRef.cpp
@author t-sakai
@date 2012/07/14 create
*/
#include "ShaderRef.h"

#include <zlib.h>

#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

namespace lgraphics
{
    const char* ShaderEntryFunctionName = "main";


    //------------------------------------------------------------
    //---
    //--- Shader
    //---
    //------------------------------------------------------------

    //------------------------------------------------------------
    // メモリからピクセルシェーダ作成
    PixelShaderRef Shader::createPixelShaderFromBinary(const u8* memory, u32 size)
    {
        LASSERT(memory != NULL);

        ID3D11PixelShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreatePixelShader(
            memory,
            size,
            NULL,
            &shader);

        if(FAILED(hr)){
            return PixelShaderRef();
        }

        return PixelShaderRef(shader);
    }

    //------------------------------------------------------------
    // メモリから頂点シェーダ作成
    VertexShaderRef Shader::createVertexShaderFromBinary(const u8* memory, u32 size)
    {
        LASSERT(memory != NULL);

        ID3D11VertexShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateVertexShader(
            memory,
            size,
            NULL,
            &shader);

        if(FAILED(hr)){
            return VertexShaderRef();
        }

        return VertexShaderRef(shader);
    }

    //------------------------------------------------------------
    // メモリからジオメトリシェーダ作成
    GeometryShaderRef Shader::createGeometryShaderFromBinary(const u8* memory, u32 size)
    {
        LASSERT(memory != NULL);

        ID3D11GeometryShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateGeometryShader(
            memory,
            size,
            NULL,
            &shader);

        if(FAILED(hr)){
            return GeometryShaderRef();
        }

        return GeometryShaderRef(shader);
    }


    //------------------------------------------------------------
    // メモリからジオメトリシェーダ作成。StreamOutput用
    GeometryShaderRef Shader::createGeometryShaderWithStreamOutputFromBinary(
        const u8* memory,
        u32 size,
        const StreamOutputDeclarationEntry* entries,
        u32 numEntries,
        const u32* bufferStrides,
        u32 numStrides,
        u32 rasterizedStream)
    {

        LASSERT(memory != NULL);
        LASSERT(0<=numEntries && numEntries<= D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT);

        D3D11_SO_DECLARATION_ENTRY soEntries[D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT];
        for(u32 i=0; i<numEntries; ++i){
            soEntries[i].Stream = entries[i].stream_;
            soEntries[i].SemanticName = entries[i].semanticName_;
            soEntries[i].SemanticIndex = entries[i].semanticIndex_;
            soEntries[i].StartComponent = entries[i].startComponent_;
            soEntries[i].ComponentCount = entries[i].componentCount_;
            soEntries[i].OutputSlot = entries[i].outputSlot_;
        }

        ID3D11GeometryShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateGeometryShaderWithStreamOutput(
            memory,
            size,
            soEntries,
            numEntries,
            bufferStrides,
            numStrides,
            rasterizedStream,
            NULL,
            &shader);

        if(FAILED(hr)){
            return GeometryShaderRef();
        }

        return GeometryShaderRef(shader);
    }


    //------------------------------------------------------------
    // メモリからコンピュートシェーダ作成
    ComputeShaderRef Shader::createComputeShaderFromBinary(const u8* memory, u32 size)
    {
        LASSERT(memory != NULL);

        ID3D11ComputeShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateComputeShader(
            memory,
            size,
            NULL,
            &shader);

        if(FAILED(hr)){
            return ComputeShaderRef();
        }

        return ComputeShaderRef(shader);
    }

    //------------------------------------------------------------
    //---
    //--- PixelShaderRef
    //---
    //------------------------------------------------------------
    PixelShaderRef::PixelShaderRef(const PixelShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,shader_(rhs.shader_)
    {
        if(shader_){
            shader_->AddRef();
        }
    }

    void PixelShaderRef::destroy()
    {
        SAFE_RELEASE(shader_);
    }

    
    void PixelShaderRef::attach() const
    {
        Graphics::getDevice().setPixelShader(shader_);
    }


    //------------------------------------------------------------
    //---
    //--- VertexShaderRef
    //---
    //------------------------------------------------------------
    VertexShaderRef::VertexShaderRef(const VertexShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,shader_(rhs.shader_)
    {
        if(shader_){
            shader_->AddRef();
        }
    }

    void VertexShaderRef::destroy()
    {
        SAFE_RELEASE(shader_);
    }


    void VertexShaderRef::attach() const
    {
        Graphics::getDevice().setVertexShader(shader_);
    }


    //------------------------------------------------------------
    //---
    //--- GeometryShaderRef
    //---
    //------------------------------------------------------------
    GeometryShaderRef::GeometryShaderRef(const GeometryShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,shader_(rhs.shader_)
    {
        if(shader_){
            shader_->AddRef();
        }
    }

    void GeometryShaderRef::destroy()
    {
        SAFE_RELEASE(shader_);
    }


    void GeometryShaderRef::attach() const
    {
        Graphics::getDevice().setGeometryShader(shader_);
    }

    //------------------------------------------------------------
    //---
    //--- ComputeShaderRef
    //---
    //------------------------------------------------------------
    ComputeShaderRef::ComputeShaderRef(const ComputeShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,shader_(rhs.shader_)
    {
        if(shader_){
            shader_->AddRef();
        }
    }

    void ComputeShaderRef::destroy()
    {
        SAFE_RELEASE(shader_);
    }


    void ComputeShaderRef::attach() const
    {
        Graphics::getDevice().setComputeShader(shader_);
    }

    //------------------------------------------------------------
    //---
    //--- ShaderCompresser
    //---
    //------------------------------------------------------------
    ShaderCompresser::ShaderCompresser()
        :compressedSize_(0)
        ,buffer_(NULL)
    {
    }

    ShaderCompresser::~ShaderCompresser()
    {
        LIME_DELETE_ARRAY(buffer_);
    }


    bool ShaderCompresser::compress(u32 codeLength, const u8* byteCode)
    {
        LASSERT(NULL != byteCode);

        u32 compressedSize = compressBound(codeLength);

        u8* buffer = LIME_NEW u8[sizeof(u32) + compressedSize];

        //エンディアン違いは無視
        u32* header = reinterpret_cast<u32*>(buffer);
        header[0] = compressedSize;

        uLongf size = compressedSize;
        if(Z_OK != compress2(buffer+sizeof(u32), &size, byteCode, codeLength, Z_BEST_COMPRESSION)){
            LIME_DELETE_ARRAY(buffer);
            return false;
        }
        LIME_DELETE_ARRAY(buffer_);

        compressedSize_ = size + sizeof(u32);
        buffer_ = buffer;
        return true;
    }


    //------------------------------------------------------------
    //---
    //--- ShaderDecompresser
    //---
    //------------------------------------------------------------
    ShaderDecompresser::ShaderDecompresser(u32 compressedSize, const u8* compressed)
        :compressedSize_(compressedSize)
        ,compressed_(compressed)
    {
    }

    ShaderDecompresser::~ShaderDecompresser()
    {
    }


    u32 ShaderDecompresser::getDecompressedSize() const
    {
        LASSERT(compressed_ != NULL);

        //エンディアン違いは無視
        const u32* header = reinterpret_cast<const u32*>(compressed_);
        return *header;
    }

    u32 ShaderDecompresser::decompress(u8* dst)
    {
        LASSERT(NULL != dst);

        uLongf size = getDecompressedSize();
        if(Z_OK != uncompress(dst, &size, compressed_+sizeof(u32), compressedSize_-sizeof(u32))){
            return 0;
        }
        return size;
    }
}
