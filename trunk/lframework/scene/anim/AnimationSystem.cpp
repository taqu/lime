/**
@file AnimationSystem.cpp
@author t-sakai
@date 2010/11/18 create

*/
#include "AnimationSystem.h"
#include "AnimationControler.h"

namespace lscene
{
namespace lanim
{
    AnimationSystem::AnimationSystem()
    {
    }

    AnimationSystem::~AnimationSystem()
    {
        // 残っているものは破棄
        for(s32 i=0; i<controlers_.size(); ++i){
            LIME_DELETE(controlers_[i]);
        }
    }

    // 初期化
    void AnimationSystem::initialize(const InitParam& param)
    {
        controlers_.setIncSize(param.ctrlVectorIncSize_);
    }

    void AnimationSystem::terminate()
    {
    }

    // 更新
    void AnimationSystem::update()
    {
        for(AnimCtrlVector::iterator itr = controlers_.begin();
            itr != controlers_.end();
            ++itr)
        {
            (*itr)->update();
        }
    }

    // アニメーション登録
    void AnimationSystem::add(AnimationControler* controler)
    {
        controlers_.push_back(controler);
    }

    // アニメーション登録削除
    void AnimationSystem::remove(AnimationControler* controler)
    {
        //線形探索
        for(s32 i=0; i<controlers_.size(); ++i){
            if(controler == controlers_[i]){
                for(s32 j=i+1; j<controlers_.size(); ++j){
                    controlers_[j-1] = controlers_[j];
                }
                controlers_.pop_back();
                break;
            }
        }
    }
}
}
