#ifndef INC_LGRAPHICS_DX11_SHADERCOMPILER_H_
#define INC_LGRAPHICS_DX11_SHADERCOMPILER_H_
/**
@file ShaderCompiler.h
@author t-sakai
@date 2013/12/30 create
*/
#include <D3Dcompiler.h>
#include <lcore/lcore.h>
#include <lcore/File.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/ShaderRef.h>
#include <lgraphics/BlobRef.h>

namespace lgfx
{
    class ShaderInculde;

    //------------------------------------------------------------
    //---
    //--- ShaderCompiler
    //---
    //------------------------------------------------------------
    template<int T>
    class ShaderCompiler
    {
    public:
        static const s32 MaxDefines = 63; /// define�̍ő吔

        static bool compileShader(
            const Char* memory,
            u32 size,
            const Char* filename,
            const Char* entryName,
            const Char* profile,
            s32 numDefines,
            const Char** defines,
            ShaderInculde* shaderInclude,
            ID3DBlob** blob);

        static bool compileShader(
            const Char* memory,
            u32 size,
            const Char* filename,
            const Char* entryName,
            const Char* profile,
            s32 numDefines,
            const Char** defines,
            ShaderInculde* shaderInclude,
            ID3DBlob** blob,
            ID3DBlob** error);

        /**
        @brief �t�@�C������s�N�Z���V�F�[�_�쐬
        @param filename ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static PixelShaderRef createPixelShaderFromFile(const Char* filename, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief ����������s�N�Z���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static PixelShaderRef createPixelShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief �t�@�C�����璸�_�V�F�[�_�쐬
        @param filename ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static VertexShaderRef createVertexShaderFromFile(const Char* filename, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief ���������璸�_�V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static VertexShaderRef createVertexShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief �t�@�C������W�I���g���V�F�[�_�쐬
        @param filename ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static GeometryShaderRef createGeometryShaderFromFile(const Char* filename, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief ����������W�I���g���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static GeometryShaderRef createGeometryShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief �t�@�C������R���s���[�g�V�F�[�_�쐬
        @param filename ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static ComputeShaderRef createComputeShaderFromFile(const Char* filename, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief ����������R���s���[�g�V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static ComputeShaderRef createComputeShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief �t�@�C������h���C���V�F�[�_�쐬
        @param filename ...
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static DomainShaderRef createDomainShaderFromFile(const Char* filename, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief ����������h���C���V�F�[�_�쐬
        @param memory ...
        @param size ...
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static DomainShaderRef createDomainShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief �t�@�C������n���V�F�[�_�쐬
        @param filename ...
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static HullShaderRef createHullShaderFromFile(const Char* filename, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief ����������n���V�F�[�_�쐬
        @param memory ...
        @param size ...
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static HullShaderRef createHullShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* blob);

        /**
        @brief ����������s�N�Z���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static BlobRef createPixelShaderBlobFromMemory(const Char* memory, u32 size, const Char* profile, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* error);

        /**
        @brief ���������璸�_�V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static BlobRef createVertexShaderBlobFromMemory(const Char* memory, u32 size, const Char* profile, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* error);

        /**
        @brief ����������W�I���g���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static BlobRef createGeometryShaderBlobFromMemory(const Char* memory, u32 size, const Char* profile, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* error);

        /**
        @brief ����������R���s���[�g�V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static BlobRef createComputeShaderBlobFromMemory(const Char* memory, u32 size, const Char* profile, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* error);

        /**
        @brief ����������h���C���V�F�[�_�쐬
        @param memory ...
        @param size ...
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static BlobRef createDomainShaderBlobFromMemory(const Char* memory, u32 size, const Char* profile, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* error);

        /**
        @brief ����������n���V�F�[�_�쐬
        @param memory ...
        @param size ...
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static BlobRef createHullShaderBlobFromMemory(const Char* memory, u32 size, const Char* profile, s32 numDefines, const char** defines, ShaderInculde* shaderInclude, BlobRef* error);
    };

    template<int T>
    bool ShaderCompiler<T>::compileShader(
        const Char* memory,
        u32 size,
        const Char* filename,
        const Char* entryName,
        const Char* profile,
        s32 numDefines,
        const Char** defines,
        ShaderInculde* shaderInclude,
        ID3DBlob** blob)
    {
        LASSERT(blob != NULL);
        LASSERT(numDefines<=MaxDefines);

        //�}�N���z��쐬
        D3D10_SHADER_MACRO macro[MaxDefines+1];
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
            shaderInclude,
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


    template<int T>
    bool ShaderCompiler<T>::compileShader(
        const Char* memory,
        u32 size,
        const Char* filename,
        const Char* entryName,
        const Char* profile,
        s32 numDefines,
        const Char** defines,
        ShaderInculde* shaderInclude,
        ID3DBlob** blob,
        ID3DBlob** error)
    {
        LASSERT(blob != NULL);
        LASSERT(numDefines<=MaxDefines);

        //�}�N���z��쐬
        D3D10_SHADER_MACRO macro[MaxDefines+1];
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
            shaderInclude,
            entryName,
            profile,
            0,
            0,
            blob,
            &errorMsg);

        if(FAILED(hr)){
            LDXSAFE_RELEASE(*blob);
            if(errorMsg && (NULL != *error)){
                *error = errorMsg;
            }else{
                LDXSAFE_RELEASE(errorMsg);
            }

            return false;
        }
        LDXSAFE_RELEASE(errorMsg);
        return true;
    }

    //------------------------------------------------------------
    // �t�@�C������s�N�Z���V�F�[�_�쐬
    template<int T>
    PixelShaderRef ShaderCompiler<T>::createPixelShaderFromFile(
        const Char* filename,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::in);
        if(!in.is_open()){
            return PixelShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LNEW Char[size]);
        in.read(buff.get(), size);
        in.close();
        
        return createPixelShaderFromMemory(buff.get(), size, numDefines, defines, shaderInclude, blob);
    }

    //------------------------------------------------------------
    // ����������s�N�Z���V�F�[�_�쐬
    template<int T>
    PixelShaderRef ShaderCompiler<T>::createPixelShaderFromMemory(
        const Char* memory,
        u32 size,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::PSModel, numDefines, defines, shaderInclude, &d3dBlob);

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
    template<int T>
    VertexShaderRef ShaderCompiler<T>::createVertexShaderFromFile(
        const Char* filename,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::in);
        if(!in.is_open()){
            return VertexShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LNEW Char[size]);
        in.read(buff.get(), size);
        in.close();
        
        return createVertexShaderFromMemory(buff.get(), size, numDefines, defines, shaderInclude, blob);
    }


    //------------------------------------------------------------
    // ���������璸�_�V�F�[�_�쐬
    template<int T>
    VertexShaderRef ShaderCompiler<T>::createVertexShaderFromMemory(
        const Char* memory,
        u32 size,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::VSModel, numDefines, defines, shaderInclude, &d3dBlob);

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
    template<int T>
    GeometryShaderRef ShaderCompiler<T>::createGeometryShaderFromFile(
        const Char* filename,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::in);
        if(!in.is_open()){
            return GeometryShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LNEW Char[size]);
        in.read(buff.get(), size);
        in.close();
        
        return createGeometryShaderFromMemory(buff.get(), size, numDefines, defines, shaderInclude, blob);
    }


    //------------------------------------------------------------
    // ����������W�I���g���V�F�[�_�쐬
    template<int T>
    GeometryShaderRef ShaderCompiler<T>::createGeometryShaderFromMemory(
        const Char* memory,
        u32 size,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::GSModel, numDefines, defines, shaderInclude, &d3dBlob);

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
    // �t�@�C������R���s���[�g�V�F�[�_�쐬
    template<int T>
    ComputeShaderRef ShaderCompiler<T>::createComputeShaderFromFile(
        const Char* filename,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::in);
        if(!in.is_open()){
            return ComputeShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LNEW Char[size]);
        in.read(buff.get(), size);
        in.close();
        
        return createComputeShaderFromMemory(buff.get(), size, numDefines, defines, shaderInclude, blob);
    }


    //------------------------------------------------------------
    // ����������R���s���[�g�V�F�[�_�쐬
    template<int T>
    ComputeShaderRef ShaderCompiler<T>::createComputeShaderFromMemory(
        const Char* memory,
        u32 size,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::CSModel, numDefines, defines, shaderInclude, &d3dBlob);

        if(!ret){
            return ComputeShaderRef();
        }

        ID3D11ComputeShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateComputeShader(
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
            return ComputeShaderRef();
        }

        return ComputeShaderRef(shader);
    }

    //------------------------------------------------------------
    // �t�@�C������h���C���V�F�[�_�쐬
    template<int T>
    DomainShaderRef ShaderCompiler<T>::createDomainShaderFromFile(
        const Char* filename,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::in);
        if(!in.is_open()){
            return DomainShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LNEW Char[size]);
        in.read(buff.get(), size);
        in.close();

        return createDomainShaderFromMemory(buff.get(), size, numDefines, defines, shaderInclude, blob);
    }


    //------------------------------------------------------------
    // ����������h���C���V�F�[�_�쐬
    template<int T>
    DomainShaderRef ShaderCompiler<T>::createDomainShaderFromMemory(
        const Char* memory,
        u32 size,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::DSModel, numDefines, defines, shaderInclude, &d3dBlob);

        if(!ret){
            return DomainShaderRef();
        }

        ID3D11DomainShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateDomainShader(
            d3dBlob->GetBufferPointer(),
            d3dBlob->GetBufferSize(),
            NULL,
            &shader);

        if(NULL == blob){
            SAFE_RELEASE(d3dBlob);
        } else{
            *blob = BlobRef(d3dBlob);
        }

        if(FAILED(hr)){
            return DomainShaderRef();
        }

        return DomainShaderRef(shader);
    }

    //------------------------------------------------------------
    // �t�@�C������n���V�F�[�_�쐬
    template<int T>
    HullShaderRef ShaderCompiler<T>::createHullShaderFromFile(
        const Char* filename,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(filename != NULL);
        lcore::ifstream in(filename, lcore::ios::in);
        if(!in.is_open()){
            return HullShaderRef();
        }

        u32 size = in.getSize(0);

        lcore::ScopedArrayPtr<Char> buff(LNEW Char[size]);
        in.read(buff.get(), size);
        in.close();

        return createHullShaderFromMemory(buff.get(), size, numDefines, defines, shaderInclude, blob);
    }


    //------------------------------------------------------------
    // ����������n���V�F�[�_�쐬
    template<int T>
    HullShaderRef ShaderCompiler<T>::createHullShaderFromMemory(
        const Char* memory,
        u32 size,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* blob)
    {
        LASSERT(memory != NULL);

        ID3DBlob *d3dBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, GraphicsDeviceRef::HSModel, numDefines, defines, shaderInclude, &d3dBlob);

        if(!ret){
            return HullShaderRef();
        }

        ID3D11HullShader *shader = NULL;

        ID3D11Device *d3ddevice = Graphics::getDevice().getD3DDevice();

        HRESULT hr = d3ddevice->CreateHullShader(
            d3dBlob->GetBufferPointer(),
            d3dBlob->GetBufferSize(),
            NULL,
            &shader);

        if(NULL == blob){
            SAFE_RELEASE(d3dBlob);
        } else{
            *blob = BlobRef(d3dBlob);
        }

        if(FAILED(hr)){
            return HullShaderRef();
        }

        return HullShaderRef(shader);
    }

    //------------------------------------------------------------
    // ����������s�N�Z���V�F�[�_�쐬
    template<int T>
    BlobRef ShaderCompiler<T>::createPixelShaderBlobFromMemory(
        const Char* memory,
        u32 size,
        const Char* profile,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* error)
    {
        LASSERT(memory != NULL);

        ID3DBlob* d3dBlob = NULL;
        ID3DBlob* errorBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, profile, numDefines, defines, shaderInclude, &d3dBlob, &errorBlob);
        if(NULL != error){
            *error = BlobRef(errorBlob);
        }

        return (ret)? BlobRef(d3dBlob) : BlobRef();
    }

    // ���������璸�_�V�F�[�_�쐬
    template<int T>
    BlobRef ShaderCompiler<T>::createVertexShaderBlobFromMemory(
        const Char* memory,
        u32 size,
        const Char* profile,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* error)
    {
        LASSERT(memory != NULL);

        ID3DBlob* d3dBlob = NULL;
        ID3DBlob* errorBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, profile, numDefines, defines, shaderInclude, &d3dBlob, &errorBlob);
        if(NULL != error){
            *error = BlobRef(errorBlob);
        }

        return (ret)? BlobRef(d3dBlob) : BlobRef();
    }

    // ����������W�I���g���V�F�[�_�쐬
    template<int T>
    BlobRef ShaderCompiler<T>::createGeometryShaderBlobFromMemory(
        const Char* memory,
        u32 size,
        const Char* profile,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* error)
    {
        LASSERT(memory != NULL);

        ID3DBlob* d3dBlob = NULL;
        ID3DBlob* errorBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, profile, numDefines, defines, shaderInclude, &d3dBlob, &errorBlob);
        if(NULL != error){
            *error = BlobRef(errorBlob);
        }

        return (ret)? BlobRef(d3dBlob) : BlobRef();
    }

    // ����������R���s���[�g�V�F�[�_�쐬
    template<int T>
    BlobRef ShaderCompiler<T>::createComputeShaderBlobFromMemory(
        const Char* memory,
        u32 size,
        const Char* profile,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* error)
    {
        LASSERT(memory != NULL);

        ID3DBlob* d3dBlob = NULL;
        ID3DBlob* errorBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, profile, numDefines, defines, shaderInclude, &d3dBlob, &errorBlob);
        if(NULL != error){
            *error = BlobRef(errorBlob);
        }

        return (ret)? BlobRef(d3dBlob) : BlobRef();
    }

    // ����������h���C���V�F�[�_�쐬
    template<int T>
    BlobRef ShaderCompiler<T>::createDomainShaderBlobFromMemory(
        const Char* memory,
        u32 size,
        const Char* profile,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* error)
    {
        LASSERT(memory != NULL);

        ID3DBlob* d3dBlob = NULL;
        ID3DBlob* errorBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, profile, numDefines, defines, shaderInclude, &d3dBlob, &errorBlob);
        if(NULL != error){
            *error = BlobRef(errorBlob);
        }

        return (ret)? BlobRef(d3dBlob) : BlobRef();
    }

    // ����������n���V�F�[�_�쐬
    template<int T>
    BlobRef ShaderCompiler<T>::createHullShaderBlobFromMemory(
        const Char* memory,
        u32 size,
        const Char* profile,
        s32 numDefines,
        const char** defines,
        ShaderInculde* shaderInclude,
        BlobRef* error)
    {
        LASSERT(memory != NULL);

        ID3DBlob* d3dBlob = NULL;
        ID3DBlob* errorBlob = NULL;
        bool ret = compileShader(memory, size, NULL, ShaderEntryFunctionName, profile, numDefines, defines, shaderInclude, &d3dBlob, &errorBlob);
        if(NULL != error){
            *error = BlobRef(errorBlob);
        }

        return (ret)? BlobRef(d3dBlob) : BlobRef();
    }

    class ShaderInculde : public ID3DInclude
    {
    public:
        ShaderInculde(){}
        ~ShaderInculde(){}

        HRESULT __stdcall Open(D3D_INCLUDE_TYPE type, LPCSTR filename, LPCVOID parentData, LPCVOID* data, UINT* bytes);
        HRESULT __stdcall Close(LPCVOID data);
    };
}
#endif //INC_LGRAPHICS_DX11_SHADERCOMPILER_H_
