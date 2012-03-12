/**
@file LoaderVmd.cpp
@author t-sakai
@date 2009/11/11
*/
#include <fstream>
#include <string>
#include <map>
#include <list>

#include "LoaderVmd.h"

#include <lcore/lcore.h>
#include <lmath/lmath.h>

#include <lgraphics/scene/Bone.h>
#include <lgraphics/scene/AnimTree.h>
#include <lgraphics/scene/AnimObject.h>
#include <lgraphics/anim/AnimDatabase.h>
#include <lgraphics/anim/AnimDataNode.h>

namespace vmd
{
    using namespace lgraphics;

    namespace
    {
        //------------------------------------------------------------
        //--- VMDファイルフォーマット用構造体
        //------------------------------------------------------------

        /// ヘッダ
        struct VMDData
        {
            typedef std::list<AnimData> AnimDataList;
            typedef std::map<std::string, AnimDataList> NameAnimDataMap;

            VMDData();

            ~VMDData();

            static const int MAGIC_SIZE = 30;
            static const int NAME_SIZE = 20;
            static const int FRAME_NUM_SIZE = sizeof(u32);

            static const int BONE_NAME_SIZE = 15;

            static const char* MAGIC_STRING;

            char magic_[MAGIC_SIZE];
            char name_[NAME_SIZE];

            /// フレームデータ数
            u32 frameNum_;

            /// ボーン名とデータリストのマップ
            NameAnimDataMap _boneAnimDataMap;

            //ストリームロード
            friend std::istream& operator>>(std::istream& is, VMDData& rhs);

            std::ostream& debugOut(std::ostream& os) const;

            // データリストをフレーム番号でソート
            void sortAnimDataList();
        };

        const char* VMDData::MAGIC_STRING = "Vocaloid Motion Data 0002";

        VMDData::VMDData()
        {
        }

        VMDData::~VMDData()
        {
        }


        //ストリームロード
        std::istream& operator>>(std::istream& is, VMDData& rhs)
        {
            // ヘッダロード
            //-----------------------------------------
            is.read(rhs.magic_, VMDData::MAGIC_SIZE);
            rhs.magic_[VMDData::MAGIC_SIZE - 1] = '\0';

            is.read(rhs.name_, VMDData::NAME_SIZE);
            rhs.name_[VMDData::NAME_SIZE - 1] = '\0';

            is.read(reinterpret_cast<char*>(&rhs.frameNum_), VMDData::FRAME_NUM_SIZE);

            //rhs.frameNum_ += 1; //初期フレーム分

            //TODO: ファイル終端チェック
            AnimData tmp;

            static const u32 INTERP_PARAM_NUM = 64;
            char interpParam[INTERP_PARAM_NUM];

            u32 maxFrame = 0;
            char name[VMDData::BONE_NAME_SIZE];

            for(u32 i=0; i<rhs.frameNum_; ++i){
                is.read(name, VMDData::BONE_NAME_SIZE);
                name[VMDData::BONE_NAME_SIZE-1] = '\0';
                //tmp.name_.assign(buffer);

                is.read(reinterpret_cast<char*>(&tmp.frameNo_), sizeof(u32));

                if(maxFrame<=tmp.frameNo_){
                    maxFrame = tmp.frameNo_+1;
                }
                f32 f[7];
                is.read(reinterpret_cast<char*>(f), sizeof(f32)*7);
                tmp.position_.set(f[0], f[1], f[2]);
                tmp.rotation_.set(f[6], f[3], f[4], f[5]);

                is.read(interpParam, INTERP_PARAM_NUM*sizeof(char));


                // ボーンに対応するデータリストがなければ入れる
                VMDData::NameAnimDataMap::iterator itr = rhs._boneAnimDataMap.find( name );

                if(itr == rhs._boneAnimDataMap.end()){
                    rhs._boneAnimDataMap.insert( VMDData::NameAnimDataMap::value_type(name, VMDData::AnimDataList()));
                    itr = rhs._boneAnimDataMap.find( name );
                }

                itr->second.push_back(tmp);
            }

            rhs.sortAnimDataList();
            return is;
        }

        std::ostream& VMDData::debugOut(std::ostream& os) const
        {
            os << magic_ << '\n';
            os << name_ << '\n';
            os << frameNum_ << '\n';

            for(NameAnimDataMap::const_iterator itr = _boneAnimDataMap.begin();
                itr != _boneAnimDataMap.end();
                ++itr)
            {
                os << itr->first.c_str() << '\n';
                const AnimDataList& animList = itr->second;

                for(AnimDataList::const_iterator itr2 = animList.begin();
                    itr2 != animList.end();
                    ++itr2)
                {
                    os << ' ' << (*itr2).frameNo_ << '\n';
                }
            }

            return os;
        }

        // データリストをフレーム番号でソート
        void VMDData::sortAnimDataList()
        {
            struct SortByFrameNo
            {
                bool operator()(const AnimData& d0, const AnimData& d1)
                {
                    return (d0.frameNo_ < d1.frameNo_);
                }
            };

            for(NameAnimDataMap::iterator itr = _boneAnimDataMap.begin();
                itr != _boneAnimDataMap.end();
                ++itr)
            {
                itr->second.sort(SortByFrameNo());
            }
        }
    }

    //------------------------------------------------------------
    /// VMDファイルローダ実体
    class LoaderVmd::Impl
    {
    public:
        Impl()
        {
        }

        ~Impl()
        {
        }

        AnimDatabase* createAnimDB();

        bool readFile(const char* filename);

    private:
        void createTree(
            AnimDataNodeTree& animNode,
            const AnimTree& node,
            AnimDatabase& animDB,
            const AnimObject& object);

        vmd::VMDData _vmdData;
    };

    bool LoaderVmd::Impl::readFile(const char* filename)
    {
        LASSERT(filename != NULL);

        std::ifstream file;
        file.open(filename, std::ios::binary);
        if(file.is_open() == false){
            return false;
        }

        file >> _vmdData;

        file.close();
        return true;
    }

    AnimDatabase* LoaderVmd::Impl::createAnimDB()
    {
        u32 numAnimData = _vmdData.frameNum_;
        u32 numDataNode = _vmdData._boneAnimDataMap.size();
        AnimDatabase *animDB = LIME_NEW AnimDatabase(numAnimData, numDataNode);
        LASSERT(animDB != NULL);

        //データをセット
        u32 dataOffset = 0;
        u32 index = 0;
        for(vmd::VMDData::NameAnimDataMap::iterator itr = _vmdData._boneAnimDataMap.begin();
            itr != _vmdData._boneAnimDataMap.end();
            ++itr)
        {
            // ボーン名セット
            AnimDatabase::Name name(itr->first.c_str());
            animDB->setName(index, name);

            u32 count = 0;
            for(vmd::VMDData::AnimDataList::iterator animItr = itr->second.begin();
                animItr != itr->second.end();
                ++animItr, ++count)
            {
                const AnimData &src = (*animItr);
                AnimData &dst = animDB->getAnimData(dataOffset + count);
                dst = src;
            }

            //ノードにデータ配列セット
            AnimDataNode &dataNode = animDB->getAnimDataNode(index);
            dataNode.setDataBuffer( &(animDB->getAnimData(dataOffset)), count);

            dataOffset += count;
            ++index;
        }

        animDB->createHash();
        return animDB;
    }

    //--------------------------------------------------------
    //---
    //--- LoaderVmd
    //---
    //--------------------------------------------------------
    LoaderVmd::LoaderVmd()
        :impl_(LIME_NEW Impl)
    {
    }

    LoaderVmd::~LoaderVmd()
    {
        LIME_DELETE(impl_);
    }

    bool LoaderVmd::readFile(const char* filename)
    {
        return impl_->readFile(filename);
    }


    lgraphics::AnimDatabase* LoaderVmd::createAnimDB()
    {
        return impl_->createAnimDB();
    }

}
