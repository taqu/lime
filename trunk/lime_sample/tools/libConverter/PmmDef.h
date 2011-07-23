#ifndef INC_PMMDEF_H__
#define INC_PMMDEF_H__
/**
@file PmmDef.h
@author t-sakai
@date 2011/02/06 create
*/
#include "converter.h"

#include <lcore/liostream.h>
#include <lcore/Buffer.h>
#include <lmath/Vector3.h>
#include <lmath/Vector4.h>
#include <lmath/Matrix34.h>
#include <lframework/anim/AnimationClip.h>
#include "Pmd.h"

namespace lscene
{
    class AnimObject;
    class Object;
}

namespace lanim
{
    class Skeleton;
    class IKPack;
    class AnimationControler;
}


namespace pmm
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;

    using lcore::Char;

    using lconverter::StringBuffer;

    static const u32 MagicVersionSize = 24;
    static const u32 UnknownEnvSize = 29;
    static const u32 NameSize = 20;

    //----------------------------------------------------------------------
    //---
    //--- ModelInfo
    //---
    //----------------------------------------------------------------------
    /**
    @brief モデル情報
    @notice ファイル内のメンバの並びとは異なる
    */
    struct ModelInfo
    {
        friend lcore::istream& operator>>(lcore::istream& is, ModelInfo& rhs);

        enum Element
        {
            Element_Unknown =0,
            Element_SelectedIndex,
            Element_Visible,
            Element_NumDisplayGroup,
            Element_Num,
        };

        enum BoneElem
        {
            BoneElem_SelectedBone =0,
            BoneElem_Unknown,
            BoneElem_Num,
        };

        u8 elements_[Element_Num];
        Char name_[24]; //名前
        Char path_[256];//ファイルを保存した環境でのフルパス

        u16 boneElements_[BoneElem_Num];
        u32 blowSkinIndex_;
        u32 lipSkinIndex_;
        u32 eyeSkinIndex_;
        u32 otherSkinIndex_;

        static const u32 MaxDisplayNum = 256;
        static const u32 NumBitArrayBytes = (MaxDisplayNum/8);
        u8 bitIsExpand_[NumBitArrayBytes]; //各表示グループが開いているか

        u32 startFrame_; //?
        u32 endFrame_; //?
    };

    lcore::istream& operator>>(lcore::istream& is, ModelInfo& rhs);


    //----------------------------------------------------------------------
    //---
    //--- JointFrame
    //---
    //----------------------------------------------------------------------
    struct JointFrame
    {
        friend lcore::istream& operator>>(lcore::istream& is, JointFrame& rhs);

        static void readInit(lcore::istream& is, JointFrame& rhs);
        static void readInit2(lcore::istream& is);

        u32 dataID_;
        u32 frameNo_;
        u32 prevDataID_;
        u32 nextDataID_;
        u8 interp_[16];

        lmath::Vector3 position_;
        lmath::Quaternion rotation_;
        u8 selected_;
    };

    lcore::istream& operator>>(lcore::istream& is, JointFrame& rhs);


    //----------------------------------------------------------------------
    //---
    //--- SkinFrame
    //---
    //----------------------------------------------------------------------
    struct SkinFrame
    {
        friend lcore::istream& operator>>(lcore::istream& is, SkinFrame& rhs);

        static void readInit(lcore::istream& is, SkinFrame& rhs);
        static void readInit2(lcore::istream& is);

        u32 dataID_;
        u32 frameNo_;
        u32 prevID_;
        u32 nextID_;
        f32 weight_;
        u8 selected_;
    };

    lcore::istream& operator>>(lcore::istream& is, SkinFrame& rhs);


    //----------------------------------------------------------------------
    //---
    //--- IKFrame
    //---
    //----------------------------------------------------------------------
    /**
    @brief IKフレーム情報
    @notice ファイル内のメンバの並びとは異なる
    */
    struct IKFrame
    {
        static void read(lcore::istream& is, IKFrame& rhs, u32 numIKs);

        static void readInit(lcore::istream& is, IKFrame& rhs, u32 numIKs);
        static void readInit2(lcore::istream& is);

        enum Element
        {
            Element_IsDisp =0,
            Element_Selected,
            Element_Num,
        };

        u32 dataID_;
        u32 frameNo_;
        u32 prevID_;
        u32 nextID_;
        u8 elements_[Element_Num];
        //u8* isIKOn_; //IK個数分
    };


    //----------------------------------------------------------------------
    //---
    //--- CameraFrame
    //---
    //----------------------------------------------------------------------
    struct CameraFrame
    {
        friend lcore::istream& operator>>(lcore::istream& is, CameraFrame& rhs);

        static void readInit(lcore::istream& is, CameraFrame& rhs);
        static void readInit2(lcore::istream& is);

        u32 dataID_;
        u32 frameNo_;
        u32 prevID_;
        u32 nextID_;

        f32 length_;
        f32 center_[3];
        f32 angle_[3];

        u8 interp_[24];

        u8 flag_;
        u32 fov_;
        u8 selected_;
    };

    lcore::istream& operator>>(lcore::istream& is, CameraFrame& rhs);


    //----------------------------------------------------------------------
    //---
    //--- LightFrame
    //---
    //----------------------------------------------------------------------
    struct LightFrame
    {
        friend lcore::istream& operator>>(lcore::istream& is, LightFrame& rhs);

        static void readInit(lcore::istream& is, LightFrame& rhs);
        static void readInit2(lcore::istream& is);

        u32 dataID_;
        u32 frameNo_;
        u32 prevID_;
        u32 nextID_;
        f32 rgb_[3];
        f32 direction_[3];
        u8 selected_;
    };

    lcore::istream& operator>>(lcore::istream& is, LightFrame& rhs);


    //----------------------------------------------------------------------
    //---
    //--- AccessoryInfo
    //---
    //----------------------------------------------------------------------
    struct AccessoryInfo
    {
        static void readName(lcore::istream& is, AccessoryInfo& rhs);

        u8 number_;
        Char name_[100]; //100byteまで
        Char path_[256];
        u8 unknown_; //何かのインデックス
    };

    lcore::istream& operator>>(lcore::istream& is, AccessoryInfo& rhs);


    //----------------------------------------------------------------------
    //---
    //--- AccessoryFrame
    //---
    //----------------------------------------------------------------------
    struct AccessoryFrame
    {
        friend lcore::istream& operator>>(lcore::istream& is, AccessoryFrame& rhs);

        static void readInit(lcore::istream& is, AccessoryFrame& rhs);
        static void readInit2(lcore::istream& is);

        u32 dataID_;
        u32 frameNo_;
        u32 prevID_;
        u32 nextID_;
        u8 isDisp_;
        u32 bindModel_;
        u32 bindBone_;
        f32 translation_[3];
        f32 rotation_[3];
        f32 scale_;
        u8 shadow_;
        u8 selected_;
    };

    lcore::istream& operator>>(lcore::istream& is, AccessoryFrame& rhs);


    //----------------------------------------------------------------------
    //---
    //--- BinarySearchInterface
    //---
    //----------------------------------------------------------------------
    /**
    */
    template<class T>
    class BinarySearchInterface
    {
    public:
        typedef BinarySearchInterface<T> this_type;

        inline void initialize();

        void create(u32 numPoses);

        inline u32 getNumPoses() const;
        inline const T& getPose(u32 index) const;
        inline T& getPose(u32 index);

        u32 binarySearchIndex(u32 frame) const;

        void swap(this_type& rhs);

        /// 次のフレームのインデックスを探索。順方向のみ
        u32 getNextIndex(u32 frame);
    protected:
        BinarySearchInterface();
        ~BinarySearchInterface();

        u32 numPoses_;
        u32 currentIndex_;
        T* poses_;
    };

    template<class T>
    BinarySearchInterface<T>::BinarySearchInterface()
        :numPoses_(0)
        ,currentIndex_(0)
        ,poses_(NULL)
    {
    }

    template<class T>
    BinarySearchInterface<T>::~BinarySearchInterface()
    {
        LIME_DELETE_ARRAY(poses_);
    }

    template<class T>
    inline void BinarySearchInterface<T>::initialize()
    {
        currentIndex_ = 0;
    }

    template<class T>
    void BinarySearchInterface<T>::create(u32 numPoses)
    {
        LIME_DELETE_ARRAY(poses_);
        numPoses_ = numPoses;
        poses_ = LIME_NEW T[numPoses_];
    }

    template<class T>
    inline u32 BinarySearchInterface<T>::getNumPoses() const
    {
        return numPoses_;
    }

    template<class T>
    inline const T& BinarySearchInterface<T>::getPose(u32 index) const
    {
        LASSERT(0<=index && index<numPoses_);
        return poses_[index];
    }

    template<class T>
    inline T& BinarySearchInterface<T>::getPose(u32 index)
    {
        LASSERT(0<=index && index<numPoses_);
        return poses_[index];
    }

    template<class T>
    u32 BinarySearchInterface<T>::binarySearchIndex(u32 frame) const
    {
        LASSERT(0<numPoses_);
        u32 mid = 0;
        u32 left = 0;
        u32 right = numPoses_;
        while(left < right){
            mid = (left + right) >> 1;
            if(poses_[mid].frameNo_ <= frame){
                left = mid + 1;
            }else{
                right = mid;
            }
        }
        LASSERT(0<=left && left<=numPoses_);
        if(left>0){
            left -= 1;
        }
        return left;
    }

    template<class T>
    void BinarySearchInterface<T>::swap(this_type& rhs)
    {
        lcore::swap(numPoses_, rhs.numPoses_);
        lcore::swap(poses_, rhs.poses_);
    }

    // 次のフレームのインデックスを探索。順方向のみ
    template<class T>
    u32 BinarySearchInterface<T>::getNextIndex(u32 frame)
    {
        while(currentIndex_<numPoses_){
            if(frame==poses_[currentIndex_].frameNo_){
                return currentIndex_;
            }else if(frame<poses_[currentIndex_].frameNo_){
                currentIndex_ = (currentIndex_ == 0)? 0 : currentIndex_-1;
                return currentIndex_;
            }
            ++currentIndex_;
        }
        return --currentIndex_;
    }

    //----------------------------------------------------------------------
    //---
    //--- SkinAnimPack
    //---
    //----------------------------------------------------------------------
    struct SkinPose
    {
        u32 frameNo_;
        f32 weight_;
    };

    class SkinAnimPack
    {
    public:
        SkinAnimPack();
        ~SkinAnimPack();

        void create(u32 totalFrames, u32 numSkins);

        inline u32 getNumSkinPoses(u32 index) const;
        inline void setNumSkinPoses(u32 index, u32 numSkinPoses);

        inline void setSkinPoses(u32 index, u32 offset);

        inline SkinPose* getSkinPoses(u32 index);

        f32 searchSkinWeight(u32 index, u32 frame) const;

        void swap(SkinAnimPack& rhs);
    private:
        struct FuncGetFrameNo
        {
            u32 operator()(const SkinPose& pose) const
            {
                return pose.frameNo_;
            }
        };

        u32* numSkinPoses_;
        SkinPose** ptrSkinPoses_;
        SkinPose* skinPoses_;
        lcore::Buffer buffer_;
    };

    inline u32 SkinAnimPack::getNumSkinPoses(u32 index) const
    {
        LASSERT(numSkinPoses_ != NULL);

        return numSkinPoses_[index];
    }

    inline void SkinAnimPack::setNumSkinPoses(u32 index, u32 numSkinPoses)
    {
        numSkinPoses_[index] = numSkinPoses;
    }

    inline void SkinAnimPack::setSkinPoses(u32 index, u32 offset)
    {
        ptrSkinPoses_[index] = skinPoses_ + offset;
    }

    inline SkinPose* SkinAnimPack::getSkinPoses(u32 index)
    {
        return ptrSkinPoses_[index];
    }

    //----------------------------------------------------------------------
    //---
    //--- ModelPack
    //---
    //----------------------------------------------------------------------
    class ModelPack
    {
    public:
        ModelPack();
        ~ModelPack();

        void set(lscene::AnimObject* object);

        void setSkinPack(pmd::SkinPack& skinPack){ skinPack_.swap(skinPack);}
        pmd::SkinPack& getSkinPack(){ return skinPack_;}

        lscene::AnimObject* getObject(){ return object_;}

        u32 getNumJoints() const;

        void setAnimationClip(lanim::AnimationClip::pointer& animClip)
        {
            animClip_ = animClip;
        }

        lanim::AnimationClip::pointer& getAnimationClip(){ return animClip_;}

        void setAnimationControler(lanim::AnimationControler* controler);
        lanim::AnimationControler* getAnimationControler(){ return animControler_;}

        void setSkinAnimPack(SkinAnimPack& skinAnim)
        {
            skinAnimPack_.swap(skinAnim);
        }

        void resetMorph();
        void updateMorph(u32 frame, u32 nextFrame);

        inline const u16* getBoneMap();
        inline void setBoneMap(u16* boneMap);

    private:
        friend class Loader;

        SkinAnimPack& getSkinAnimPack(){ return skinAnimPack_;}

        lscene::AnimObject* object_;
        pmd::SkinPack skinPack_;
        SkinAnimPack skinAnimPack_;

        lanim::AnimationClip::pointer animClip_;
        lanim::AnimationControler* animControler_;

        u16* boneMap_;
    };

    inline const u16* ModelPack::getBoneMap()
    {
        LASSERT(boneMap_ != NULL);
        return boneMap_;
    }
    inline void ModelPack::setBoneMap(u16* boneMap)
    {
        boneMap_ = boneMap;
    }

    //----------------------------------------------------------------------
    //---
    //--- CameraAnimPack
    //---
    //----------------------------------------------------------------------
    struct CameraPose
    {
        u32 frameNo_;
        lmath::Vector3 center_;
        lmath::Vector3 angle_;
        f32 length_;
        f32 fov_;
    };

    class CameraAnimPack : public BinarySearchInterface<CameraPose>
    {
    public:
        CameraAnimPack()
        {}

        ~CameraAnimPack()
        {}

        void set(u32 index, const CameraFrame& frame);
    };


    //----------------------------------------------------------------------
    //---
    //--- LightAnimPack
    //---
    //----------------------------------------------------------------------
    struct LightPose
    {
        u32 frameNo_;
        lmath::Vector3 direction_; //ライト方向
        u8 rgbx_[4]; //RGB+dummy
    };

    class LightAnimPack : public BinarySearchInterface<LightPose>
    {
    public:
        LightAnimPack()
        {}

        ~LightAnimPack()
        {}

        void set(u32 index, const LightFrame& frame);
    };

    //----------------------------------------------------------------------
    //---
    //--- AccessoryPack
    //---
    //----------------------------------------------------------------------
    struct AccessoryPose
    {
        static const u8 GroundID = 0xFFU; //特別な地面モデルID
        enum Element
        {
            Elem_Disp =0,
            Elem_BindModel,
            Elem_BindBone,
            Elem_Shadow,
            Elem_Num,
        };

        u8 getDisp() const{ return elements_[Elem_Disp];}
        u8 getBindModel() const{ return elements_[Elem_BindModel];}
        u8 getBindBone() const{ return elements_[Elem_BindBone];}
        u8 getShadow() const{ return elements_[Elem_Shadow];}

        void setBindBone(u8 boneIndex){ elements_[Elem_BindBone] = boneIndex;}

        u32 frameNo_;
        lmath::Vector3 translation_;
        lmath::Vector3 rotation_;
        f32 scale_;
        u8 elements_[Elem_Num];
    };

    class AccessoryPack : public BinarySearchInterface<AccessoryPose>
    {
    public:
        typedef BinarySearchInterface<AccessoryPose> parent_type;

        AccessoryPack();
        ~AccessoryPack();

        void setObject(lscene::Object* object);

        lscene::Object* getObject(){ return object_;}

        void set(u32 index, const AccessoryFrame& frame);

        bool alphaBlendEnable() const{ return alphaBlendEnable_;}
        void setAlphaBlendEnable(bool enable){ alphaBlendEnable_ = enable;}

        bool isDisp() const{ return isDisp_;}
        void setIsDisp(bool enable){ isDisp_ = enable;}

        void setMatrix(const lmath::Matrix34& matrix){ matrix_ = matrix;}

        void reset(u32 numModels, ModelPack* models);
        void update(u32 frame);
    private:
        lmath::Matrix34 matrix_;
        lscene::Object* object_;
        const lscene::AnimObject* targetObject_;
        const lmath::Matrix34* targetMat_;
        u8 bindBone_;
        bool alphaBlendEnable_;
        bool isDisp_;
    };
}

#endif //INC_PMMDEF_H__
