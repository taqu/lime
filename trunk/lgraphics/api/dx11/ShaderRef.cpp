/**
@file ShaderRef.cpp
@author t-sakai
@date 2012/07/14 create
*/
#include "ShaderRef.h"

#include "../../lgraphicsAPIInclude.h"
#include <D3Dcompiler.h>

#include <zlib.h>

#include <lcore/liostream.h>
#include <lcore/utility.h>
#include <lmath/Matrix44.h>
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

#include "ShaderRef.h"
#include "BlobRef.h"

namespace lgraphics
{
namespace
{
    bool compileShader(
        const Char* memory,
        u32 size,
        const Char* filename,
        const Char* entryName,
        const Char* profile,
        s32 numDefines,
        const Char** defines,
        ID3DBlob** blob)
    {
        LASSERT(blob != NULL);
        LASSERT(numDefines<=Shader::MaxDefines);

        //�}�N���z��쐬
        D3D10_SHADER_MACRO macro[Shader::MaxDefines+1];
        for(s32 i=0; i<numDefines; ++i){
            macro[i].Name = defines[i];
            macro[i].Definition = NULL;
        }
        macro[numDefines].Name = NULL;
        macro[numDefines].Definition = NULL;


        ID3DBlob *errorMsg = NULL;
        HRESULT hr;
        hr=D3DCompile(
            memory,
            size,
            filename,
            macro,
            NULL,
            entryName,
            profile,
            0,
            0,
            blob,
            &errorMsg);

        if(FAILED(hr)){
            SAFE_RELEASE(*blob);
            if(errorMsg){
                const char* message = (const char*)errorMsg->GetBufferPointer();
                lcore::Log( message );
                errorMsg->Release();
                errorMsg = NULL;
            }
            return false;
        }
        SAFE_RELEASE(errorMsg);
        return true;
    }

}

    const char* ShaderEntryFunctionName = "main";


    //------------------------------------------------------------
    //---
    //--- Shader
    //---
    //------------------------------------------------------------
    // �t�@�C������s�N�Z���V�F�[�_�쐬
    PixelShaderRef Shader::createPixelShaderFromFile(const Char* filename, s32 numDefines, const char** defines, BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::binary);
        if(!in.is_open()){
            return PixelShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LIME_NEW Char[size]);
        in.read(buff.get(), size);
        in.close();
        
        return createPixelShaderFromMemory(buff.get(), size, numDefines, defines, blob);
    }

    //------------------------------------------------------------
    // ����������s�N�Z���V�F�[�_�쐬
    PixelShaderRef Shader::createPixelShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::PSModel, numDefines, defines, &d3dBlob);

        if(!ret){
            return PixelShaderRef();
        }

        ID3D11PixelShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreatePixelShader(
            d3dBlob->GetBufferPointer(),
            d3dBlob->GetBufferSize(),
            NULL,
            &shader);

        if(NULL == blob){
            SAFE_RELEASE(d3dBlob);
        }else{
            *blob = BlobRef(d3dBlob);
        }
        if(FAILED(hr)){
            return PixelShaderRef();
        }

        return PixelShaderRef(shader);
    }


    //------------------------------------------------------------
    // �t�@�C�����璸�_�V�F�[�_�쐬
    VertexShaderRef Shader::createVertexShaderFromFile(const Char* filename, s32 numDefines, const char** defines, BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::binary);
        if(!in.is_open()){
            return VertexShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LIME_NEW Char[size]);
        in.read(buff.get(), size);
        in.close();
        
        return createVertexShaderFromMemory(buff.get(), size, numDefines, defines, blob);
    }


    //------------------------------------------------------------
    // ���������璸�_�V�F�[�_�쐬
    VertexShaderRef Shader::createVertexShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::VSModel, numDefines, defines, &d3dBlob);

        if(!ret){
            return VertexShaderRef();
        }

        ID3D11VertexShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateVertexShader(
            d3dBlob->GetBufferPointer(),
            d3dBlob->GetBufferSize(),
            NULL,
            &shader);

        if(NULL == blob){
            SAFE_RELEASE(d3dBlob);
        }else{
            *blob = BlobRef(d3dBlob);
        }

        if(FAILED(hr)){
            return VertexShaderRef();
        }

        return VertexShaderRef(shader);
    }


    //------------------------------------------------------------
    // �t�@�C������W�I���g���V�F�[�_�쐬
    GeometryShaderRef Shader::createGeometryShaderFromFile(const Char* filename, s32 numDefines, const char** defines, BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::binary);
        if(!in.is_open()){
            return GeometryShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LIME_NEW Char[size]);
        in.read(buff.get(), size);
        in.close();
        
        return createGeometryShaderFromMemory(buff.get(), size, numDefines, defines, blob);
    }


    //------------------------------------------------------------
    // ����������W�I���g���V�F�[�_�쐬
    GeometryShaderRef Shader::createGeometryShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::GSModel, numDefines, defines, &d3dBlob);

        if(!ret){
            return GeometryShaderRef();
        }

        ID3D11GeometryShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateGeometryShader(
            d3dBlob->GetBufferPointer(),
            d3dBlob->GetBufferSize(),
            NULL,
            &shader);

        if(NULL == blob){
            SAFE_RELEASE(d3dBlob);
        }else{
            *blob = BlobRef(d3dBlob);
        }

        if(FAILED(hr)){
            return GeometryShaderRef();
        }

        return GeometryShaderRef(shader);
    }



    //------------------------------------------------------------
    // ����������s�N�Z���V�F�[�_�쐬
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
    // ���������璸�_�V�F�[�_�쐬
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
    // ����������W�I���g���V�F�[�_�쐬
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

        //�G���f�B�A���Ⴂ�͖���
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

        //�G���f�B�A���Ⴂ�͖���
        const u32* header = reinterpret_cast<const u32*>(compressed_);
        return *header;
    }

    u32 ShaderDecompresser::decompress(u8* dst)
    {
        LASSERT(NULL != dst);

        uLongf size = getDecompressedSize();
        if(Z_OK != uncompress(dst, &size, compressed_+sizeof(u32), compressedSize_)){
            return 0;
        }
        return size;
    }
}