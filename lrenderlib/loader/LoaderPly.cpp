/**
@file LoaderPly.cpp
@author t-sakai
@date 2013/05/13 create
*/
#include "LoaderPly.h"
#include <lcore/liostream.h>
#include "shape/Mesh.h"

namespace lrender
{
namespace
{
    u32 readLine(Char* dst, lcore::ifstream& file)
    {
        u32 count = 0;
        while(!file.eof()){
            s32 c = file.get();
            if(c == 0x0A){
                break;
            }
            dst[count] = c;
            ++count;
        }
        dst[count] = '\0';
        return count;
    }

    u32 split(Char** dst, Char* src, const Char* end)
    {
        u32 count = 0;
        Char* top = src;
        while(end != src){
            if(lcore::isSpace(*src)){
                *src = '\0';
                ++src;
                while(end != src){
                    if(!lcore::isSpace(*src)){
                        break;
                    }
                    *src = '\0';
                }
                dst[count] = top;
                ++count;
                top = src;
            }else{
                ++src;
            }
        }

        if(*top != '\0'){
            dst[count] = top;
            ++count;
        }
        return count;
    }

    static const s32 StateElementInvalid = -1;
    static const s32 StateElementVertex = 0;
    static const s32 StateElementFace = 1;
    s32 getElementState(const Char* element)
    {
        if(0==lcore::strncmp(element, "vertex", strlen("vertex"))){
            return StateElementVertex;
        }else if(0==lcore::strncmp(element, "face", strlen("face"))){
            return StateElementFace;
        }
        return StateElementInvalid;
    }
}

    LoaderPly::LoaderPly()
    {
    }

    LoaderPly::~LoaderPly()
    {
    }

    s32 LoaderPly::loadFromFile(Shape::ShapeVector& shapes, const Char* filename)
    {
        LASSERT(NULL != filename);

        lcore::ifstream file(filename);
        if(!file.is_open()){
            return -1;
        }

        Char buffer[1024];
        Char* elements[64];
        u32 lineLen;
        u32 numElements;

        s32 state = StateElementVertex;
        s32 prop = 0;
        s32 posindices[3];
        u32 numVertices = 0;
        u32 numFaces = 0;
        while(!file.eof()){
            lineLen = readLine(buffer, file);
            if(lineLen<=0){
                continue;
            }
            numElements = split(elements, buffer, buffer+lineLen);
            if(0 == lcore::strncmp(elements[0], "format", strlen("format"))){
            }else if(0 == lcore::strncmp(elements[0], "comment", strlen("comment"))){
            }else if(0 == lcore::strncmp(elements[0], "element", strlen("element"))){
                state = getElementState(elements[1]);
                prop = 0;

                switch(state)
                {
                case StateElementVertex:
                    numVertices = atoi(elements[2]);
                    break;

                case StateElementFace:
                    numFaces = atoi(elements[2]);
                    break;
                };

            }else if(0 == lcore::strncmp(elements[0], "property", strlen("property"))){
                switch(state)
                {
                case StateElementVertex:
                    if(0 == lcore::strncmp(elements[2], "x", strlen("x"))){
                        posindices[0] = prop;
                    }else if(0 == lcore::strncmp(elements[2], "y", strlen("y"))){
                        posindices[1] = prop;
                    }else if(0 == lcore::strncmp(elements[2], "z", strlen("z"))){
                        posindices[2] = prop;
                    }
                    break;

                case StateElementFace:
                    break;
                };
                ++prop;
            }else if(0 == lcore::strncmp(elements[0], "end_header", strlen("end_header"))){
                break;
            }
        }

        BufferVector3 points;
        BufferVector3 normals;
        BufferVector3::create(points, numVertices);
        BufferVector3::create(normals, numVertices);
        for(u32 i=0; i<numVertices; ++i){
            lineLen = readLine(buffer, file);
            if(lineLen<=0){
                continue;
            }
            numElements = split(elements, buffer, buffer+lineLen);
            points[i].x_ = atof(elements[posindices[0]]);
            points[i].y_ = atof(elements[posindices[1]]);
            points[i].z_ = atof(elements[posindices[2]]);
            normals[i].zero();
        }

        BufferTriangle triangles;
        BufferTriangle::create(triangles, numFaces);

        AABB  bbox;
        bbox.setInvalid();

        AABB tbbox;
        lmath::Vector3 normal;
        for(u32 i=0; i<numFaces; ++i){
            lineLen = readLine(buffer, file);
            if(lineLen<=0){
                continue;
            }
            numElements = split(elements, buffer, buffer+lineLen);

            s32 i0 = atoi(elements[1]);
            s32 i1 = atoi(elements[2]);
            s32 i2 = atoi(elements[3]);

            triangles[i].indices_[0] = i0;
            triangles[i].indices_[1] = i1;
            triangles[i].indices_[2] = i2;

            getBBox(tbbox, points[i0], points[i1], points[i2]);
            bbox.extend(tbbox);
            getNormal(normal, points[i0], points[i1], points[i2]);

            normals[i0] += normal;
            normals[i1] += normal;
            normals[i2] += normal;
        }

        //
        for(u32 i=0; i<numVertices; ++i){
            f32 l = normals[i].lengthSqr();
            if(lmath::isZero(l)){
                continue;
            }
            normals[i] /= lmath::sqrt(l);
        }

        Mesh* mesh =  LIME_NEW Mesh();

        BufferVector2 uvs;
        BufferVector4 colors;
        Mesh::create(*mesh,
            bbox.bmin_,
            bbox.bmax_,
            points,
            normals,
            uvs,
            colors,
            triangles);
        shapes.push_back(Shape::pointer(mesh));
        return 1;
    }
}
