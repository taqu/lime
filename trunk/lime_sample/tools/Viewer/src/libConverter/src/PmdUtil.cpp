/**
@file PmdUtil.cpp
@author t-sakai
@date 2010/07/04 create
*/
#include "PmdUtil.h"

#include <lgraphics/scene/Geometry.h>
using namespace lgraphics;

namespace pmd
{
    typedef std::vector<u8> BoneIndexVector;

    namespace
    {
        template<class T>
        bool find(typename std::vector<T>::const_iterator begin, typename std::vector<T>::const_iterator end, const T& value, u8& index)
        {
            u8 i =0;
            for(std::vector<T>::const_iterator itr = begin;
                itr != end;
                ++itr)
            {
                if(*itr == value){
                    index = i;
                    return true;
                }
                ++i;
            }
            return false;
        }
    }

    void seperateGeometry(
        u32 numVertices,
        pmd::Vertex* pmdVertices,
        PmdVertexVector& vertices,
        lcore::u32 startIndex,
        lcore::u16* pmdIndices,
        lcore::s32* tmpIndices,
        lcore::u32 materialIndex,
        pmd::Material& material,
        BoneIndexVector& boneIndices,
        GeomPtrVector& retGeometries,
        u32 maxBones)
    {
#if 1
        Geometry *geom = LIME_NEW Geometry(material.faceVertexCount_/3, materialIndex, startIndex);
        retGeometries.push_back(geom);
#else
        u32 count = 0;
        for(u32 i=0; i<material.faceVertexCount_; i+=3){
            ++count;

            for(u32 j=0; j<3; ++j){
                u16 index = pmdIndices[i+j];
                if(tmpIndices[index] < 0){
                    tmpIndices[index] = vertices.size();
                    vertices.push_back(pmdVertices[index]);
                }
                pmdIndices[i+j] = tmpIndices[index];

            }

            // 配列にないボーンインデックスなら、配列に入れる
            for(u32 j=0; j<3; ++j){
                pmd::Vertex& v = vertices[ pmdIndices[i+j] ];

                for(u32 k=0; k<2; ++k){
                    u8 boneIndex = 0;
                    if(find(boneIndices.begin(), boneIndices.end(), v.boneNo_[k], boneIndex)){
                        v.boneNo_[k] = boneIndex;
                    }else{
                        boneIndices.push_back(v.boneNo_[k]);
                        v.boneNo_[k] = boneIndices.size();
                    }
                }
            }

            if(boneIndices.size()>=maxBones){
                Geometry* geom = LIME_NEW Geometry(count, materialIndex, boneIndices.size(), startIndex);
                geom->setNumBones(boneIndices.size());
                for(u32 k=0; k<boneIndices.size(); ++k){
                    geom->setBoneIndex(k, boneIndices[k]);
                }
                retGeometries.push_back(geom);
                boneIndices.clear();

                for(u32 k=0; k<numVertices; ++k){
                    tmpIndices[k] = -1;
                }
                startIndex += count*3;
                count = 0;
            }
        }

        if(count <= 0){
            return;
        }

        Geometry* geom = LIME_NEW Geometry(count, materialIndex, boneIndices.size(), startIndex);
        geom->setNumBones(boneIndices.size());
        for(u32 k=0; k<boneIndices.size(); ++k){
            geom->setBoneIndex(k, boneIndices[k]);
        }
        retGeometries.push_back(geom);
        boneIndices.clear();

        for(u32 k=0; k<numVertices; ++k){
            tmpIndices[k] = -1;
        }
#endif
    }


    void seperateGeometry(
        lcore::u32 numVertices,
        pmd::Vertex* pmdVertices,
        PmdVertexVector& vertices,
        lcore::u32 numIndices,
        lcore::u16* pmdIndices,
        lcore::u32 numMaterials,
        pmd::Material* materials,
        GeomPtrVector& retGeometries,
        u32 maxBones)
    {
        BoneIndexVector boneIndices;

        u32 startIndex = 0;
        for(u32 i=0; i<numMaterials; ++i){
            pmd::Material& material = materials[i];

            seperateGeometry(
                numVertices,
                pmdVertices,
                vertices,
                startIndex,
                pmdIndices + startIndex,
                NULL,
                i,
                material,
                boneIndices,
                retGeometries,
                maxBones);
            startIndex += material.faceVertexCount_;
        }
    }
}
