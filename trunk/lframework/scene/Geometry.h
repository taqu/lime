#ifndef INC_GEOMETRY_H__
#define INC_GEOMETRY_H__
/**
@file Geometry.h
@author t-sakai
@date 2009/08/10 create
*/
#include "lscene.h"
#include <lgraphics/lgraphicscore.h>
#include <lgraphics/api/GeometryBuffer.h>
#include "../Flags.h"

namespace lscene
{
    class ShaderVSBase;
    class ShaderPSBase;

    //----------------------------------------------------
    //---
    //--- Geometry
    //---
    //----------------------------------------------------
    class Geometry
    {
    public:
        typedef lframework::Flags Flags;

        /// ジオメトリ機能フラグ
        enum GeometryFlag
        {
            GeomFlag_Texture0 = (0x01U<<0),
            GeomFlag_Texture1 = (0x01U<<1),
            GeomFlag_TextureN = (0x01U<<2),
            GeomFlag_TextureS = (0x01U<<3),
            GeomFlag_DiffuseVS = (0x01U<<4),
            GeomFlag_Skinning = (0x01U<<5),
            GeomFlag_Valid = (0x01U<<31),
            GeomFlag_Num = 6,
        };

        Geometry();
        Geometry(u16 count, u32 indexOffset, u8 materialIndex);
        Geometry(lgraphics::GeometryBuffer::pointer& geomBuffer, u16 count, u32 indexOffset, u8 materialIndex);

        ~Geometry();
        
        lgraphics::GeometryBuffer::pointer& getGeometryBuffer(){ return geomBuffer_;}
        void setGeometryBuffer(lgraphics::GeometryBuffer::pointer& geomBuffer){ geomBuffer_ = geomBuffer;}

        u16 getPrimitiveCount() const{ return count_;}
        void setPrimitiveCount(u16 count){ count_ = count;}

        u32 getIndexOffset() const{ return indexOffset_;}
        u32 getNumIndices() const{ return numIndices_;}

        u8 getMaterialIndex() const{ return materialIndex_;}

        inline ShaderVSBase* getShaderVS();
        inline void setShaderVS(ShaderVSBase* vs);

        inline ShaderPSBase* getShaderPS();
        inline void setShaderPS(ShaderPSBase* ps);

        u8 getNumBonesInPalette() const{ return numBonesInPalette_;}
        void setNumBonesInPalette(u8 numBones)
        {
            LASSERT(numBones<=lgraphics::LIME_MAX_SKINNING_MATRICES);
            numBonesInPalette_ = numBones;
        }

        inline u8 getBoneInPalette(u8 index) const;
        inline void setBoneInPalette(u8 index, u8 boneIndex);

        /**
        @brief スワップ
        @notice マトリックスパレットだけスワップしない
        */
        void swap(Geometry& rhs);

        inline const Flags& getFlags() const;
        inline Flags& getFlags();
    private:
        Geometry(const Geometry&);
        Geometry& operator=(const Geometry&);

        lgraphics::GeometryBuffer::pointer geomBuffer_;
        ShaderVSBase* shaderVS_;
        ShaderPSBase* shaderPS_;

        u32 indexOffset_;
        u32 numIndices_;
        u16 count_;
        u8 materialIndex_;
        u8 numBonesInPalette_;
        u8 palette_[lgraphics::LIME_MAX_SKINNING_MATRICES];
        Flags geometryFlags_; /// 機能フラグ
    };

    inline ShaderVSBase* Geometry::getShaderVS()
    {
        return shaderVS_;
    }
    
    inline void Geometry::setShaderVS(ShaderVSBase* vs)
    {
        shaderVS_ = vs;
    }


    inline ShaderPSBase* Geometry::getShaderPS()
    {
        return shaderPS_;
    }
    
    inline void Geometry::setShaderPS(ShaderPSBase* ps)
    {
        shaderPS_ = ps;
    }


    inline u8 Geometry::getBoneInPalette(u8 index) const
    {
        LASSERT(0<=index && index<numBonesInPalette_);
        return palette_[index];
    }

    inline void Geometry::setBoneInPalette(u8 index, u8 boneIndex)
    {
        LASSERT(0<=index && index<numBonesInPalette_);
        palette_[index] = boneIndex;
    }

    inline const Geometry::Flags& Geometry::getFlags() const
    {
        return geometryFlags_;
    }

    inline Geometry::Flags& Geometry::getFlags()
    {
        return geometryFlags_;
    }
}

#endif //INC_GEOMETRY_H__
