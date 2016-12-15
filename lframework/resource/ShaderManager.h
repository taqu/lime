#ifndef INC_LFRAMEWORK_SHADERMANAGER_H__
#define INC_LFRAMEWORK_SHADERMANAGER_H__
/**
@file ShaderManager.h
@author t-sakai
@date 2016/11/12 create
*/
#include <lcore/Array.h>
#include "../lframework.h"
#include "ShaderID.h"

namespace lcore
{
    class FileProxy;
}

namespace lgfx
{
    class VertexShaderRef;
    class DomainShaderRef;
    class HullShaderRef;
    class GeometryShaderRef;
    class PixelShaderRef;
    class ComputeShaderRef;

    struct StreamOutputDeclarationEntry;
}

namespace lfw
{
    enum ShaderSetType
    {
        ShaderSetType_Deferred =0,
        ShaderSetType_DeferredUV,
        ShaderSetType_DeferredTexC,
        ShaderSetType_DeferredTexCN,
        ShaderSetType_DeferredSkinning,
        ShaderSetType_DeferredUVSkinning,
        ShaderSetType_DeferredSkinningTexC,
        ShaderSetType_DeferredSkinningTexCN,

        ShaderSetType_Forward,
        ShaderSetType_ForwardUV,
        ShaderSetType_ForwardTexC,
        ShaderSetType_ForwardTexCN,
        ShaderSetType_ForwardSkinning,
        ShaderSetType_ForwardUVSkinning,
        ShaderSetType_ForwardSkinningTexC,
        ShaderSetType_ForwardSkinningTexCN,
        ShaderSetType_ForwardShadow,
        ShaderSetType_ForwardUVShadow,
        ShaderSetType_ForwardTexCShadow,
        ShaderSetType_ForwardTexCNShadow,
        ShaderSetType_ForwardSkinningShadow,
        ShaderSetType_ForwardUVSkinningShadow,
        ShaderSetType_ForwardSkinningTexCShadow,
        ShaderSetType_ForwardSkinningTexCNShadow,

        ShaderSetType_DefaultMax,
        ShaderSetType_User = ShaderSetType_DefaultMax,
    };

    enum ShaderSetFlag
    {
        ShaderSetFlag_None = 0,
        ShaderSetFlag_Skinning = (0x01U<<0),
    };

    struct ShaderSet;
    class Mesh;

    class ShaderManager
    {
    public:
        typedef void(*ShaderSetCreateFunc)(ShaderSet& shaderSet, const Mesh& mesh, ShaderSetFlag flags); 

        ShaderManager();
        ~ShaderManager();

        bool loadDefaultShaders();

        lgfx::VertexShaderRef& getVS(s32 id);
        void addVS(s32 id, lgfx::VertexShaderRef& vs);
        bool loadVS(s32 id, lcore::FileProxy& file);
        bool loadVS(s32 id, s32 size, const u8* mem);
        void releaseVS(s32 id);

        lgfx::DomainShaderRef& getDS(s32 id);
        void addDS(s32 id, lgfx::DomainShaderRef& ds);
        bool loadDS(s32 id, lcore::FileProxy& file);
        bool loadDS(s32 id, s32 size, const u8* mem);
        void releaseDS(s32 id);

        lgfx::HullShaderRef& getHS(s32 id);
        void addHS(s32 id, lgfx::HullShaderRef& hs);
        bool loadHS(s32 id, lcore::FileProxy& file);
        bool loadHS(s32 id, s32 size, const u8* mem);
        void releaseHS(s32 id);

        lgfx::GeometryShaderRef& getGS(s32 id);
        void addGS(s32 id, lgfx::GeometryShaderRef& gs);
        bool loadGS(s32 id, lcore::FileProxy& file);
        bool loadGS(s32 id, s32 size, const u8* mem);
        void releaseGS(s32 id);
        bool loadGSStreamOutput(
            s32 id,
            lcore::FileProxy& file,
            const lgfx::StreamOutputDeclarationEntry* entries,
            u32 numEntries,
            const u32* strides,
            u32 numStrides);
        bool loadGSStreamOutput(
            s32 id,
            s32 size,
            const u8* mem,
            const lgfx::StreamOutputDeclarationEntry* entries,
            u32 numEntries,
            const u32* strides,
            u32 numStrides);

        lgfx::PixelShaderRef& getPS(s32 id);
        void addPS(s32 id, lgfx::PixelShaderRef& ps);
        bool loadPS(s32 id, lcore::FileProxy& file);
        bool loadPS(s32 id, s32 size, const u8* mem);
        void releasePS(s32 id);

        lgfx::ComputeShaderRef& getCS(s32 id);
        void addCS(s32 id, lgfx::ComputeShaderRef& cs);
        bool loadCS(s32 id, lcore::FileProxy& file);
        bool loadCS(s32 id, s32 size, const u8* mem);
        void releaseCS(s32 id);

        void releaseTemporaryResources();

        void setShaderSetCreateFunc(ShaderSetCreateFunc func);
        void setShaderSet(Mesh& mesh, ShaderSetFlag flags);
    private:
        ShaderManager(const ShaderManager&);
        ShaderManager& operator=(const ShaderManager&);

        static const u32 IncSize = 1023;
        inline s32 incSize(s32 size){return (size+IncSize)&~IncSize;}
        s32 load(lcore::FileProxy& file);
        s32 unpack(s32 size, const u8* mem);

        ShaderSet* addShaderSet(ShaderSet& shaderSet);
        void defaultShaderSetCreateFunc(ShaderSet& shaderSet, const Mesh& mesh, ShaderSetFlag flags);

        s32 sourceSize_;
        u8* source_;

        s32 bufferSize_;
        u8* buffer_;

        lcore::Array<lgfx::VertexShaderRef> vs_;
        lcore::Array<lgfx::DomainShaderRef> ds_;
        lcore::Array<lgfx::HullShaderRef> hs_;
        lcore::Array<lgfx::GeometryShaderRef> gs_;
        lcore::Array<lgfx::PixelShaderRef> ps_;
        lcore::Array<lgfx::ComputeShaderRef> cs_;

        ShaderSetCreateFunc shaderSetCreateFunc_;
        s32 shaderSetsSize_;
        ShaderSet** shaderSets_;
    };
}
#endif //INC_LFRAMEWORK_SHADERMANAGER_H__
