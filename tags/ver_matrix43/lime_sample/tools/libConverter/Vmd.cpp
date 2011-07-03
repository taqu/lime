/**
@file Vmd.cpp
@author t-sakai
@date 2010/12/30
*/
#include "Vmd.h"

#include <lcore/Vector.h>
#include <lcore/HashMap.h>
#include <lcore/String.h>

#include <lframework/anim/JointPose.h>
#include <lframework/anim/JointAnimation.h>


#include "converter.h"

#if defined(_WIN32) && defined(_DEBUG)
//#define LIME_LIB_CONV_DEBUG (1)
#endif


namespace vmd
{
namespace
{
    typedef lcore::String<JointNameSize> JointName;
    typedef lcore::vector_arena<lanim::JointPoseWithFrame> JointPoseVec;

    struct JointPoseEntry
    {
        JointPoseVec* ptr_;
        JointName name_;
    };


    static const u32 SortStackSize = 32;
    static const u32 SortThreshold = 10;

    struct CompJointFrameOp
    {
        bool operator()(lanim::JointPoseWithFrame* p0, lanim::JointPoseWithFrame* p1) const
        {
            return (p0->frameNo_ < p1->frameNo_);
        }
    };

#if 0
    /**
    @brief 挿入ソート
    */
    void insertionSort(
        u32 numPoses,
        lanim::JointPoseWithFrame** poses)
    {
        lanim::JointPoseWithFrame* pose = NULL;
        u32 j;
        for(u32 i=1; i<numPoses; ++i){
            pose = poses[i];
            for(j=i-1; 0<=j; --j){
                if(poses[j]->frameNo_ <= pose->frameNo_){
                    break;
                }
                poses[j+1] = poses[j];
            }
            poses[j+1] = pose;
        }
    }

    /**
    @brief クイックソート
    */
    void quickSort(
        u32 numPoses,
        lanim::JointPoseWithFrame** poses,
        u32* leftStack,
        u32* rightStack)
    {
        u32 left = 0;
        u32 right = numPoses - 1;
        u32 p = 0;
        u32 i, j;
        lanim::JointPoseWithFrame* pose = NULL;
        lanim::JointPoseWithFrame* tmp = NULL;

        for(;;){
            if((right - left) <= SortThreshold){
                if(p==0){
                    break;
                }
                --p;
                left = leftStack[p];
                right = rightStack[p];
            }

            pose = poses[(left+right)>>1]; //真ん中選ぶ
            i = left;
            j = right;

            for(;;){
                while(poses[i]->frameNo_ < pose->frameNo_){
                    ++i;
                }
                while(pose->frameNo_ < poses[j]->frameNo_){
                    --j;
                }

                if(i>=j){
                    break;
                }
                tmp = poses[i];
                poses[i] = poses[j];
                poses[j] = tmp;
                ++i;
                --j;
            }

            u32 leftNum = (i-left);
            u32 rightNum = (right-j);
            if(leftNum>rightNum){
                if(leftNum>SortThreshold){
                    leftStack[p] = left;
                    rightStack[p] = i-1;
                    ++p;
                }
                left = j + 1;
            }else{
                if(rightNum>SortThreshold){
                    leftStack[p] = j + 1;
                    rightStack[p] = right;
                    ++p;
                }
                right = i-1;
            }
        }

        insertionSort(numPoses, poses);
    }
#endif
}

    //--------------------------------------
    //---
    //--- Header
    //---
    //--------------------------------------
    //ストリームロード
    lcore::istream& operator>>(lcore::istream& is, Header& rhs)
    {
        is.read(rhs.magic_, Header::MagicSize);
        rhs.magic_[Header::MagicSize - 1] = '\0';

        is.read(rhs.name_, NameSize);
        rhs.name_[NameSize - 1] = '\0';

        lcore::io::read(is, rhs.frameNum_);

        return is;
    }

    //--------------------------------------
    //---
    //--- Frame
    //---
    //--------------------------------------
    //ストリームロード
    lcore::istream& operator>>(lcore::istream& is, Frame& rhs)
    {
        is.read(rhs.name_, BoneNameSize);

        f32 tmp[4];
        lcore::io::read(is, rhs.frameNo_);
        lcore::io::read(is, rhs.position_, sizeof(f32)*3);
        lcore::io::read(is, tmp, sizeof(f32)*4);
        rhs.rotation_[0] = tmp[3];
        rhs.rotation_[1] = tmp[0];
        rhs.rotation_[2] = tmp[1];
        rhs.rotation_[3] = tmp[2];
        lcore::io::read(is, rhs.interp_, sizeof(f32)*16);

        return is;
    }


    //--------------------------------------
    //---
    //--- Pack
    //---
    //--------------------------------------
    lanim::AnimationClip::pointer Pack::loadFromFile(const Char* filepath)
    {
        LASSERT(filepath != NULL);

        lanim::AnimationClip::pointer clip;

        LASSERT(filepath != NULL);
        lcore::ifstream in(filepath, lcore::ios::binary|lcore::ios::in);
        if(in.is_open()){
            loadInternal(clip, in);
        }else{
            lcore::Log("vmd fail to open file");
        }

        return clip;
    }

    lanim::AnimationClip::pointer Pack::loadFromMemory(const u8* data, u32 size)
    {
        LASSERT(data != NULL);
        lcore::isstream in(reinterpret_cast<const char*>(data), size);

        lanim::AnimationClip::pointer clip;
        loadInternal(clip, in);
        return clip;
    }

    void Pack::loadInternal(lanim::AnimationClip::pointer& clip, lcore::istream& is)
    {

        typedef lcore::vector_arena<JointPoseEntry> JointAnimPtrVec;

        typedef lcore::HashMap<JointName, u32> JointNameIndexMap;

        //lcore::Log("vmd start to load");

        //ヘッダーロード
        Header header;
        is >> header;

        //lcore::Log("vmd load header ok");

        // アニメーションロード。ついでに後処理のソートバッファのサイズも計算
        u32 sortBufferSize = 0;
        JointAnimPtrVec jointAnimPtrVec;
        {

            Frame frame;
            JointPoseEntry jointEntry;
            lanim::JointPoseWithFrame jointPose;
            JointNameIndexMap jointNameIndexMap(256, jointEntry.name_);
            u32 index = 0;

            for(u32 i=0; i<header.frameNum_; ++i){
                is >> frame;
                jointEntry.name_.assignMemory(frame.name_, BoneNameSize);
                JointNameIndexMap::size_type pos = jointNameIndexMap.find(jointEntry.name_);

                if(pos == jointNameIndexMap.end()){
                    //なければ追加
                    index = jointAnimPtrVec.size();
                    jointNameIndexMap.insert(jointEntry.name_, index);

                    jointEntry.ptr_ = LIME_NEW JointPoseVec;
                    jointAnimPtrVec.push_back(jointEntry);
                }else{
                    index = jointNameIndexMap.getValue(pos);
                }

                jointPose.frameNo_ = frame.frameNo_;
                jointPose.translation_.set(frame.position_[0], frame.position_[1], frame.position_[2]);
                jointPose.rotation_.set(frame.rotation_[0], frame.rotation_[1], frame.rotation_[2], frame.rotation_[3]);

                JointPoseVec* ptr = jointAnimPtrVec[index].ptr_;
                ptr->push_back(jointPose);
                if(ptr->size() == ptr->capacity()){
                    ptr->setIncSize(ptr->size());
                    if(sortBufferSize<ptr->size()){
                        sortBufferSize = ptr->size();
                    }
                }
            }

        }

        //lcore::Log("vmd load anim ok");

#if defined(LIME_LIB_CONV_DEBUG)
        //NaNかどうかチェックする
        for(JointAnimPtrVec::iterator i = jointAnimPtrVec.begin();
            i != jointAnimPtrVec.end();
            ++i)
        {
            for(JointPoseVec::iterator j = i->ptr_->begin();
                j != i->ptr_->end();
                ++j)
            {

                if(j->rotation_.isNan()){
                    LASSERT(false);
                }
                if(j->translation_.isNan()){
                    LASSERT(false);
                }
            }
        }
#endif
        

        lanim::AnimationClip *animClip = LIME_NEW lanim::AnimationClip(jointAnimPtrVec.size());

        //各フレーム配列をソート
        u32 lastFrame = 0;
        {
            typedef lcore::vector_arena<lanim::JointPoseWithFrame*> SortBuffer;
            SortBuffer sortBuffer(sortBufferSize>>1);
            sortBuffer.reserve(sortBufferSize);

            for(u32 i=0; i<jointAnimPtrVec.size(); ++i){
                lanim::JointAnimation& jointAnim = animClip->getJointAnimation(i);
                JointPoseVec* ptr = jointAnimPtrVec[i].ptr_;

                jointAnim.setNumPoses(ptr->size());
                jointAnim.setName( jointAnimPtrVec[i].name_.c_str(), jointAnimPtrVec[i].name_.size() );

                //配列内をソートする
                sortBuffer.clear();
                for(u32 j=0; j<jointAnim.getNumPoses(); ++j){
                    sortBuffer.push_back( &((*ptr)[j]) );
                }
                //quickSort(jointAnim.getNumPoses(), &(sortBuffer[0]), leftStack, rightStack);

                lconverter::quickSort<lanim::JointPoseWithFrame*, u32, CompJointFrameOp, 10, 32>
                    (jointAnim.getNumPoses(), &(sortBuffer[0]), CompJointFrameOp());
#if defined(LIME_LIB_CONV_DEBUG)
                for(u32 j=1; j<jointAnim.getNumPoses(); ++j){
                    if(sortBuffer[j-1]->frameNo_ > sortBuffer[j]->frameNo_){
                        LASSERT(false);
                    }
                    for(u32 k=0; k<j; ++k){
                        if(sortBuffer[k] == sortBuffer[j]){
                            LASSERT(false);
                        }
                    }
                }
#endif

                for(u32 j=0; j<jointAnim.getNumPoses(); ++j){
                    jointAnim.setPose(j, *(sortBuffer[j]));
                }

                if(sortBuffer[ sortBuffer.size() - 1 ]->frameNo_ > lastFrame){
                    lastFrame = sortBuffer[ sortBuffer.size() - 1 ]->frameNo_;
                }

                LIME_DELETE(jointAnimPtrVec[i].ptr_ );
            }
        }
        //lcore::Log("vmd sort anim ok");

        animClip->setLastFrame(static_cast<f32>(lastFrame));
        animClip->setNameMemory(header.name_, NameSize);

        clip = animClip;
        //lcore::Log("vmd end to load");
    }
}
