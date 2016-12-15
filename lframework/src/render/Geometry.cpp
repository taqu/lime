/**
@file Geometry.cpp
@author t-sakai
@date 2016/11/23 create
*/
#include "render/Geometry.h"

namespace lfw
{
    Geometry::Geometry()
        :vflag_(0)
        ,vsize_(0)
        ,numVertices_(0)
        ,numIndices_(0)
        ,vertices_(NULL)
        ,indices_(NULL)
    {
    }

    Geometry::~Geometry()
    {
        LFREE(indices_);
        LFREE(vertices_);
    }

    void Geometry::create(
        u32 vflag,
        u32 vsize,
        u32 numVertices,
        u8* vertices,
        u32 numIndices,
        u16* indices,
        lgfx::InputLayoutRef& inputLayout,
        lgfx::VertexBufferRef& vb,
        lgfx::IndexBufferRef& ib)
    {
        LFREE(indices_);
        LFREE(vertices_);

        vflag_ = vflag;
        vsize_ = vsize;
        numVertices_ = numVertices;
        vertices_ = vertices;
        numIndices_ = numIndices;
        indices_ = indices;

        inputLayout_ = inputLayout;
        vb_ = vb;
        ib_ = ib;
    }

    void Geometry::copyTo(Geometry& dst)
    {
        dst.vflag_ = vflag_;
        dst.vsize_ = vsize_;
        dst.numVertices_ = numVertices_;
        dst.inputLayout_ = inputLayout_;
        dst.vb_ = vb_;
        dst.ib_ = ib_;
    }

    void Geometry::swap(Geometry& rhs)
    {
        lcore::swap(vflag_, rhs.vflag_);
        lcore::swap(vsize_, rhs.vsize_);
        lcore::swap(numVertices_, rhs.numVertices_);
        lcore::swap(numIndices_, rhs.numIndices_);
        lcore::swap(vertices_, rhs.vertices_);
        lcore::swap(indices_, rhs.indices_);
        inputLayout_.swap(rhs.inputLayout_);
        vb_.swap(rhs.vb_);
        ib_.swap(rhs.ib_);
    }
}
