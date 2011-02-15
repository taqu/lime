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
        Geometry(u32 primCount, u32 indexOffset, u8 materialIndex);
        Geometry(lgraphics::GeometryBuffer::pointer& geomBuffer, u32 primCount, u32 indexOffset, u8 materialIndex);

        ~Geometry();

        /// GeometryBufferゲット
        inline lgraphics::GeometryBuffer::pointer& getGeometryBuffer();
        /// GeometryBufferセット
        inline void setGeometryBuffer(lgraphics::GeometryBuffer::pointer& geomBuffer);

        /// プリミティブ数ゲット
        inline u32 getPrimitiveCount() const;
        /// プリミティブ数セット
        inline void setPrimitiveCount(u32 count);

        /// インデックスオフセットゲット
        inline u32 getIndexOffset() const;
        /// インデックスオフセットセット
        inline void setIndexOffset(u32 offset);

        /// インデックス数ゲット
        inline u32 getNumIndices() const;
        /// インデックス数セット
        inline void setNumIndices(u32 numIndices);

        /// マテリアルのインデックスゲット
        inline u8 getMaterialIndex() const;
        /// マテリアルのインデックスセット
        inline void setMaterialIndex(u8 index);

        /// ＶＳゲット
        inline ShaderVSBase* getShaderVS();
        /// ＶＳセット
        inline void setShaderVS(ShaderVSBase* vs);

        /// ＰＳゲット
        inline ShaderPSBase* getShaderPS();
        /// ＰＳセット
        inline void setShaderPS(ShaderPSBase* ps);

        /// ボーン数ゲット
        inline u8 getNumBonesInPalette() const;
        /// ボーン数セット
        inline void setNumBonesInPalette(u8 numBones);

        /// ボーンインデックスゲット
        inline u8 getBoneInPalette(u8 index) const;
        /// ボーンインデックスセット
        inline void setBoneInPalette(u8 index, u8 boneIndex);

        /// フラグゲット
        inline const Flags& getFlags() const;
        /// フラグゲット
        inline Flags& getFlags();

        /**
        @brief スワップ
        @notice マトリックスパレットだけスワップしない
        */
        void swap(Geometry& rhs);
    private:
        Geometry(const Geometry&);
        Geometry& operator=(const Geometry&);

        enum Index
        {
            Index_Material =0,
            Index_NumBones,
            Index_Padding0,
            Index_Padding1,
            Index_Num,
        };

        lgraphics::GeometryBuffer::pointer geomBuffer_;
        ShaderVSBase* shaderVS_;
        ShaderPSBase* shaderPS_;

        u32 indexOffset_;
        u32 numIndices_;
        Flags geometryFlags_; /// 機能フラグ

        u32 primitiveCount_; /// プリミティブカウント
        u8 data_[Index_Num];
        u8 palette_[lgraphics::LIME_MAX_SKINNING_MATRICES];
    };

    inline lgraphics::GeometryBuffer::pointer& Geometry::getGeometryBuffer()
    {
        return geomBuffer_;
    }

    inline void Geometry::setGeometryBuffer(lgraphics::GeometryBuffer::pointer& geomBuffer)
    {
        geomBuffer_ = geomBuffer;
    }

    inline u32 Geometry::getPrimitiveCount() const
    {
        return primitiveCount_;
    }

    inline void Geometry::setPrimitiveCount(u32 count)
    {
        primitiveCount_ = count;
    }

    inline u32 Geometry::getIndexOffset() const
    {
        return indexOffset_;
    }

    inline void Geometry::setIndexOffset(u32 offset)
    {
        indexOffset_ = offset;
    }

    inline u32 Geometry::getNumIndices() const
    {
        return numIndices_;
    }

    inline void Geometry::setNumIndices(u32 numIndices)
    {
        numIndices_ = numIndices;
    }

    inline u8 Geometry::getMaterialIndex() const
    {
        return data_[Index_Material];
    }

    inline void Geometry::setMaterialIndex(u8 index)
    {
        data_[Index_Material] = index;
    }

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

    inline u8 Geometry::getNumBonesInPalette() const
    {
        return data_[Index_NumBones];
    }

    inline void Geometry::setNumBonesInPalette(u8 numBones)
    {
        LASSERT(numBones<=lgraphics::LIME_MAX_SKINNING_MATRICES);
        data_[Index_NumBones] = numBones;
    }

    inline u8 Geometry::getBoneInPalette(u8 index) const
    {
        LASSERT(0<=index && index<data_[Index_NumBones]);
        return palette_[index];
    }

    inline void Geometry::setBoneInPalette(u8 index, u8 boneIndex)
    {
        LASSERT(0<=index && index<data_[Index_NumBones]);
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
