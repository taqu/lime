#ifndef INC_PMD_H__
#define INC_PMD_H__
/**
@file Pmd.h
@author t-sakai
@date 2010/05/16 create
*/
#include <lcore/liostream.h>
#include <lcore/Vector.h>
#include <lcore/smart_ptr/intrusive_ptr.h>
#include <lgraphics/api/SamplerState.h>
#include <lgraphics/api/VertexBufferRef.h>
#include <lframework/scene/lscene.h>
#include "converter.h"

namespace lmath
{
    class Vector3;
}

namespace lscene
{
    class AnimObject;
}

namespace lanim
{
    class Skeleton;
    class IKPack;
}

namespace pmd
{
    using lcore::Char;
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;

    typedef lcore::Char CHAR;
    typedef u8 BYTE;
    typedef u16 WORD;
    typedef u32 DWORD;
    typedef f32 FLOAT;

    static const u32 NameSize = 20;
    static const u32 FileNameSize = 20;
    static const u32 CommentSize = 256;
    static const u32 LabelNameSize = 50;
    static const u32 ToonTexturePathSize = 100;
    static const u32 FilePathSize = 256;
    static const u32 NumToonTextures = 11;

    static const u8 MaxBones = lscene::MAX_BONES;
    static const u8 BoneFreeSlot = 0xFFU;
    static const u32 MaxIndices = 0xFFFFFFFEU;
    static const u32 MaxVertices = 0xFFFEU;
    static const u16 MaxIKIterations = 8;

    static const u32 NumLimitJoint = 1; ///動き制限するジョイントキーワード数
    extern const Char* LimitJointName[NumLimitJoint]; ///動き制限するジョイントキーワード

    extern const f32 ColorRatio; //色の割合
    extern const f32 AmbientRatio; //環境光の割合

    //--------------------------------------
    //---
    //--- Header
    //---
    //--------------------------------------
    struct Header
    {
        static const s32 MagicSize = 3;

        CHAR magic_[MagicSize];
        FLOAT version_;
        CHAR name_[NameSize];
        CHAR comment_[CommentSize];

        void swap(Header& rhs);

        friend lcore::istream& operator>>(lcore::istream& is, Header& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Header& rhs);


    //--------------------------------------
    //---
    //--- Vertex
    //---
    //--------------------------------------
    struct Vertex
    {
        enum Index
        {
            Index_Bone0 =0,
            Index_Bone1,
            Index_Weight,
            Index_Edge,
            Index_Num,
        };

        FLOAT position_[3];
        s16 normal_[4];//normal (x, y, z) padding
        s16 uv_[2];
        BYTE element_[Index_Num]; //bone0, bone1
                                  //boneWeight min:0-max:100 シェーダで正規化されていないので注意
                                  //edgeFlag 0:on, 1:off

        friend lcore::istream& operator>>(lcore::istream& is, Vertex& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Vertex& rhs);


    //２番目の頂点ストリーム用頂点
    struct VertexStream1
    {
        s16 normal_[4];//normal (x, y, z) padding
        s16 uv_[2];
        BYTE element_[Vertex::Index_Num]; //bone0, bone1
                                  //boneWeight min:0-max:100 シェーダで正規化されていないので注意
                                  //edgeFlag 0:on, 1:off
    };


    //--------------------------------------
    //---
    //--- FaceIndex
    //---
    //--------------------------------------
    struct FaceIndex
    {
        WORD index_;
        friend lcore::istream& operator>>(lcore::istream& is, FaceIndex& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, FaceIndex& rhs);


    //--------------------------------------
    //---
    //--- Material
    //---
    //--------------------------------------
    struct Material
    {
        FLOAT diffuse_[4];
        FLOAT specularity_;
        FLOAT specularColor_[3];
        FLOAT ambient_[3];
        BYTE toonIndex_;
        BYTE edgeFlag_;
        DWORD faceVertexCount_;
        CHAR textureFileName_[FileNameSize];

        friend lcore::istream& operator>>(lcore::istream& is, Material& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Material& rhs);


    //--------------------------------------
    //---
    //--- Bone
    //---
    //--------------------------------------
    struct Bone
    {
        CHAR name_[NameSize];
        WORD parentIndex_; //親のインデックス。無い場合は0xFFFFU
        WORD tailPosIndex_;//末端は0xFFFFU
        BYTE type_; //0:回転、1:回転・移動、2:IK、3:、4:IK影響下、5:回転影響下、6:IK接続先、7:非表示、8:捻り、9:回転運動
        WORD ikParentIndex_; //影響を受けるIKボーン番号。無い場合0
        FLOAT headPos_[3];

        void swap(Bone& rhs);

        Bone& operator=(const Bone& rhs);

        //ストリームロード
        friend lcore::istream& operator>>(lcore::istream& is, Bone& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Bone& rhs);


    //--------------------------------------
    //---
    //--- Palette
    //---
    //--------------------------------------
    struct Palette
    {
        typedef lcore::vector_arena<u8> BoneVector;
        static const u32 DefaultSize = 16;

        typedef smart_ptr::intrusive_ptr<Palette> pointer;

        Palette()
            :refCount_(0)
            ,bones_(DefaultSize)
        {
            lcore::memset(globalToLocal_, BoneFreeSlot, MaxBones);
        }

        ~Palette()
        {
        }

        u32 size() const
        {
            return bones_.size();
        }

        u8 get(u8 index) const
        {
            LASSERT(0<=index && static_cast<u32>(index)<bones_.size());
            return bones_[index];
        }

        void add(u8 boneIndex)
        {
            globalToLocal_[boneIndex] = static_cast<u8>( bones_.size() );
            bones_.push_back(boneIndex);
        }

        void pop()
        {
            if(bones_.size()>0){
                u8 boneIndex = bones_[ bones_.size() - 1 ];
                bones_.pop_back();

                globalToLocal_[boneIndex] = BoneFreeSlot;
            }
        }

        void clear()
        {
            bones_.clear();
            lcore::memset(globalToLocal_, BoneFreeSlot, MaxBones);
        }

        void addRef()
        {
            ++refCount_;
        }
        void release()
        {
            if(--refCount_ == 0){
                LIME_DELETE_NONULL this;
            }
        }

        u8 get(u32 index) const
        {
            LASSERT(0<=index && index<bones_.size());
            return bones_[index];
        }

        u8 getGlobalToLocal(u32 index) const
        {
            LASSERT(0<=index && index<MaxBones);
            return globalToLocal_[index];
        }

        s32 refCount_;
        BoneVector bones_;
        u8 globalToLocal_[MaxBones];
    };

    inline void intrusive_ptr_addref(Palette* ptr)
    {
        ptr->addRef();
    }

    inline void intrusive_ptr_release(Palette* ptr)
    {
        ptr->release();
    }


    //--------------------------------------
    //---
    //--- BufferTemplate
    //---
    //--------------------------------------
    template<class T>
    struct BufferTemplate
    {
        typedef smart_ptr::intrusive_ptr<BufferTemplate<T> > pointer;
        typedef lcore::vector_arena<T> ElementVector;

        BufferTemplate()
            :refCount_(0)
        {
        }

        BufferTemplate(u32 incSize)
            :refCount_(0)
            ,elements_(incSize)
        {
        }

        ~BufferTemplate()
        {
        }

        void addRef()
        {
            ++refCount_;
        }

        void release()
        {
            if(--refCount_ == 0){
                LIME_DELETE_NONULL this;
            }
        }

        s32 refCount_;
        ElementVector elements_;
    };

    template<class T>
    inline void intrusive_ptr_addref(BufferTemplate<T>* ptr)
    {
        ptr->addRef();
    }

    template<class T>
    inline void intrusive_ptr_release(BufferTemplate<T>* ptr)
    {
        ptr->release();
    }

    //--------------------------------------
    //---
    //--- VertexBuffer
    //---
    //--------------------------------------
    typedef BufferTemplate<Vertex> VertexBuffer;

    //--------------------------------------
    //---
    //--- IndexBufer
    //---
    //--------------------------------------
    typedef BufferTemplate<u16> IndexBufer;

    //--------------------------------------
    //---
    //--- IK
    //---
    //--------------------------------------
    struct IK
    {
        IK();
        ~IK();

        WORD boneIndex_; /// IKボーン
        WORD targetBoneIndex_; /// ターゲットボーン
        BYTE chainLength_; /// 子の数
        WORD numIterations_; /// 反復回数
        FLOAT controlWeight_; /// PIに対する倍率？制限角度？
        WORD *childBoneIndex_;

        WORD prevNumChildren_;
        //ストリームロード
        friend lcore::istream& operator>>(lcore::istream& is, IK& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, IK& rhs);


    //--------------------------------------
    //---
    //--- Skin
    //---
    //--------------------------------------
    struct Skin
    {
        struct SkinVertex
        {
            u32 index_; //頂点配列のインデックス
                        //typeがbaseの場合頂点配列のインデックス、それ以外はbaseの配列のインデックス
            f32 position_[3]; //typeがbaseの場合頂点座標、それ以外はbaseからのオフセット
        };

        Skin();
        ~Skin();

        void swap(Skin& rhs);

        //CHAR name_[NameSize];
        u32 numVertices_; //実質頂点は16ビットを越えない
        SkinVertex* vertices_;
        u32 type_; //0:base, 1:blow, 2:eye, 3:rip, 4:other
        //u16 dispIndex_;

        //ストリームロード
        friend lcore::istream& operator>>(lcore::istream& is, Skin& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Skin& rhs);


    //--------------------------------------
    //---
    //--- SkinPack
    //---
    //--------------------------------------
//マルチストリーム実験
#if defined(LIME_LIBCONVERT_PMD_USE_MULTISTREAM)
    class SkinPack
    {
    public:
        SkinPack();
        SkinPack(u32 numSkins);
        ~SkinPack();

        inline u32 getNumSkins() const;

        inline Skin* getSkins();

        inline Skin& getSkin(u32 index);

        void swap(SkinPack& rhs);

        void createMorphBaseVertices(u32 numVertices, const Vertex* vertices, lgraphics::VertexBufferRef& backBuffer);

        u16 getMinIndex() const{ return indexRange_[0];}
        u16 getMaxIndex() const{ return indexRange_[1];}

        lmath::Vector3* getMorphBaseVertices(){ return morphBaseVertices_;}

        lgraphics::VertexBufferRef& getBackVertexBuffer(){ return vb_;}
    private:
        SkinPack(const SkinPack&);
        SkinPack& operator=(SkinPack&);

        void sortVertexIndices();

        struct Batch
        {
            u32 numVertices_;
            lmath::Vector3* vertices_;
        };

        u32 numSkins_;
        Skin* skins_;
        u16 indexRange_[2];
        lmath::Vector3* morphBaseVertices_;

        lgraphics::VertexBufferRef vb_; //ダブルバッファリング用
    };

#else
    class SkinPack
    {
    public:
        SkinPack();
        SkinPack(u32 numSkins);
        ~SkinPack();

        inline u32 getNumSkins() const;

        inline Skin* getSkins();

        inline Skin& getSkin(u32 index);

        void swap(SkinPack& rhs);

        void createMorphBaseVertices(u32 numVertices, const Vertex* vertices);

        u16 getMinIndex() const{ return indexRange_[0];}
        u16 getMaxIndex() const{ return indexRange_[1];}

        Vertex* getMorphBaseVertices(){ return morphBaseVertices_;}
    private:
        SkinPack(const SkinPack&);
        SkinPack& operator=(SkinPack&);

        void sortVertexIndices();

        struct Batch
        {
            u32 numVertices_;
            Vertex* vertices_;
        };

        u32 numSkins_;
        Skin* skins_;
        u16 indexRange_[2];
        Vertex* morphBaseVertices_;
    };
#endif

    inline u32 SkinPack::getNumSkins() const
    {
        return numSkins_;
    }

    inline Skin* SkinPack::getSkins()
    {
        return skins_;
    }

    inline Skin& SkinPack::getSkin(u32 index)
    {
        LASSERT(0<=index && index<numSkins_);
        return skins_[index];
    }


    //--------------------------------------
    //---
    //--- DispLabel
    //---
    //--------------------------------------
    /**
    @brief 表示枠

    表示枠のインデックスは、0がセンター固定。
    */
    class DispLabel
    {
    public:
        struct BoneLabel
        {
            WORD boneIndex_;
            WORD labelIndex_;
        };

        DispLabel();
        ~DispLabel();

        u32 getNumBoneGroups() const{ return numBoneGroups_;}

        u16 getSkinLabelCount() const;
        inline u16 getSkinIndex(u16 labelIndex) const;

        u16 getBoneLabelCount() const;
        inline const BoneLabel& getBoneLabel(u16 labelIndex) const;

        const u16* getBoneMap() const{ return boneMap_;}

        void read(lcore::istream& is);

        void swapBoneMap(u16*& boneMap);

        void swap(DispLabel& rhs);
    private:
        u32 numBoneGroups_;

        u8* buffer_;

        u16* skinIndices_;
        BoneLabel* boneLabels_;

        u16* boneMap_;

    };

    inline u16 DispLabel::getSkinIndex(u16 labelIndex) const
    {
        LASSERT(skinIndices_ != NULL);
        return skinIndices_[labelIndex];
    }

    inline const DispLabel::BoneLabel& DispLabel::getBoneLabel(u16 labelIndex) const
    {
        LASSERT(boneLabels_ != NULL);
        return boneLabels_[labelIndex];
    }

    //--------------------------------------
    //---
    //--- Geometry
    //---
    //--------------------------------------
    struct Geometry
    {
        Geometry();
        ~Geometry();

        Geometry* next_;
        typedef lcore::vector_arena<u16> FaceIndexVector;

        u32 indexOffset_;
        FaceIndexVector faceIndices_;

        s32 materialIndex_;
        Palette::pointer palette_;

        VertexBuffer::pointer vertexBuffer_;
    };


    //--------------------------------------
    //---
    //--- Pack
    //---
    //--------------------------------------
    class Pack
    {
    public:
        enum State
        {
            State_None = 0,
            State_LoadGeometry,
            State_LoadToonTexture,
            State_LoadTexture,
            State_Finish,
            State_Error,
        };

        Pack();
        ~Pack();

        inline void setNameTextureMap(lconverter::NameTextureMap* nameTexMap);

        bool open(const Char* filepath);
        void release();

        inline const Header& getHeader() const;

        inline u32 getNumVertices() const;
        inline const Vertex& getVertex(u32 index) const;

        inline u32 getNumFaceIndices() const;
        inline const FaceIndex& getFaceIndex(u32 index) const;

        inline u32 getNumMaterials() const;
        inline const Material& getMaterial(u32 index) const;

        inline u16 getNumBones() const;
        inline const Bone& getBone(u32 index) const;

        inline u16 getNumIKs() const;
        inline const IK& getIK(u32 index) const;

        inline State getState() const{ return state_;}
        void updateLoad(const char* directory);

        bool createObject(lscene::AnimObject& obj, const char* directory, bool swapOrigin);

        inline lanim::Skeleton* releaseSkeleton();

        inline SkinPack& getSkinPack();

        inline DispLabel& getDispLabel();

    private:

        class ToonTexturePack
        {
        public:
            ToonTexturePack()
            {
                for(u32 i=0; i<NumToonTextures; ++i){
                    textures_[i] = NULL;
                }
            }
            lgraphics::SamplerState samplers_[NumToonTextures];
            lgraphics::TextureRef* textures_[NumToonTextures];
        };

        inline lanim::IKPack* releaseIKPack();
        bool loadInternal(const char* directory);

        template<class T, class U>
        void loadElements(lcore::istream& in, T** elements, U& numElements)
        {
            LASSERT(elements != NULL);
            lcore::io::read(in, numElements);
            if(numElements>0){
                *elements = LIME_NEW T[numElements];
                T* ptr = *elements;
                for(u32 i=0; i<numElements; ++i){
                    in >> ptr[i];
                }
            }
        }

        /// 英語名ロード
        void loadNamesForENG();

        /// トゥーンシェーディング用テクスチャロード
        void loadToonTextures(const char* directory);

        /// ボーンを親が前になるようにソート
        void sortBones();

        void createInternalNameTextureMap();

        State state_;
        lcore::ifstream input_;

        Header header_;

        u32 numVertices_;
        Vertex *vertices_;

        u32 numFaceIndices_;
        FaceIndex *faceIndices_;

        u32 numMaterials_;
        u32 countTextureLoadedMaterial_;
        Material *materials_;

        u32 numGeometries_;
        Geometry* geometries_;

        u16 numBones_;
        Bone *bones_;
        u16* boneMap_;

        SkinPack skinPack_;

        lanim::Skeleton* skeleton_;
        lanim::IKPack* ikPack_;

        lconverter::NameTextureMap *nameTexMap_;
        lconverter::NameTextureMap texMapInternal_;
        DispLabel dispLabel_;

        ToonTexturePack toonTextures_;

//デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
    public:
        lconverter::DebugLog debugLog_;
#endif
    };


    inline void Pack::setNameTextureMap(lconverter::NameTextureMap* nameTexMap)
    {
        LASSERT(nameTexMap != NULL);
        nameTexMap_ = nameTexMap;
    }

    inline const Header& Pack::getHeader() const
    {
        return header_;
    }

    inline u32 Pack::getNumVertices() const
    {
        return numVertices_;
    }

    inline const Vertex& Pack::getVertex(u32 index) const
    {
        LASSERT(0<=index && index<numVertices_);
        return vertices_[index];
    }

    inline u32 Pack::getNumFaceIndices() const
    {
        return numFaceIndices_;
    }

    inline const FaceIndex& Pack::getFaceIndex(u32 index) const
    {
        LASSERT(0<=index && index<numFaceIndices_);
        return faceIndices_[index];
    }

    inline u32 Pack::getNumMaterials() const
    {
        return numMaterials_;
    }

    inline const Material& Pack::getMaterial(u32 index) const
    {
        LASSERT(0<=index && index<numMaterials_);
        return materials_[index];
    }

    inline u16 Pack::getNumBones() const
    {
        return numBones_;
    }

    inline const Bone& Pack::getBone(u32 index) const
    {
        LASSERT(0<=index && index<numBones_);
        return bones_[index];
    }

    inline lanim::Skeleton* Pack::releaseSkeleton()
    {
        lanim::Skeleton* skeleton = skeleton_;
        skeleton_ = NULL;
        return skeleton;
    }

    inline lanim::IKPack* Pack::releaseIKPack()
    {
        lanim::IKPack* ikPack = ikPack_;
        ikPack_ = NULL;
        return ikPack;
    }

    inline SkinPack& Pack::getSkinPack()
    {
        return skinPack_;
    }

    inline DispLabel& Pack::getDispLabel()
    {
        return dispLabel_;
    }
}
#endif //INC_PMD_H__
