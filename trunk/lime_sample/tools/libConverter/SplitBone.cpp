/**
@file SplitBone.cpp
@author t-sakai
@date 2010/12/05 create
*/
#include <lcore/HashMap.h>
#include <lcore/HashSet.h>
#include <lcore/Vector.h>

#include "splitBone.h"

#if defined(_WIN32) && defined(_DEBUG)
//#define LIME_LIB_CONV_DEBUG (1)
#endif

#if defined(LIME_LIB_CONV_DEBUG)
#include <lcore/liostream.h>
#endif

namespace lcore
{
    template<>
    struct hasher<pmd::BoneCombination*>
    {
        inline static size_t calc(const pmd::BoneCombination* t)
        {
            if(t == NULL){
                return 0;
            }

            const unsigned char* v = reinterpret_cast<const unsigned char*>(t->indices_);
            return lcore::calc_hash(v, sizeof(u8)*t->size());
        }
    };

    template<>
    struct hasher<pmd::BoneCombinationWrapper>
    {
        inline static size_t calc(const pmd::BoneCombinationWrapper& t)
        {
            if(t.impl_ == NULL){
                return 0;
            }

            const unsigned char* v = reinterpret_cast<const unsigned char*>(t.impl_->indices_);
            return lcore::calc_hash(v, sizeof(u8)*t.impl_->size());
        }
    };

    template<>
    struct hasher<pmd::Palette::pointer>
    {
        inline static size_t calc(const pmd::Palette::pointer& t)
        {
            if(t == NULL){
                return 0;
            }

            const unsigned char* v = reinterpret_cast<const unsigned char*>(t.get());
            return lcore::calc_hash(reinterpret_cast<const unsigned char*>(&v), sizeof(const unsigned char*));
        }
    };

}

namespace pmd
{

namespace
{
    typedef lcore::HashSet<BoneCombinationWrapper> BoneCombinationSet;
    typedef lcore::HashMap<BoneCombinationWrapper, Palette::pointer> CombinationPaletteMap;


    //----------------------------------------------------------------------
    /**
    @brief ボーンコンビネーションセット内のボーンを、パレットに追加
    @param boneCombinationSet
    @param bonePalette
    @param addedBones
    @param maxBones
    */
    void addBoneToPalette(BoneCombinationSet& boneCombinationSet, Palette::pointer& bonePalette, bool* addedBones, u32 maxBones)
    {
        for(BoneCombinationSet::size_type pos = boneCombinationSet.begin();
            pos != boneCombinationSet.end();
            pos = boneCombinationSet.next(pos))
        {
            BoneCombination* combination = boneCombinationSet.getValue(pos).impl_;
            for(s32 j=0; j<BoneCombination::NumMaxCombination; ++j){
                if(combination->indices_[j] == BoneCombination::FreeSlot){
                    break;
                }

                LASSERT(0<=combination->indices_[j] && combination->indices_[j] < MaxBones);

                if(false == addedBones[ combination->indices_[j] ]){
                    bonePalette->add( combination->indices_[j] );
                    addedBones[ combination->indices_[j] ] = true;
                }

                if(bonePalette->size()>=maxBones){
                    return;
                }
            }
        }// for(BoneCombinationMap
        return;
    }


    //----------------------------------------------------------------------
    /**
    @brief パレット作成
    */
    u32 createPalette(BoneCombinationSet& boneCombinationSet, CombinationPaletteMap& combinationPaletteMap, u32* boneCombinationCount, u32 maxBones)
    {
        bool addedBones[MaxBones];

        BoneCombinationWrapper emptyBoneCombination;

        Palette::pointer bonePalette(NULL);

        u32 numPalettes = 0;
        while(boneCombinationSet.size()>0){
            for(u32 j=0; j<MaxBones; ++j){
                addedBones[j] = false;
            }

            bonePalette = LIME_NEW Palette;
            while(bonePalette->size() < maxBones){

                //最も参照されている数の少ないボーンを探索
                s32 index = -1;
                u32 count = 0xFFFFFFFFU;
                for(u32 j=0; j<MaxBones; ++j){
                    if(boneCombinationCount[j]>0
                        && boneCombinationCount[j]<count)
                    {
                        count = boneCombinationCount[j];
                        index = j;
                    }
                }
                //参照されているボーンがなかったので終了
                if(index<0){
                    return numPalettes;
                }

                //-------------------------------------------------------------
                LASSERT(0<=index && index < MaxBones);
                u8 targetIndex = static_cast<u8>(index);
                if(addedBones[targetIndex] == false){
                    bonePalette->add(targetIndex);
                    addedBones[targetIndex] = true;
                }

                //パレットにボーンを追加
                if(bonePalette->size() < maxBones){
                    addBoneToPalette(boneCombinationSet, bonePalette, addedBones, maxBones);
                }

                //-------------------------------------------------------------------
                for(BoneCombinationSet::size_type pos = boneCombinationSet.begin();
                    pos != boneCombinationSet.end();
                    pos = boneCombinationSet.next(pos))
                {

                    BoneCombination* combination = boneCombinationSet.getValue(pos).impl_;
                    bool added = true;
                    for(s32 j=0; j<BoneCombination::NumMaxCombination; ++j){
                        if(combination->indices_[j] == BoneCombination::FreeSlot){
                            break;
                        }

                        if(false == addedBones[ combination->indices_[j] ]){
                            added = false;
                            break;
                        }
                    }

                    if(added){
                        for(s32 j=0; j<BoneCombination::NumMaxCombination; ++j){
                            if(combination->indices_[j] == BoneCombination::FreeSlot){
                                break;
                            }
                            --boneCombinationCount[ combination->indices_[j] ];
                        }
                        combinationPaletteMap.insert(combination, bonePalette);

                        boneCombinationSet.erase(pos);
                    }
                }// for(BoneCombinationSet::size_type 
            }// while(bonePalette.size()<maxBones)
            ++numPalettes;

        }// while(boneCombinationSet.size()>0)

        return numPalettes;
    }


    //----------------------------------------------------------------------
    /**
    @brief マテリアル内でボーン分割
    */
    void splitBone(
        Vertex* vertices,
        FaceIndex* faceIndices,
        u32 materialIndex,
        Material& material,
        u32 numBones,
        u32 maxBones,
        u32& numGeometries,
        Geometry** geometries)
    {
        LASSERT(geometries != NULL);
        LASSERT(numBones<=MaxBones);

        u32 numTriangles = material.faceVertexCount_/3;

        BoneCombinationWrapper emptyBoneCombination;

        BoneCombination *combinations = LIME_NEW BoneCombination[numTriangles];
        BoneCombinationSet boneCombinationSet(numTriangles, emptyBoneCombination);

        //-------------------------------------------------------------------
        // 三角形毎の使用ボーン組み合わせを調査。重複がないようにSetに保存
        bool boneUsed[MaxBones];

        for(u32 i=0; i<numTriangles; ++i){
            combinations[i].reset();

            for(u8 j=0; j<MaxBones; ++j){
                boneUsed[j] = false;
            }

            u32 triangle = 3*i;
            for(u32 j=0; j<3; ++j){
                BYTE index = vertices[ faceIndices[triangle + j].index_ ].element_[ Vertex::Index_Bone0 ];
                boneUsed[index] = true;

                index = vertices[ faceIndices[triangle + j].index_ ].element_[ Vertex::Index_Bone1 ];
                boneUsed[index] = true;
            }

            for(u8 j=0; j<MaxBones; ++j){
                if(boneUsed[j]){
                    combinations[i].addBoneIndex(j);
                }
            }

            boneCombinationSet.insert( &combinations[i] );
        }

        //----------------------------------------------------------------------
        // 各ボーンが組み合わせから参照されている回数をカウント
        u32 boneCombinationCount[MaxBones];
        lcore::memset(boneCombinationCount, 0, sizeof(u32)*MaxBones);
        for(BoneCombinationSet::size_type pos = boneCombinationSet.begin();
            pos != boneCombinationSet.end();
            pos = boneCombinationSet.next(pos))
        {
            BoneCombination* combination = boneCombinationSet.getValue(pos).impl_;
            for(s32 i=0; i<BoneCombination::NumMaxCombination; ++i){
                if(combination->indices_[i] != BoneCombination::FreeSlot){
                    LASSERT(0<=combination->indices_[i] && combination->indices_[i]<MaxBones);

                    boneCombinationCount[ combination->indices_[i] ] += 1;
                }
            }
        }

        // パレット作成
        //----------------------------------------------------------------------
        CombinationPaletteMap combinationPaletteMap(boneCombinationSet.size()*2, emptyBoneCombination);

        u32 numPalettes = createPalette(boneCombinationSet, combinationPaletteMap, boneCombinationCount, maxBones);


        //パレットからジオメトリへのマップ
        typedef lcore::HashMap<Palette::pointer, Geometry*> PaletteGeometryMap;
        Palette::pointer emptyPalette;
        PaletteGeometryMap paletteGeometryMap(numPalettes*2, emptyPalette);

        //面インデックスをジオメトリに入れる
        //-----------------------------------------
        Geometry* top = NULL;
        numGeometries = 0;
        for(u32 i=0; i<numTriangles; ++i){
            BoneCombination& c = combinations[i];
            CombinationPaletteMap::size_type palettePos = combinationPaletteMap.find(&c);
            LASSERT(palettePos != combinationPaletteMap.end());

            Palette::pointer& palette = combinationPaletteMap.getValue(palettePos);
            LASSERT(palette->size()<=maxBones);

            //パレットに対応するジオメトリがなかったら追加
            Geometry* geometry = NULL;
            PaletteGeometryMap::size_type pos = paletteGeometryMap.find(palette);
            if(paletteGeometryMap.isEnd(pos)){
                geometry = LIME_NEW Geometry;

                geometry->faceIndices_.setIncSize(material.faceVertexCount_);
                geometry->materialIndex_ = materialIndex;
                geometry->palette_ = palette;
                geometry->next_ = NULL;

                if(top == NULL){
                    top = geometry;
                    *geometries = geometry; //最初なので戻り値設定
                }else{
                    top->next_ = geometry;
                    top = geometry;
                }
                paletteGeometryMap.insert(palette, geometry);
                ++numGeometries;

            }else{
                geometry = paletteGeometryMap.getValue(pos);
            }

            //インデックス追加
            //三角形の３頂点は同じボーンの組み合わせになる
            u32 triangle = 3*i;
            for(u32 j=0; j<3; ++j){
                geometry->faceIndices_.push_back( faceIndices[triangle + j].index_ );
            }
        }

        LIME_DELETE_ARRAY(combinations);
    }


    void setSkinIndex(Skin& skin, u16* toIndices, u16 from, u16 to)
    {
        for(u32 i=0; i<skin.numVertices_; ++i){
            if(skin.vertices_[i].index_ == from){
                toIndices[i] = to;
            }
        }
    }
}

    void splitBone(
        u32 numVertices,
        Vertex* vertices,
        u32 numFaceIndices,
        FaceIndex* faceIndices,
        u32 numMaterials,
        Material* materials,
        Skin* skins,
        u32 numBones,
        u32 maxBones,
        u32& numGeometries,
        Geometry** geometries)
    {
        LASSERT(geometries != NULL);
        LASSERT(numBones<=MaxBones);
        LASSERT(numVertices<=MaxVertices);
        LASSERT(numFaceIndices<=MaxIndices);


        u32 offset =0;
        Geometry* geometriesInMaterial = NULL;
        u32 numGeometriesInMaterial = 0;

        u32 maxNumIndices = 0;
        Geometry* nextTop = NULL;
        for(u32 i=0; i<numMaterials; ++i){
            splitBone(
                vertices,
                faceIndices + offset,
                i,
                materials[i],
                numBones,
                maxBones,
                numGeometriesInMaterial,
                &geometriesInMaterial);

            if(maxNumIndices<materials[i].faceVertexCount_){
                maxNumIndices = materials[i].faceVertexCount_;
            }

            //新しいリストを後尾に繋げる
            if(nextTop != NULL){
                nextTop->next_ = geometriesInMaterial;
            }else{
                nextTop = geometriesInMaterial;
                *geometries = geometriesInMaterial; //最初なので戻り値設定
            }
            //後尾を探索
            while(nextTop != NULL){
                if(nextTop->next_ == NULL){
                    break;
                }
                nextTop = nextTop->next_;
            }
            numGeometries += numGeometriesInMaterial;

            offset += materials[i].faceVertexCount_;
        }

        typedef lcore::HashMap<VertexKey, u16> VKeyIndexMap;

        Geometry* top = *geometries;

        VertexBuffer::pointer vb( LIME_NEW VertexBuffer(maxNumIndices) );

        VertexKey key;
        VKeyIndexMap indexIndexMap(numVertices*2, key);

        u16* skinIndices = NULL;
        if(skins != NULL){
            Skin& skin = skins[0];
            skinIndices = LIME_NEW u16[skin.numVertices_];
            for(u32 i=0; i<skin.numVertices_; ++i){
                skinIndices[i] = static_cast<u16>( skin.vertices_[i].index_ );
            }
        }

        pmd::Vertex vertex;
        u32 indexOffset = 0;
        while(top != NULL){
            top->vertexBuffer_ = vb;
            top->indexOffset_ = indexOffset;

            if((MaxVertices - vb->elements_.size()) < top->faceIndices_.size()){
                vb = LIME_NEW VertexBuffer(maxNumIndices);
                indexIndexMap.clear();
            }

            for(u32 i=0; i<top->faceIndices_.size(); ++i){
                vertex = vertices[ top->faceIndices_[i] ];
                key.set( top->palette_.get(), top->faceIndices_[i] );

                VKeyIndexMap::size_type pos = indexIndexMap.find( key );
                if(indexIndexMap.isEnd(pos)){
                    //なかったから頂点追加
                    u16 index = static_cast<u16>( vb->elements_.size() );

                    //表情用頂点のインデックスを変換
                    if(skins != NULL){
                        setSkinIndex(skins[0], skinIndices, top->faceIndices_[i], index);
                    }

                    faceIndices[ indexOffset+i ].index_ = index;

                    vertex.element_[ Vertex::Index_Bone0 ] = top->palette_->getGlobalToLocal( vertex.element_[ Vertex::Index_Bone0 ] );
                    vertex.element_[ Vertex::Index_Bone1 ] = top->palette_->getGlobalToLocal( vertex.element_[ Vertex::Index_Bone1 ] );

                    vb->elements_.push_back( vertex );
                    indexIndexMap.insert( key, index );
                }else{
                    u16 index = indexIndexMap.getValue(pos);
                    faceIndices[ indexOffset+i ].index_ = index;
                }
            }

            indexOffset += top->faceIndices_.size();
            top = top->next_;
        }


        if(skins != NULL){
            Skin& skin = skins[0];
            for(u32 i=0; i<skin.numVertices_; ++i){
                skin.vertices_[i].index_ = skinIndices[i];
            }
        }
        LIME_DELETE_ARRAY(skinIndices);

#if defined(LIME_LIB_CONV_DEBUG)
        // debug out
        //-----------------------------------------------------------------------
        lcore::ofstream out("debug.txt", lcore::ios::binary);

        top = *geometries;

        out.print("Num Geometries = %d\n", numGeometries);

        s32 materialIndex = -1;
        while(top != NULL){
            if(materialIndex != top->materialIndex_){
                out.print("------------------\n---- material %d ----\n", top->materialIndex_);
                materialIndex = top->materialIndex_;
            }
            out.print("vertex count: %d\n", top->vertexBuffer_->elements_.size());
            out.print("index count: %d\n", top->faceIndices_.size());

            if(top->palette_){
                out.print("palette bone count: %d\n", top->palette_->size());
                out.print("palette:[");
                for(u32 i=0; i<top->palette_->size(); ++i){
                    out.print("%d, ", static_cast<s32>(top->palette_->get(i)) );
                }
                out.print("]\n");

            }else{
                out.print("palette bone count: 0\n");
            }

            top = top->next_;
        }
#endif
    }
}
