/**
@file ShaderRef.cpp
@author t-sakai
@date 2009/01/26 create
*/
#include <lcore/liostream.h>
#include <lmath/Matrix44.h>
#include "../../lgraphicsAPIInclude.h"
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"

#include "ShaderRef.h"
#include "../ShaderKey.h"

namespace lgraphics
{
    const char* ShaderEntryFunctionName = "main";

    namespace
    {
        void d3dDebugLogError(HRESULT hr)
        {
            const char* message = "";
            switch(hr)
            {
            case D3DERR_INVALIDCALL:
                message = "INVALIDCALL";
                break;

            case D3DERR_OUTOFVIDEOMEMORY:
                message = "D3DERR_OUTOFMEMORY";
                break;

            case E_OUTOFMEMORY:
                message = "E_OUTOFMEMORY";
                break;
            };
            Debug_LogError( message );
        };
    }


    //------------------------------------------------------------
    //---
    //--- Shader
    //---
    //------------------------------------------------------------
    // ファイルからピクセルシェーダ作成
    PixelShaderRef Shader::createPixelShaderFromFile(const char* filename, s32 numDefines, const char** defines)
    {
        LASSERT(filename != NULL);

        ID3DXConstantTable *constantTable = NULL;
        ID3DXBuffer *shaderBuffer = NULL;
        bool ret = compileShaderFromFile(filename, ShaderEntryFunctionName, "ps_3_0", numDefines, defines, &shaderBuffer, &constantTable); //TODO:プロファイルきめうち

        if(!ret){
            return PixelShaderRef();
        }

        IDirect3DPixelShader9 *shader = NULL;

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreatePixelShader(
            (const DWORD*)shaderBuffer->GetBufferPointer(),
            &shader);

        SAFE_RELEASE(shaderBuffer);
        if(FAILED(hr)){
            return PixelShaderRef();
        }

        return PixelShaderRef(shader, constantTable);
    }

    //------------------------------------------------------------
    // メモリからピクセルシェーダ作成
    PixelShaderRef Shader::createPixelShaderFromMemory(const char* memory, u32 size, s32 numDefines, const char** defines)
    {
        LASSERT(memory != NULL);

        ID3DXConstantTable *constantTable = NULL;
        ID3DXBuffer *shaderBuffer = NULL;
        bool ret = compileShaderFromMemory(memory, size, ShaderEntryFunctionName, "ps_3_0", numDefines, defines, &shaderBuffer, &constantTable); //TODO:プロファイルきめうち

        if(!ret){
            return PixelShaderRef();
        }

        IDirect3DPixelShader9 *shader = NULL;

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreatePixelShader(
            (const DWORD*)shaderBuffer->GetBufferPointer(),
            &shader);

        SAFE_RELEASE(shaderBuffer);
        if(FAILED(hr)){
            return PixelShaderRef();
        }

        return PixelShaderRef(shader, constantTable);
    }


    //------------------------------------------------------------
    // ファイルから頂点シェーダ作成
    VertexShaderRef Shader::createVertexShaderFromFile(const char* filename, s32 numDefines, const char** defines)
    {
        LASSERT(filename != NULL);

        ID3DXConstantTable *constantTable = NULL;
        ID3DXBuffer *shaderBuffer = NULL;
        bool ret = compileShaderFromFile(filename, ShaderEntryFunctionName, "vs_3_0", numDefines, defines, &shaderBuffer, &constantTable); //TODO:プロファイルきめうち

        if(!ret){
            return VertexShaderRef();
        }

        IDirect3DVertexShader9 *shader = NULL;

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateVertexShader(
            (const DWORD*)shaderBuffer->GetBufferPointer(),
            &shader);

        SAFE_RELEASE(shaderBuffer);
        if(FAILED(hr)){
            return VertexShaderRef();
        }

        return VertexShaderRef(shader, constantTable);
    }


    //------------------------------------------------------------
    // メモリから頂点シェーダ作成
    VertexShaderRef Shader::createVertexShaderFromMemory(const char* memory, u32 size, s32 numDefines, const char** defines)
    {
        LASSERT(memory != NULL);

        ID3DXConstantTable *constantTable = NULL;
        ID3DXBuffer *shaderBuffer = NULL;
        bool ret = compileShaderFromMemory(memory, size, ShaderEntryFunctionName, "vs_3_0", numDefines, defines, &shaderBuffer, &constantTable); //TODO:プロファイルきめうち

        if(!ret){
            return VertexShaderRef();
        }

        IDirect3DVertexShader9 *shader = NULL;

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateVertexShader(
            (const DWORD*)shaderBuffer->GetBufferPointer(),
            &shader);

        SAFE_RELEASE(shaderBuffer);
        if(FAILED(hr)){
            return VertexShaderRef();
        }

        return VertexShaderRef(shader, constantTable);
    }

    bool Shader::compileShaderFromFile(
        const char* filename,
        const char* entryName,
        const char* profile,
        s32 numDefines,
        const char** defines,
        ID3DXBuffer** shaderBuffer,
        ID3DXConstantTable** constants)
    {
        LASSERT(shaderBuffer != NULL);
        LASSERT(constants != NULL);
        LASSERT(numDefines<=MaxDefines);
        LASSERT(defines != NULL);

        //マクロ配列作成
        D3DXMACRO macro[MaxDefines+1];
        for(s32 i=0; i<numDefines; ++i){
            macro[i].Name = defines[i];
            macro[i].Definition = NULL;
        }
        macro[numDefines].Name = NULL;
        macro[numDefines].Definition = NULL;


        ID3DXBuffer *errorMsg = NULL;
        HRESULT hr;
        hr=D3DXCompileShaderFromFile(
            filename,
            macro,
            NULL,
            entryName,
            profile,
            0,
            shaderBuffer,
            &errorMsg,
            constants);

        if(FAILED(hr)){
            SAFE_RELEASE(*shaderBuffer);
            if(errorMsg){
                const char* message = (const char*)errorMsg->GetBufferPointer();
                Debug_LogError( message );
                SAFE_RELEASE(errorMsg);
            }
            return false;
        }
        return true;
    }

    bool Shader::compileShaderFromMemory(
        const char* memory,
        u32 size,
        const char* entryName,
        const char* profile,
        s32 numDefines,
        const char** defines,
        ID3DXBuffer** shaderBuffer,
        ID3DXConstantTable** constants)
    {
        LASSERT(shaderBuffer != NULL);
        LASSERT(constants != NULL);
        LASSERT(numDefines<=MaxDefines);

        //マクロ配列作成
        D3DXMACRO macro[MaxDefines+1];
        for(s32 i=0; i<numDefines; ++i){
            macro[i].Name = defines[i];
            macro[i].Definition = NULL;
        }
        macro[numDefines].Name = NULL;
        macro[numDefines].Definition = NULL;

        ID3DXBuffer *errorMsg = NULL;
        HRESULT hr;
        hr=D3DXCompileShader(
            memory,
            size,
            macro,
            NULL,
            entryName,
            profile,
            0,
            shaderBuffer,
            &errorMsg,
            constants);

        if(FAILED(hr)){
            SAFE_RELEASE(*shaderBuffer);
            if(errorMsg){
                const char* message = (const char*)errorMsg->GetBufferPointer();
                Debug_LogError( message );
                SAFE_RELEASE(errorMsg);

                {
                    lcore::ofstream out("vs_failed.hlsl", lcore::ios::binary);
                    if(out.is_open()){
                        for(s32 i=0; i<numDefines; ++i){
                            out.print("#define %s\n", macro[i]);
                        }
                        out.write(memory, size);
                        out.close();
                    }
                }
            }
            return false;
        }
        return true;
    }


    //------------------------------------------------------------
    //---
    //--- ShaderRefBase
    //---
    //------------------------------------------------------------
    ShaderRefBase::ShaderRefBase(const ShaderRefBase& rhs)
        :_constantTable(rhs._constantTable)
        ,key_(rhs.key_)
    {
        if(_constantTable != NULL){
            _constantTable->AddRef();
        }
    }

    void ShaderRefBase::destroy()
    {
        SAFE_RELEASE(_constantTable);
    }

#if 0
    void ShaderRefBase::setMatrixByName(const char* name, const lmath::Matrix44& matrix)
    {
        LASSERT(name != NULL);
        LASSERT(_constantTable != NULL);

        D3DXHANDLE handle = _constantTable->GetConstantByName(NULL, name);
        LASSERT(handle != NULL);
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetMatrix(d3ddevice, handle, (const D3DXMATRIX*)&matrix);
    }

    void ShaderRefBase::setFloatByName(const char* name, f32 value)
    {
        LASSERT(name != NULL);
        LASSERT(_constantTable != NULL);

        D3DXHANDLE handle = _constantTable->GetConstantByName(NULL, name);
        LASSERT(handle != NULL);
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetFloat(d3ddevice, handle, value);
    }

    void ShaderRefBase::setVector2ByName(const char* name, const lmath::Vector2& vector)
    {
        LASSERT(name != NULL);
        LASSERT(_constantTable != NULL);

        D3DXHANDLE handle = _constantTable->GetConstantByName(NULL, name);
        LASSERT(handle != NULL);
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetFloatArray(d3ddevice, handle, (const FLOAT*)&vector, 2);
    }

    void ShaderRefBase::setVector3ByName(const char* name, const lmath::Vector3& vector)
    {
        LASSERT(name != NULL);
        LASSERT(_constantTable != NULL);

        D3DXHANDLE handle = _constantTable->GetConstantByName(NULL, name);
        LASSERT(handle != NULL);
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetFloatArray(d3ddevice, handle, (const FLOAT*)&vector, 3);
    }

    void ShaderRefBase::setVector4ByName(const char* name, const lmath::Vector4& vector)
    {
        LASSERT(name != NULL);
        LASSERT(_constantTable != NULL);

        D3DXHANDLE handle = _constantTable->GetConstantByName(NULL, name);
        LASSERT(handle != NULL);
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetVector(d3ddevice, handle, (const D3DXVECTOR4*)&vector);
    }
#endif

    void ShaderRefBase::setMatrix(HANDLE handle, const lmath::Matrix44& matrix)
    {
        LASSERT(handle != NULL);
        LASSERT(_constantTable != NULL);

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetMatrix(d3ddevice, handle, (const D3DXMATRIX*)&matrix);
    }

    void ShaderRefBase::setMatrix(HANDLE handle, const lmath::Matrix34& matrix)
    {
        LASSERT(handle != NULL);
        LASSERT(_constantTable != NULL);

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetFloatArray(d3ddevice, handle, (const FLOAT*)&matrix, 12);
    }

    void ShaderRefBase::setFloat(HANDLE handle, f32 value)
    {
        LASSERT(handle != NULL);
        LASSERT(_constantTable != NULL);

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetFloat(d3ddevice, handle, value);
    }

    void ShaderRefBase::setVector2(HANDLE handle, const lmath::Vector2& vector)
    {
        LASSERT(handle != NULL);
        LASSERT(_constantTable != NULL);

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetFloatArray(d3ddevice, handle, (const FLOAT*)&vector, 2);
    }

    void ShaderRefBase::setVector3(HANDLE handle, const lmath::Vector3& vector)
    {
        //LASSERT(handle != NULL);
        LASSERT(_constantTable != NULL);

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetFloatArray(d3ddevice, handle, (const FLOAT*)&vector, 3);
    }

    void ShaderRefBase::setVector4(HANDLE handle, const lmath::Vector4& vector)
    {
        //LASSERT(handle != NULL);
        LASSERT(_constantTable != NULL);

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetVector(d3ddevice, handle, (const D3DXVECTOR4*)&vector);
    }

    void ShaderRefBase::setVector4Array(HANDLE handle, const lmath::Vector4* vector, u32 count)
    {
        //LASSERT(handle != NULL);
        LASSERT(_constantTable != NULL);

        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        _constantTable->SetVectorArray(d3ddevice, handle, (const D3DXVECTOR4*)vector, count);
    }

    lgraphics::HANDLE ShaderRefBase::getHandle(const char* name)
    {
        LASSERT(_constantTable != NULL);
        LASSERT(name != NULL);

        D3DXHANDLE handle = _constantTable->GetConstantByName(NULL, name);

        //LASSERT(handle != NULL);
        //return static_cast<lgraphics::HANDLE>(handle);
        return handle;
    }

    u32 ShaderRefBase::getSamplerIndex(const char* name)
    {
        LASSERT(name != NULL);
        D3DXHANDLE handle = _constantTable->GetConstantByName(NULL, name);
        if(handle == NULL){
            return INVALID_SAMPLER_INDEX;
        }

        return _constantTable->GetSamplerIndex(handle);
    }


    //------------------------------------------------------------
    //---
    //--- PixelShaderRef
    //---
    //------------------------------------------------------------
    PixelShaderRef::PixelShaderRef(const PixelShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,_shader(rhs._shader)
    {
        if(_shader){
            _shader->AddRef();
        }
    }

    void PixelShaderRef::destroy()
    {
        ShaderRefBase::destroy();
        SAFE_RELEASE(_shader);
    }

    
    void PixelShaderRef::attach() const
    {
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        d3ddevice->SetPixelShader( const_cast<IDirect3DPixelShader9*>(_shader) );
    }


    //------------------------------------------------------------
    //---
    //--- VertexShaderRef
    //---
    //------------------------------------------------------------
    VertexShaderRef::VertexShaderRef(const VertexShaderRef& rhs)
        :ShaderRefBase(rhs)
        ,_shader(rhs._shader)
    {
        if(_shader){
            _shader->AddRef();
        }
    }

    void VertexShaderRef::destroy()
    {
        ShaderRefBase::destroy();
        SAFE_RELEASE(_shader);
    }


    void VertexShaderRef::attach() const
    {
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();
        d3ddevice->SetVertexShader( const_cast<IDirect3DVertexShader9*>(_shader) );
    }
}
