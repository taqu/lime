/**
@file IOObject.cpp
@author t-sakai
@date 2010/05/16 create
*/
#include "IOObject.h"

#include <lcore/HashMap.h>

#include <lgraphics/io/IOGeometryBuffer.h>
#include <lgraphics/io/IOTexture.h>
#include <lgraphics/io/IOGeometryBuffer.h>
#include <lgraphics/api/TextureRef.h>
#include <lgraphics/api/GeometryBuffer.h>

#include "lscene.h"
#include "../IOUtil.h"
#include "../SectionID.h"

#include "IOMaterial.h"
#include "IOGeometry.h"

#include "AnimObject.h"
#include "Geometry.h"
#include "Material.h"

using namespace lframework;

namespace lscene
{
    namespace
    {
        struct ObjHeader
        {
            u32 id_;
            u32 numGeometries_;
            u32 numMaterials_;
        };

        typedef lcore::HashMapCharArray<lgraphics::TextureRef> NameTextureMap;

    }

    bool IOObject::read(lcore::istream &is, AnimObject& obj)
    {
        // ヘッダ入力
        ObjHeader header;
        lcore::io::read(is, header);

        lmath::Vector4 bsphere;
        lcore::io::read(is, bsphere);

        lgraphics::GeometryBuffer::pointer buffer;
        lgraphics::io::IOGeometryBuffer::read(is, buffer);

        AnimObject tmp(header.numGeometries_,
            header.numMaterials_);

        tmp.setBSphere(bsphere);

         for(u32 i=0; i<header.numGeometries_; ++i){
            IOGeometry::read(is, tmp.getGeometry(i));

            //すでにロードしたバッファでないならロード
            u32 bufferID = i;
            lcore::io::read(is, bufferID);

            if(bufferID < i){
                lgraphics::GeometryBuffer::pointer& geomBuffer = tmp.getGeometry(bufferID).getGeometryBuffer();
                tmp.getGeometry(i).setGeometryBuffer( geomBuffer );

            }else{
                lgraphics::GeometryBuffer::pointer geomBuffer;
                lgraphics::io::IOGeometryBuffer::read(is, geomBuffer);
                tmp.getGeometry(i).setGeometryBuffer(geomBuffer);
            }
        }

        for(u32 i=0; i<header.numMaterials_; ++i){
            lscene::Material& material = tmp.getMaterial(i);
            IOMaterial::read(is, material);

            // テクスチャ名ロード
            for(u32 j=0; j<material.getTextureNum(); ++j){
                lgraphics::TextureRef& texture = material.getTexture(j);
                is >> texture.getName();
            }
        }

        NameTextureMap texMap(header.numMaterials_ * 2); //テクスチャは重複するため後でロード

        {//名前と一緒にテクスチャをロード
            u32 texNum = 0;
            lcore::io::read(is, texNum);

            lgraphics::TextureRef texture;

            for(u32 i=0; i<texNum; ++i){

                lgraphics::io::IOTexture::read(is, texture);

                NameString& name = texture.getName();
                texMap.insert(name.c_str(), name.size(), texture);
            }

            for(u32 i=0; i<header.numMaterials_; ++i){
                Material& mat = tmp.getMaterial(i);
                for(u32 j=0; j<mat.getTextureNum(); ++j){

                    NameString& name = mat.getTexture(j).getName();

                    u32 pos = texMap.find(name.c_str(), name.size());
                    //LASSERT(texMap.isEnd(pos) == false); //必ず存在する
                    if(texMap.isEnd(pos) == false){
                        mat.setTexture(j, texMap.getValue(pos));
                    }
                }
            }
        }

        obj.swap(tmp);
        return true;
    }

    bool IOObject::write(lcore::ostream &os, AnimObject& obj)
    {
        // ヘッダ出力
        ObjHeader header;
        header.id_ = ObjectID;
        header.numGeometries_ = obj.getNumGeometries();
        header.numMaterials_ = obj.getNumMaterials();

        lcore::io::write(os, header);
        lcore::io::write(os, obj.getBSphere());

        for(u32 i=0; i<header.numGeometries_; ++i){
            lscene::Geometry& geometry = obj.getGeometry(i);
            IOGeometry::write(os, geometry);

            //以前に同じジオメトリバッファを保存していたら、そのインデックスを保存
            u32 bufferID = i;
            lgraphics::GeometryBuffer::pointer& geomBuffer = geometry.getGeometryBuffer();
            for(u32 j=0; j<i; ++j){
                if(obj.getGeometry(j).getGeometryBuffer() == geomBuffer){
                    bufferID = j;
                    break;
                }
            }
            lcore::io::write(os, bufferID);
            if(bufferID != i){ //すでに保存したバッファでなければ保存
                lgraphics::io::IOGeometryBuffer::write(os, geometry.getGeometryBuffer());
            }
        }

        NameTextureMap texMap(header.numMaterials_ * 2); //テクスチャは重複するため後で保存する
        for(u32 i=0; i<header.numMaterials_; ++i){
            Material& mat = obj.getMaterial(i);
            IOMaterial::write(os, mat);

            // テクスチャ名保存
            for(u32 j=0; j<mat.getTextureNum(); ++j){
                lgraphics::TextureRef& texture = mat.getTexture(j);
                os << texture.getName();
                texMap.insert(texture.getName().c_str(), texture.getName().size(), texture);
            }
        }


        {//名前と一緒にテクスチャを保存
            u32 size = texMap.size();
            lcore::io::write(os, size);

            u32 end = texMap.end();
            for(u32 itr = texMap.begin(); itr!=end; itr=texMap.next(itr)){
                lgraphics::TextureRef &texture = texMap.getValue(itr);

                lgraphics::io::IOTexture::write(os, texture);
            }

        }

        return true;
    }

}
