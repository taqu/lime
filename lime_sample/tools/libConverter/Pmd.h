#ifndef INC_PMD_H__
#define INC_PMD_H__
/**
@file Pmd.h
@author t-sakai
@date 2010/05/16 create
*/
#include <lcore/lcore.h>
#include <lcore/liofwd.h>
#include <lcore/Vector.h>
#include <lcore/smart_ptr/intrusive_ptr.h>
#include <lframework/scene/lscene.h>

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
    static const u8 MaxBones = lscene::MAX_BONES;
    static const u8 BoneFreeSlot = 0xFFU;
    static const u32 MaxIndices = 0xFFFFFFFEU;
    static const u32 MaxVertices = 0xFFFEU;
    static const u16 MaxIKIterations = 15;

    static const u32 NumLimitJoint = 1; ///動き制限するジョイントキーワード数
    extern const Char* LimitJointName[NumLimitJoint]; ///動き制限するジョイントキーワード

    //--------------------------------------
    //---
    //--- Header
    //---
    //--------------------------------------
    struct Header
    {
        static const s32 MagicSize = 3;
        static const s32 CommentSize = 256;

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
        FLOAT position_[3];
        FLOAT normal_[3];
        FLOAT uv_[2];
        BYTE boneNo_[2];
        BYTE boneWeight_;//min:0-max:100
        BYTE edgeFlag_; //0:on, 1:off

        friend lcore::istream& operator>>(lcore::istream& is, Vertex& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Vertex& rhs);


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
        struct BaseVertex
        {
            DWORD index_; //頂点配列のインデックス
            FLOAT position_[3];
        };

        struct OffsetVertex
        {
            DWORD baseIndex_; //BaseVertex配列のインデックス
            FLOAT offset_[3]; //
        };

        union SkinVertex
        {
            BaseVertex base_;
            OffsetVertex offset_;
        };

        Skin();
        ~Skin();

        CHAR name_[NameSize];
        DWORD numVertices_;
        BYTE type_; //0:base, 1:blow, 2:eye, 3:rip, 4:other
        SkinVertex* vertices_;

        //ストリームロード
        friend lcore::istream& operator>>(lcore::istream& is, Skin& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Skin& rhs);


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
        Pack();
        ~Pack();

        bool open(const Char* filepath);

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

        inline u16 getNumSkins() const;
        inline const Skin& getSkin(u32 index) const;


        bool createObject(lscene::AnimObject& obj, const char* directory, bool swapOrigin);

        inline lanim::Skeleton* releaseSkeleton();
        inline lanim::IKPack* releaseIKPack();
    private:
        bool loadInternal();

        void release();

        template<class T, class U>
        void loadElements(lcore::istream& in, T** elements, U& numElements)
        {
            LASSERT(elements != NULL);
            lcore::io::read(in, numElements);
            *elements = LIME_NEW T[numElements];
            T* ptr = *elements;
            for(u32 i=0; i<numElements; ++i){
                in >> ptr[i];
            }
        }

        /// ボーンを親が前になるようにソート
        void sortBones();

        lcore::ifstream input_;

        Header header_;

        u32 numVertices_;
        Vertex *vertices_;

        u32 numFaceIndices_;
        FaceIndex *faceIndices_;

        u32 numMaterials_;
        Material *materials_;

        u32 numGeometries_;
        Geometry* geometries_;

        u16 numBones_;
        Bone *bones_;
        u16* boneMap_;

        u16 numSkins_;
        Skin *skins_;

        lanim::Skeleton* skeleton_;
        lanim::IKPack* ikPack_;

    };

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

    inline u16 Pack::getNumSkins() const
    {
        return numSkins_;
    }

    inline const Skin& Pack::getSkin(u32 index) const
    {
        LASSERT(0<=index && index<numSkins_);
        return skins_[index];
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
}
#endif //INC_PMD_H__
