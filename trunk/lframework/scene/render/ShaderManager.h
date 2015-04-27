#ifndef INC_LSCENE_LRENDER_SHADERMANAGER_H__
#define INC_LSCENE_LRENDER_SHADERMANAGER_H__
/**
@file ShaderManager.h
@author t-sakai
@date 2014/12/15 create
*/
#include <lcore/Vector.h>
#include "render.h"
#include "ShaderID.h"
#include "ShaderSet.h"

namespace lgraphics
{
    class VertexShaderRef;
    class GeometryShaderRef;
    class PixelShaderRef;
    class ComputeShaderRef;
    class DomainShaderRef;
    class HullShaderRef;
}

namespace lscene
{
namespace lrender
{
    struct ShaderSet;

    class ShaderManager : public ShaderManagerAllocator
    {
    public:
        static const s32 InitSize = 16;

        static void initialize(s32 numVS, s32 numGS, s32 numPS, s32 numCS, s32 numDS, s32 numHS);
        static void terminate();

        inline static ShaderManager& getInstance(){ return *instance_;}

        lgraphics::VertexShaderRef* getVS(s32 id);
        void setVS(s32 id, lgraphics::VertexShaderRef& vs);

        lgraphics::GeometryShaderRef*getGS(s32 id);
        void setGS(s32 id, lgraphics::GeometryShaderRef& gs);

        lgraphics::PixelShaderRef* getPS(s32 id);
        void setPS(s32 id, lgraphics::PixelShaderRef& ps);

        lgraphics::ComputeShaderRef* getCS(s32 id);
        void setCS(s32 id, lgraphics::ComputeShaderRef& cs);

        lgraphics::DomainShaderRef* getDS(s32 id);
        void setDS(s32 id, lgraphics::DomainShaderRef& ds);

        lgraphics::HullShaderRef* getHS(s32 id);
        void setHS(s32 id, lgraphics::HullShaderRef& hs);

        ShaderSet* addShaderSet(const ShaderSet& set);
        ShaderSet* findShaderSet(s32 key);
    private:
        ShaderManager(const ShaderManager&);
        ShaderManager& operator=(const ShaderManager&);

        typedef lcore::vector_arena<ShaderSet*, lscene::SceneAllocator, lcore::vector_arena_static_inc_size<16> > ShaderSetVector;

        struct CompFunc
        {
            s32 operator()(const ShaderSet* v0, const s32& key) const;
        };

        ShaderManager(s32 numVS, s32 numGS, s32 numPS, s32 numCS, s32 numDS, s32 numHS);
        ~ShaderManager();

        template<class T>
        void create(T** v, s32 num);

        template<class T>
        void destroy(T** v, s32& num);

        static ShaderManager* instance_;
        s32 numVS_;
        s32 numGS_;
        s32 numPS_;
        s32 numCS_;
        s32 numDS_;
        s32 numHS_;

        lgraphics::VertexShaderRef* vs_;
        lgraphics::GeometryShaderRef* gs_;
        lgraphics::PixelShaderRef* ps_;
        lgraphics::ComputeShaderRef* cs_;
        lgraphics::DomainShaderRef* ds_;
        lgraphics::HullShaderRef* hs_;

        ShaderSetVector shaderSets_;
    };

    template<class T>
    void ShaderManager::create(T** v, s32 num)
    {
        *v = reinterpret_cast<T*>( LSCENE_MALLOC(num*sizeof(T)) );
        for(s32 i=0; i<num; ++i){
            T* p = &((*v)[i]);
            LIME_PLACEMENT_NEW(p) T();
        }
    }

    template<class T>
    void ShaderManager::destroy(T** v, s32& num)
    {
        for(s32 i=0; i<num; ++i){
            (*v)[i].~T();
        }
        num = 0;
        LSCENE_FREE(*v);
    }
}
}
#endif //INC_LSCENE_LRENDER_SHADERMANAGER_H__
