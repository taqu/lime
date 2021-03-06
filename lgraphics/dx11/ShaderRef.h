﻿#ifndef INC_LGRAPHICS_DX11_SHADERREF_H_
#define INC_LGRAPHICS_DX11_SHADERREF_H_
/**
@file ShaderRef.h
@author t-sakai
@date 2012/07/14 create
*/
#include "../lgraphics.h"

struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11ComputeShader;
struct ID3D11DomainShader;
struct ID3D11HullShader;

namespace lgfx
{
    class BlobRef;
    class ContextRef;

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
        typedef ID3D11PixelShader element_type;
        typedef ID3D11PixelShader* pointer_type;
        typedef pointer_type const* pointer_array_type;

        PixelShaderRef()
            :shader_(NULL)
        {}

        PixelShaderRef(const PixelShaderRef& rhs);
        PixelShaderRef(PixelShaderRef&& rhs);

        ~PixelShaderRef()
        {
            destroy();
        }

        pointer_type get(){ return shader_;}
        operator pointer_type(){ return shader_; }
        operator pointer_array_type(){ return &shader_; }

        bool valid() const{ return (shader_!=NULL);}

        void attach(ContextRef& context) const;

        PixelShaderRef& operator=(const PixelShaderRef& rhs);
        PixelShaderRef& operator=(PixelShaderRef&& rhs);

        void swap(PixelShaderRef& rhs);
    private:
        friend class Shader;
        template<int T> friend class ShaderCompiler;

        void destroy();

        explicit PixelShaderRef(pointer_type shader)
            :shader_(shader)
        {
        }

        pointer_type shader_;
    };


    //------------------------------------------------------------
    //---
    //--- VertexShaderRef
    //---
    //------------------------------------------------------------
    class VertexShaderRef : public ShaderRefBase
    {
    public:
        typedef ID3D11VertexShader element_type;
        typedef ID3D11VertexShader* pointer_type;
        typedef pointer_type const* pointer_array_type;

         VertexShaderRef()
            :shader_(NULL)
        {}

        VertexShaderRef(const VertexShaderRef& rhs);
        VertexShaderRef(VertexShaderRef&& rhs);


        ~VertexShaderRef()
        {
            destroy();
        }

        pointer_type get(){ return shader_;}
        operator pointer_type(){ return shader_; }
        operator pointer_array_type(){ return &shader_; }

        bool valid() const{ return (shader_!=NULL);}

        void attach(ContextRef& context) const;

        VertexShaderRef& operator=(const VertexShaderRef& rhs);
        VertexShaderRef& operator=(VertexShaderRef&& rhs);

        void swap(VertexShaderRef& rhs);
    private:
        friend class Shader;
        template<int T> friend class ShaderCompiler;

        void destroy();

        explicit VertexShaderRef(pointer_type shader)
            :shader_(shader)
        {
        }

        pointer_type shader_;
    };


    //------------------------------------------------------------
    //---
    //--- GeometryShaderRef
    //---
    //------------------------------------------------------------
    class GeometryShaderRef : public ShaderRefBase
    {
    public:
        typedef ID3D11GeometryShader element_type;
        typedef ID3D11GeometryShader* pointer_type;
        typedef pointer_type const* pointer_array_type;

        GeometryShaderRef()
            :shader_(NULL)
        {}

        GeometryShaderRef(const GeometryShaderRef& rhs);
        GeometryShaderRef(GeometryShaderRef&& rhs);

        ~GeometryShaderRef()
        {
            destroy();
        }

        pointer_type get(){ return shader_;}
        operator pointer_type(){ return shader_; }
        operator pointer_array_type(){ return &shader_; }

        bool valid() const{ return (shader_!=NULL);}

        void attach(ContextRef& context) const;

        GeometryShaderRef& operator=(const GeometryShaderRef& rhs);
        GeometryShaderRef& operator=(GeometryShaderRef&& rhs);

        void swap(GeometryShaderRef& rhs);
    private:
        friend class Shader;
        template<int T> friend class ShaderCompiler;

        void destroy();

        explicit GeometryShaderRef(pointer_type shader)
            :shader_(shader)
        {
        }

        pointer_type shader_;
    };

    //------------------------------------------------------------
    //---
    //--- ComputeShaderRef
    //---
    //------------------------------------------------------------
    class ComputeShaderRef : public ShaderRefBase
    {
    public:
        typedef ID3D11ComputeShader element_type;
        typedef ID3D11ComputeShader* pointer_type;
        typedef pointer_type const* pointer_array_type;

        ComputeShaderRef()
            :shader_(NULL)
        {}

        ComputeShaderRef(const ComputeShaderRef& rhs);
        ComputeShaderRef(ComputeShaderRef&& rhs);

        ~ComputeShaderRef()
        {
            destroy();
        }

        pointer_type get(){ return shader_;}
        operator pointer_type(){ return shader_; }
        operator pointer_array_type(){ return &shader_; }

        bool valid() const{ return (shader_!=NULL);}

        void attach(ContextRef& context) const;


        ComputeShaderRef& operator=(const ComputeShaderRef& rhs);
        ComputeShaderRef& operator=(ComputeShaderRef&& rhs);

        void swap(ComputeShaderRef& rhs);
    private:
        friend class Shader;
        template<int T> friend class ShaderCompiler;

        void destroy();

        explicit ComputeShaderRef(pointer_type shader)
            :shader_(shader)
        {
        }

        pointer_type shader_;
    };


    //------------------------------------------------------------
    //---
    //--- DomainShaderRef
    //---
    //------------------------------------------------------------
    class DomainShaderRef : public ShaderRefBase
    {
    public:
        typedef ID3D11DomainShader element_type;
        typedef ID3D11DomainShader* pointer_type;
        typedef pointer_type const* pointer_array_type;

        DomainShaderRef()
            :shader_(NULL)
        {}

        DomainShaderRef(const DomainShaderRef& rhs);
        DomainShaderRef(DomainShaderRef&& rhs);

        ~DomainShaderRef()
        {
            destroy();
        }

        pointer_type get(){ return shader_; }
        operator pointer_type(){ return shader_; }
        operator pointer_array_type(){ return &shader_; }

        bool valid() const{ return (shader_!=NULL); }

        void attach(ContextRef& context) const;

        DomainShaderRef& operator=(const DomainShaderRef& rhs);
        DomainShaderRef& operator=(DomainShaderRef&& rhs);

        void swap(DomainShaderRef& rhs);
    private:
        friend class Shader;
        template<int T> friend class ShaderCompiler;

        void destroy();

        explicit DomainShaderRef(pointer_type shader)
            :shader_(shader)
        {
        }

        pointer_type shader_;
    };


    //------------------------------------------------------------
    //---
    //--- HullShaderRef
    //---
    //------------------------------------------------------------
    class HullShaderRef : public ShaderRefBase
    {
    public:
        typedef ID3D11HullShader element_type;
        typedef ID3D11HullShader* pointer_type;
        typedef pointer_type const* pointer_array_type;

        HullShaderRef()
            :shader_(NULL)
        {}

        HullShaderRef(const HullShaderRef& rhs);
        HullShaderRef(HullShaderRef&& rhs);

        ~HullShaderRef()
        {
            destroy();
        }

        pointer_type get(){ return shader_; }
        operator pointer_type(){ return shader_; }
        operator pointer_array_type(){ return &shader_; }

        bool valid() const{ return (shader_!=NULL); }

        void attach(ContextRef& context) const;

        HullShaderRef& operator=(const HullShaderRef& rhs);
        HullShaderRef& operator=(HullShaderRef&& rhs); 

        void swap(HullShaderRef& rhs);
    private:
        friend class Shader;
        template<int T> friend class ShaderCompiler;

        void destroy();

        explicit HullShaderRef(pointer_type shader)
            :shader_(shader)
        {
        }

        pointer_type shader_;
    };


    struct StreamOutputDeclarationEntry
    {
        u32 stream_;
        const Char* semanticName_;
        u32 semanticIndex_;
        u8 startComponent_;
        u8 componentCount_;
        u8 outputSlot_;
    };

    //------------------------------------------------------------
    //---
    //--- Shader
    //---
    //------------------------------------------------------------
    class Shader
    {
    public:
        /**
        @brief メモリからピクセルシェーダ作成
        @param memory ... 
        @param size ... 
        */
        static PixelShaderRef createPixelShaderFromBinary(const u8* memory, u32 size);

        /**
        @brief メモリから頂点シェーダ作成
        @param memory ... 
        @param size ... 
        */
        static VertexShaderRef createVertexShaderFromBinary(const u8* memory, u32 size);

        /**
        @brief メモリからジオメトリシェーダ作成。StreamOutput用
        @param memory ... 
        @param size ... 
        */
        static GeometryShaderRef createGeometryShaderWithStreamOutputFromBinary(
            const u8* memory,
            u32 size,
            const StreamOutputDeclarationEntry* entries,
            u32 numEntries,
            const u32* bufferStrides,
            u32 numStrides,
            u32 rasterizedStream);

        /**
        @brief メモリからジオメトリシェーダ作成
        @param memory ... 
        @param size ... 
        */
        static GeometryShaderRef createGeometryShaderFromBinary(const u8* memory, u32 size);

        /**
        @brief メモリからコンピュートシェーダ作成
        @param memory ... 
        @param size ... 
        */
        static ComputeShaderRef createComputeShaderFromBinary(const u8* memory, u32 size);

        /**
        @brief メモリからドメインシェーダ作成
        @param memory ...
        @param size ...
        */
        static DomainShaderRef createDomainShaderFromBinary(const u8* memory, u32 size);

        /**
        @brief メモリからハルシェーダ作成
        @param memory ...
        @param size ...
        */
        static HullShaderRef createHullShaderFromBinary(const u8* memory, u32 size);
    };
}
#endif //INC_LGRAPHICS_DX11_SHADERREF_H_
