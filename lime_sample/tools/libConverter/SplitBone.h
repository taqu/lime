#ifndef INC_SPLITBONE_H__
#define INC_SPLITBONE_H__
/**
@file SplitBone.h
@author t-sakai
@date 2010/12/05 create
*/
#include "Pmd.h"

namespace pmd
{
    /**
    @brief ボーンの組み合わせ

    １三角形のボーンの組み合わせ。１頂点辺り４ボーンまで×３頂点分。
    */
    class BoneCombination
    {
    public:

        static const s32 NumMaxCombination = 12;
        static const u8 FreeSlot = BoneFreeSlot;

        BoneCombination()
            :size_(0)
        {}

        bool operator==(const BoneCombination& rhs) const
        {
            if(size_ != rhs.size_){
                return false;
            }

            for(s32 i=0; i<size_; ++i){
                if(indices_[i] != rhs.indices_[i]){
                    return false;
                }
            }
            return true;
        }

        void addBoneIndex(u8 index)
        {
            LASSERT(size_<=NumMaxCombination);
            indices_[size_] = index;
            ++size_;
        }

        s32 findBoneIndex(u8 index)
        {
            for(s32 i=0; i<size_; ++i){
                if(indices_[i] == 0xFFU){
                    break;
                }
                if(indices_[i] == index){
                    return i;
                }
            }
            return -1;
        }

        s32 size() const
        {
            return size_;
        }

        void reset()
        {
            size_ = 0;
            lcore::memset((void*)indices_, BoneCombination::FreeSlot, NumMaxCombination);
        }

        s32 size_;
        u8 indices_[NumMaxCombination];
    };


    /**
    */
    class BoneCombinationWrapper
    {
    public:
        BoneCombinationWrapper()
            :impl_(NULL)
        {}

        /**
        意図してexplicitじゃない
        */
        BoneCombinationWrapper(BoneCombination* impl)
            :impl_(impl)
        {}

        bool operator==(const BoneCombinationWrapper& rhs) const
        {
            if(impl_ == rhs.impl_){
                return true;
            }

            if(impl_ == NULL || rhs.impl_ == NULL){
                return false;
            }

            return (*impl_ == *(rhs.impl_));
        }

        BoneCombination *impl_;
    };


    class VertexKey
    {
    public:
        VertexKey()
            :palette_(NULL)
            ,originalIndex_(0xFFFFU)
        {}

        VertexKey(Palette* palette, u16 index)
            :palette_(palette)
            ,originalIndex_(index)
        {
        }

        void set(Palette* palette, u16 index)
        {
            palette_ = palette;
            originalIndex_ = index;
        }

        bool operator==(const VertexKey& rhs) const
        {
            return (palette_ == rhs.palette_) && (originalIndex_ == rhs.originalIndex_);
        }

        bool operator!=(const VertexKey& rhs) const
        {
            return (palette_ != rhs.palette_) || (originalIndex_ != rhs.originalIndex_);
        }

        Palette* palette_;
        u16 originalIndex_;
    };

    /**
    */
    void splitBone(
        u32 numVertices,
        Vertex* vertices,
        u32 numFaceIndices,
        FaceIndex* faceIndices,
        u32 numMaterials,
        Material* materials,
        u32 numBones,
        u32 maxBones,
        u32& numGeometries,
        Geometry** geometries);
}

#endif //INC_SPLITBONE_H__
