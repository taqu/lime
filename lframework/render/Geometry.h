#ifndef INC_LFRAMEWORK_GEOMETRY_H_
#define INC_LFRAMEWORK_GEOMETRY_H_
/**
@file Geometry.h
@author t-sakai
@date 2016/11/23 create
*/
#include "../lframework.h"
#include <lgraphics/GraphicsDeviceRef.h>
#include <lgraphics/InputLayoutRef.h>
#include <lgraphics/VertexBufferRef.h>
#include <lgraphics/IndexBufferRef.h>

namespace lgfx
{
    class ContextRef;
}

namespace lfw
{
    class Geometry
    {
    public:
        Geometry();
        ~Geometry();

        void create(
            u32 vflag,
            u32 vsize,
            u32 numVertices,
            u8* vertices,
            u32 numIndices,
            u16* indices,
            lgfx::InputLayoutRef& inputLayout,
            lgfx::VertexBufferRef& vb,
            lgfx::IndexBufferRef& ib);

        void copyTo(Geometry& dst);
        inline u32 getVFlag() const;
        inline u32 getVSize() const;

        inline u32 getNumVertices() const;
        inline const u8* getVertices() const;
        inline u32 getNumIndices() const;
        inline const u16* getIndices() const;

        inline void attach(lgfx::ContextRef& context);

        void swap(Geometry& rhs);

        inline bool valid() const;
    private:
        Geometry(const Geometry&);
        Geometry& operator=(const Geometry&);

        u32 vflag_;
        u32 vsize_;
        u32 numVertices_;
        u32 numIndices_;
        u8* vertices_;
        u16* indices_;

        lgfx::InputLayoutRef inputLayout_;
        lgfx::VertexBufferRef vb_;
        lgfx::IndexBufferRef ib_;
    };

    inline u32 Geometry::getVFlag() const
    {
        return vflag_;
    }

    inline u32 Geometry::getVSize() const
    {
        return vsize_;
    }

    inline u32 Geometry::getNumVertices() const
    {
        return numVertices_;
    }

    inline const u8* Geometry::getVertices() const
    {
        return vertices_;
    }

    inline u32 Geometry::getNumIndices() const
    {
        return numIndices_;
    }

    inline const u16* Geometry::getIndices() const
    {
        return indices_;
    }

    inline void Geometry::attach(lgfx::ContextRef& context)
    {
        inputLayout_.attach(context);
        vb_.attach(context, 0, vsize_, 0);
        ib_.attach(context, lgfx::Data_R16_UInt, 0);
    }

    inline bool Geometry::valid() const
    {
        return (inputLayout_.valid() && vb_.valid());
    }
}
#endif //INC_LFRAMEWORK_GEOMETRY_H_
