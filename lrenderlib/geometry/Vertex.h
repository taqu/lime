#ifndef INC_LRENDER_VERTEX_H__
#define INC_LRENDER_VERTEX_H__
/**
@file Vertex.h
@author t-sakai
@date 2009/12/21 create
*/
#include <lmath/Vector3.h>
#include <lcore/FixedArray.h>

namespace lrender
{
    class Vertex
    {
    public:
        lmath::Vector3 position_;
        lmath::Vector3 normal_;
    };

    typedef lcore::FixedArray<Vertex> VertexBuffer;
}

#endif //INC_LRENDER_VERTEX_H__
