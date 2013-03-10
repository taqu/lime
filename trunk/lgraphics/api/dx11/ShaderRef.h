#ifndef INC_LGRAPHICS_DX11_SHADERREF_H__
#define INC_LGRAPHICS_DX11_SHADERREF_H__
/**
@file ShaderRef.h
@author t-sakai
@date 2012/07/14 create
*/
#include "../../lgraphicscore.h"

struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;

namespace lgraphics
{
    class BlobRef;

    extern const char* ShaderEntryFunctionName; //="main";

    //------------------------------------------------------------
    //---
    //--- ShaderRefBase
    //---
    //------------------------------------------------------------
    class ShaderRefBase
    {
    };


    //------------------------------------------------------------
    //---
    //--- PixelShaderRef
    //---
    //------------------------------------------------------------
    class PixelShaderRef : public ShaderRefBase
    {
    public:
        PixelShaderRef()
            :shader_(NULL)
        {
        }

        PixelShaderRef(const PixelShaderRef& rhs);

        ~PixelShaderRef()
        {
            destroy();
        }

        PixelShaderRef& operator=(const PixelShaderRef& rhs)
        {
            PixelShaderRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        bool valid() const{ return (shader_!=NULL);}

        void attach() const;

        void swap(PixelShaderRef& rhs)
        {
            lcore::swap(shader_, rhs.shader_);
        }

    private:
        friend class Shader;

        void destroy();

        PixelShaderRef(ID3D11PixelShader *shader)
            :shader_(shader)
        {
        }

        ID3D11PixelShader* shader_;
    };


    //------------------------------------------------------------
    //---
    //--- VertexShaderRef
    //---
    //------------------------------------------------------------
    class VertexShaderRef : public ShaderRefBase
    {
    public:
         VertexShaderRef()
            :shader_(NULL)
        {
        }

        VertexShaderRef(const VertexShaderRef& rhs);

        ~VertexShaderRef()
        {
            destroy();
        }

        VertexShaderRef& operator=(const VertexShaderRef& rhs)
        {
            VertexShaderRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        bool valid() const{ return (shader_!=NULL);}

        void attach() const;

        void swap(VertexShaderRef& rhs)
        {
            lcore::swap(shader_, rhs.shader_);
        }

    private:
        friend class Shader;

        void destroy();

        VertexShaderRef(ID3D11VertexShader *shader)
            :shader_(shader)
        {
        }

        ID3D11VertexShader* shader_;
    };


    //------------------------------------------------------------
    //---
    //--- GeometryShaderRef
    //---
    //------------------------------------------------------------
    class GeometryShaderRef : public ShaderRefBase
    {
    public:
         GeometryShaderRef()
            :shader_(NULL)
        {
        }

        GeometryShaderRef(const GeometryShaderRef& rhs);

        ~GeometryShaderRef()
        {
            destroy();
        }

        GeometryShaderRef& operator=(const GeometryShaderRef& rhs)
        {
            GeometryShaderRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        bool valid() const{ return (shader_!=NULL);}

        void attach() const;

        void swap(GeometryShaderRef& rhs)
        {
            lcore::swap(shader_, rhs.shader_);
        }

    private:
        friend class Shader;

        void destroy();

        GeometryShaderRef(ID3D11GeometryShader *shader)
            :shader_(shader)
        {
        }

        ID3D11GeometryShader* shader_;
    };

    //------------------------------------------------------------
    //---
    //--- Shader
    //---
    //------------------------------------------------------------
    class Shader
    {
    public:
        static const s32 MaxDefines = 63; /// define�̍ő吔

        /**
        @brief �t�@�C������s�N�Z���V�F�[�_�쐬
        @param filename ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static PixelShaderRef createPixelShaderFromFile(const Char* filename, s32 numDefines, const char** defines, BlobRef* blob);

        /**
        @brief ����������s�N�Z���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static PixelShaderRef createPixelShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, BlobRef* blob);

        /**
        @brief �t�@�C�����璸�_�V�F�[�_�쐬
        @param filename ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static VertexShaderRef createVertexShaderFromFile(const Char* filename, s32 numDefines, const char** defines, BlobRef* blob);

        /**
        @brief ���������璸�_�V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static VertexShaderRef createVertexShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, BlobRef* blob);

        /**
        @brief �t�@�C������W�I���g���V�F�[�_�쐬
        @param filename ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static GeometryShaderRef createGeometryShaderFromFile(const Char* filename, s32 numDefines, const char** defines, BlobRef* blob);

        /**
        @brief ����������W�I���g���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        @param numDefines ... define�}�N���̐�
        @param defines ... define�}�N����
        */
        static GeometryShaderRef createGeometryShaderFromMemory(const Char* memory, u32 size, s32 numDefines, const char** defines, BlobRef* blob);



        /**
        @brief ����������s�N�Z���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        */
        static PixelShaderRef createPixelShaderFromBinary(const u8* memory, u32 size);

        /**
        @brief ���������璸�_�V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        */
        static VertexShaderRef createVertexShaderFromBinary(const u8* memory, u32 size);

        /**
        @brief ����������W�I���g���V�F�[�_�쐬
        @param memory ... 
        @param size ... 
        */
        static GeometryShaderRef createGeometryShaderFromBinary(const u8* memory, u32 size);
    };


    //------------------------------------------------------------
    //---
    //--- ShaderCompresser
    //---
    //------------------------------------------------------------
    class ShaderCompresser
    {
    public:
        ShaderCompresser();
        ~ShaderCompresser();

        bool compress(u32 codeLength, const u8* byteCode);

        u32 getSize() const{ return compressedSize_;}
        const u8* getData() const{ return buffer_;}
    private:
        u32 compressedSize_;
        u8* buffer_;
    };

    //------------------------------------------------------------
    //---
    //--- ShaderDecompresser
    //---
    //------------------------------------------------------------
    class ShaderDecompresser
    {
    public:
        ShaderDecompresser(u32 compressedSize, const u8* compressed);
        ~ShaderDecompresser();

        u32 decompress(u8* dst);

        u32 getDecompressedSize() const;
    private:
        u32 compressedSize_;
        const u8* compressed_;
    };
}
#endif //INC_LGRAPHICS_DX11_SHADERREF_H__