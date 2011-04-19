#ifndef INC_PMM_CPP__
#define INC_PMM_CPP__
/**
@file Pmm.cpp
@author t-sakai
@date 2011/02/05 create
*/
#include "Pmm.h"
#include <lcore/utility.h>
#include "Pmd.h"
#include "XLoader.h"

#include <lframework/scene/AnimObject.h>
#include <lframework/scene/Object.h>
#include <lframework/anim/Skeleton.h>
#include <lframework/anim/Joint.h>
#include <lframework/anim/IKPack.h>

#include <lframework/anim/JointPose.h>
#include <lframework/anim/JointAnimation.h>

#include <lgraphics/api/TextureRef.h>

using namespace lcore;
using namespace lconverter;

#if defined(LIME_ENABLE_LOG)
#define LCONVERTER_PMM_DEBUG_LOG (1)
#endif

namespace pmm
{
    //----------------------------------------------
    //---
    //---
    //---
    //----------------------------------------------
#define LPMM_CHECK_READ(file, obj, code) \
    if(0 == io::read((file), (obj))){ errorCode_ = code;}

#define LPMM_CHECK_READ_BUFF(file, buffer, size, code) \
    if(0 == io::read((file), (buffer), (size))){ errorCode_ = code;}

#define LPMM_CHECK_READ_RETURN(file, obj, code) \
    if(0 == io::read((file), (obj))){ errorCode_ = code; return;}

#define LPMM_CHECK_READ_BUFF_RETURN(file, buffer, size, code) \
    if(0 == io::read((file), (buffer), (size))){ errorCode_ = code; return;}

    //----------------------------------------------
    //---
    //--- Loader
    //---
    //----------------------------------------------
    // Xファイルのためのスケーリング
    const f32 Loader::XToPMMScaling = 10.0f;


    template<class PACK_TYPE, class FRAME_TYPE>
    inline void Loader::loadAnim(PACK_TYPE& pack, bool convert)
    {
        FRAME_TYPE frame;
        FRAME_TYPE::readInit(file_, frame);
        u32 numFrames = 0;
        io::read(file_, numFrames);
        numFrames += 1;

        buffer_.resize( sizeof(FRAME_TYPE) * numFrames );
        FRAME_TYPE *frames = buffer_.get<FRAME_TYPE>(0);

        frames[0] = frame;

        for(u32 i=1; i<numFrames; ++i){
            file_ >> frames[i];
        }

        if(convert){
            pack.create(numFrames);

            u32 next = 0;
            u32 index = 0;
            do{
                pack.set(index, frames[next]);

                ++index;
                next = frames[next].nextID_;
                if(next == 0){
                    break;
                }
            }while(next<numFrames);
        }

        FRAME_TYPE::readInit2(file_);
    }


    Loader::Loader()
        :errorCode_(Error_None)
        ,filename_(NULL)
        ,directory_(NULL)
        ,startFrame_(0)
        ,endFrame_(0)
        ,numModels_(0)
        ,modelPacks_(NULL)
        ,numAccessories_(0)
        ,accessoryPacks_(NULL)
    {
    }

    //----------------------------------------------
    Loader::~Loader()
    {
        for(NameTextureMap::size_type pos = nameTexMap_.begin();
            pos != nameTexMap_.end();
            pos = nameTexMap_.next(pos))
        {
            lgraphics::TextureRef *tex = nameTexMap_.getValue(pos);
            LIME_DELETE(tex);
        }

        LIME_DELETE_ARRAY(modelPacks_);
        LIME_DELETE_ARRAY(accessoryPacks_);
    }

    //----------------------------------------------
    void Loader::load(const Char* filename, const Char* directory)
    {
        LASSERT(filename != NULL);
        filename_ = filename;
        directory_ = directory;
        
        //デバッグ情報初期化
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        debugLog_.reset();
#endif

        lenFilename_ = lcore::strlen(filename_);
        {
            StringBuffer filepath;
            if(directory_ != NULL){
                lenDirectory_ = lcore::strlen(directory_);
                filepath.push_back(directory, lenDirectory_);
                filepath.push_back(filename_, lenFilename_);
            }else{
                lenDirectory_= 0;
                filepath.push_back(filename_, lenFilename_);
            }

            file_.open(filepath.c_str(), lcore::ios::binary);
            if(false == file_.is_open()){
                errorCode_ = Error_FileOpen;
                return;
            }
        }

        readMagicVersion();
        readEnvironment();

        u8 numModels = 0;
        if(0 == io::read(file_, numModels)){
            errorCode_ = Error_Read;
            file_.close();
            return;
        }
        numModels_ = numModels;

        lcore::Log("pmm: %d models", numModels_);

        LIME_DELETE_ARRAY(modelPacks_);
        modelPacks_ = LIME_NEW ModelPack[numModels_];

        readModelNames();
        if(errorCode_ != Error_None){
            return;
        }

        readModels();
        if(errorCode_ != Error_None){
            return;
        }

        readCamera();
        if(errorCode_ != Error_None){
            return;
        }

        readLight();
        if(errorCode_ != Error_None){
            return;
        }

        readAccessory();

        file_.close();
    }

    //----------------------------------------------
    void Loader::readMagicVersion()
    {
        Char buffer[MagicVersionSize];
        LPMM_CHECK_READ_BUFF(file_, buffer, MagicVersionSize, Error_Read);
    }

    //----------------------------------------------
    void Loader::readEnvironment()
    {
        Char buffer[UnknownEnvSize];
        LPMM_CHECK_READ_BUFF(file_, buffer, UnknownEnvSize, Error_Read);
    }

    //----------------------------------------------
    void Loader::readModelNames()
    {
        Char name[NameSize];
        for(u32 i=0; i<numModels_; ++i){
            LPMM_CHECK_READ_BUFF_RETURN(file_, name, NameSize, Error_Read);
        }
    }

    //----------------------------------------------
    void Loader::readModels()
    {
        ModelInfo modelInfo;


        u32 numIKFrames = 0;
        IKFrame ikFrame;

        pmd::Pack pmdPack;
        StringBuffer path;

        pmd::DispLabel dispLabel;

        for(u32 i=0; i<numModels_; ++i){
            file_ >> modelInfo;

            //PMDロード
            loadPmd(i, modelInfo, dispLabel);
            if(errorCode_ != Error_None){
                return;
            }

            //フレームロード
            readJointPoses(i, dispLabel);

            modelPacks_[i].getAnimationClip()->setLastFrame( static_cast<f32>(modelInfo.endFrame_) );
            if(endFrame_<modelInfo.endFrame_){
                endFrame_ = modelInfo.endFrame_;
            }

            readSkinMorphPoses(i);

            u32 numIKs = (modelPacks_[i].getObject()->getIKPack())? modelPacks_[i].getObject()->getIKPack()->getNumIKs() : 0;
            IKFrame::readInit(file_, ikFrame, numIKs);

            io::read(file_, numIKFrames);
            for(u32 j=0; j<numIKFrames; ++j){
                IKFrame::read(file_, ikFrame, numIKs);
            }

            //以下エディタ上の現在値
            u32 numJoints = modelPacks_[i].getNumJoints();
            for(u32 j=0; j<numJoints; ++j){
                JointFrame::readInit2(file_);
            }

            u32 numSkins = modelPacks_[i].getSkinPack().getNumSkins();
            for(u32 j=0; j<numSkins; ++j){
                SkinFrame::readInit2(file_);
            }
            for(u32 j=0; j<numIKs; ++j){
                IKFrame::readInit2(file_);
            }

            u16* boneMap = NULL;
            dispLabel.swapBoneMap(boneMap);
            modelPacks_[i].setBoneMap(boneMap);
        }
    }


    //----------------------------------------------
    // PMDロード
    void Loader::loadPmd(u32 index, ModelInfo& modelInfo, pmd::DispLabel& dispLabel)
    {
        static const Char ModelDirectory[] = "Model/";
        static const u32 LenModelDir = sizeof(ModelDirectory) - 1;

        ModelPack& modelPack = modelPacks_[index];

        lscene::AnimObject* object = NULL;
        pmd::Pack pmdPack(&nameTexMap_);

        {//ファイルオープン
            u32 dlen = lenDirectory_ + LenModelDir;
            Char* path = createPath(modelInfo.path_, ModelDirectory, LenModelDir);


            if(false == pmdPack.open(path)){
                //ModelディレクトリになければPMMファイルと同じディレクトリがあるか調べる
                path = createPath(modelInfo.path_);

                if(false == pmdPack.open(path)){
                    errorCode_ = Error_ReadPmd;
                    lcore::Log("fail to load %s", modelInfo.path_);
                    return;
                }

                dlen = lenDirectory_;
            }


            object = LIME_NEW lscene::AnimObject;
            if(object == NULL){
                errorCode_ = Error_ReadPmd;
                return;
            }

            if(dlen <= 0){
                buffer_.resize(1);
            }
            Char* directory = buffer_.get<Char>(0);

            directory[dlen] = '\0'; //ディレクトリパスを作成
            if(false == pmdPack.createObject(*object, directory, false)){
                LIME_DELETE(object);
                errorCode_ = Error_ReadPmd;
                return;
            }
            pmdPack.getDispLabel().swap(dispLabel);

            //デバッグ情報追加
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
            debugLog_ += pmdPack.debugLog_;
#endif
        }

        modelPack.set(object);
        modelPack.setSkinPack(pmdPack.getSkinPack());
    }


    //----------------------------------------------
    void Loader::readJointPoses(u32 index, pmd::DispLabel& dispLabel)
    {
        ModelPack& modelPack = modelPacks_[index];
        u32 numJoints = modelPack.getNumJoints();
        if(numJoints<=0){
            return;
        }

        u32 numJointPoses = 0;
        JointFrame frame;
        //初期フレームを飛ばしてフレーム数ロード
        {
            s32 initFrameSize = sizeof(u32)*3 + 16 + sizeof(f32)*7 + 1;
            s32 offset = initFrameSize * numJoints;
            file_.seekg(offset, lcore::ios::cur);

            //フレーム数
            io::read(file_, numJointPoses);

            offset += sizeof(u32);
            file_.seekg(-offset, lcore::ios::cur);
        }

        //ロード用バッファ作成
        u32* numFrames = NULL;
        JointFrame* jointFrames = NULL;
        {
            u32 offset = sizeof(u32) * numJoints;
            u32 size = offset + (numJointPoses + numJoints) * sizeof(JointFrame);

            buffer_.resize(size);
            numFrames = buffer_.get<u32>(0);

            jointFrames = buffer_.get<JointFrame>(offset);
        }

        //初期フレームロード
        for(u32 i=0; i<numJoints; ++i){
            JointFrame::readInit(file_, jointFrames[i]);
        }
        //
        io::read(file_, numJointPoses);

        //フレームロード
        for(u32 i=0; i<numJointPoses; ++i){
            file_ >> jointFrames[i + numJoints];
        }

        //各ジョイント（ボーン）のフレーム数を数える
        u32 totalJointFrames = numJoints + numJointPoses;
        u32 next;
        for(u32 i=0; i<numJoints; ++i){
            numFrames[i] = 1;
            next = jointFrames[i].nextDataID_;
            while(next<totalJointFrames){
                if(next == 0){
                    break;
                }
                ++numFrames[i];
                next = jointFrames[next].nextDataID_;
            }
        }


        //アニメーションクリップ作成
        lanim::Skeleton::pointer& skeleton = modelPack.getObject()->getSkeleton();

        const u16* boneMap = dispLabel.getBoneMap();
        LASSERT(boneMap != NULL);

        u32 poseIndex = 0;
        lanim::AnimationClip::pointer animClip( LIME_NEW lanim::AnimationClip(numJoints) );
        for(u32 i=0; i<numJoints; ++i){
            lanim::JointAnimation& jointAnim = animClip->getJointAnimation(i);

            //const lanim::Name& name = skeleton->getJointName( boneMap[i] );
            //jointAnim.setName( name );
            jointAnim.setNumPoses(numFrames[i]);

            next = i;
            poseIndex = 0;
            do{
                lanim::JointPoseWithFrame& jointPose = jointAnim.getPose(poseIndex);
                jointPose.frameNo_ = jointFrames[next].frameNo_;
                jointPose.translation_ = jointFrames[next].position_;
                jointPose.rotation_ = jointFrames[next].rotation_;

                ++poseIndex;
                next = jointFrames[next].nextDataID_;
                if(next == 0){
                    break;
                }
            }while(next<totalJointFrames);
        }

        //名前セット
        if(numJoints>0){
            animClip->getJointAnimation(0).setName(  skeleton->getJointName( boneMap[0] ) ); //センターは0で固定

            u16 numBoneLabels = dispLabel.getBoneLabelCount();
            for(u16 i=0; i<numBoneLabels; ++i){
                const pmd::DispLabel::BoneLabel& label = dispLabel.getBoneLabel(i);
                lanim::JointAnimation& jointAnim = animClip->getJointAnimation(label.boneIndex_);

                const lanim::Name& name = skeleton->getJointName( boneMap[label.boneIndex_] );
                jointAnim.setName( name );
            }
        }

        modelPack.setAnimationClip(animClip);
    }

    //----------------------------------------------
    // モデルのジョイントアニメーションロード
    void Loader::readSkinMorphPoses(u32 index)
    {
        ModelPack& modelPack = modelPacks_[index];
        u32 numSkins = modelPack.getSkinPack().getNumSkins();

        u32 numSkinPoses = 0;
        //初期フレームを飛ばしてフレーム数ロード
        {
            s32 initFrameSize = sizeof(u32)*3 + sizeof(f32) + 1;
            s32 offset = initFrameSize * numSkins;
            file_.seekg(offset, lcore::ios::cur);

            //フレーム数
            io::read(file_, numSkinPoses);

            if(numSkinPoses<=0){
                return;
            }

            offset += sizeof(u32);
            file_.seekg(-offset, lcore::ios::cur);
        }

        //ロード用バッファ作成
        u32* numFrames = NULL;
        SkinFrame* skinFrames = NULL;
        {
            u32 offset = sizeof(u32) * numSkins;
            u32 size = offset + (numSkinPoses + numSkins) * sizeof(SkinFrame);

            buffer_.resize(size);
            numFrames = buffer_.get<u32>(0);

            skinFrames = buffer_.get<SkinFrame>(offset);
        }

        //初期フレームロード
        for(u32 i=0; i<numSkins; ++i){
            SkinFrame::readInit(file_, skinFrames[i]);
        }
        //
        io::read(file_, numSkinPoses);

        //フレームロード
        for(u32 i=0; i<numSkinPoses; ++i){
            file_ >> skinFrames[i + numSkins];
        }

        //各表情のフレーム数を数える
        u32 totalSkinFrames = numSkins + numSkinPoses;
        u32 next;
        for(u32 i=0; i<numSkins; ++i){
            numFrames[i] = 1;
            next = skinFrames[i].nextID_;
            while(next<totalSkinFrames){
                if(next == 0){
                    break;
                }
                ++numFrames[i];
                next = skinFrames[next].nextID_;
            }
        }
        //表情アニメーション作成
        SkinAnimPack& skinAnimPack = modelPack.getSkinAnimPack();
        skinAnimPack.create(totalSkinFrames, numSkins);

        u32 offset = 0;
        u32 frameIndex = 0;
        pmm::SkinPose* poses = NULL;
        for(u32 i=0; i<numSkins; ++i){
            skinAnimPack.setNumSkinPoses(i, numFrames[i]);
            skinAnimPack.setSkinPoses(i, offset);
            offset += numFrames[i];

            poses = skinAnimPack.getSkinPoses(i);
            next = i;
            frameIndex = 0;
            do{

                poses[frameIndex].frameNo_ = skinFrames[next].frameNo_;
                poses[frameIndex].weight_ = skinFrames[next].weight_;

                ++frameIndex;
                next = skinFrames[next].nextID_;
                if(next == 0){
                    break;
                }
            }while(next < totalSkinFrames);
        }
    }

    //----------------------------------------------
    // カメラアニメーションロード
    void Loader::readCamera()
    {
        loadAnim<CameraAnimPack, CameraFrame>(cameraAnimPack_, true);
    }

    //----------------------------------------------
    // ライトアニメーションロード
    void Loader::readLight()
    {
        loadAnim<LightAnimPack, LightFrame>(lightAnimPack_, true);
    }

    //----------------------------------------------
    // アクセサリ、アニメーションロード
    void Loader::readAccessory()
    {
        file_.seekg(5, lcore::ios::cur); //unknown 5 bytes
        u8 numAccessories = 0;
        io::read(file_, numAccessories);

        LIME_DELETE_ARRAY(accessoryPacks_);
        accessoryPacks_ = LIME_NEW AccessoryPack[numAccessories];

        AccessoryInfo accInfo;

        for(u8 i=0; i<numAccessories; ++i){
            AccessoryInfo::readName(file_, accInfo);
        }

        lmath::Matrix43 matrix;
        lx::XLoader xloader;
        u8 index = 0;
        for(u8 i=0; i<numAccessories; ++i){
            file_ >> accInfo;

            AccessoryPack& accPack = accessoryPacks_[index];

            if(checkVac(accInfo, matrix)){
                accPack.setMatrix(matrix);
            }

            bool isLoaded = loadX(index, xloader, accInfo);
            xloader.release();

            loadAnim<AccessoryPack, AccessoryFrame>(accPack, isLoaded);

            if(isLoaded){
                const AccessoryPose& pose = accPack.getPose(0);
                accPack.setIsDisp(pose.getDisp() == 1);

                ++index;
            }

            for(u32 i=0; i<accPack.getNumPoses(); ++i){
                AccessoryPose& pose = accPack.getPose(i);

                if(pose.getBindModel() < numModels_){
                    const u16* boneMap = modelPacks_[ pose.getBindModel() ].getBoneMap();
                    u8 boneIndex = pose.getBindBone();
                    pose.setBindBone( boneMap[boneIndex] );
                }
            }

        }
        numAccessories_ = index;
    }

    bool Loader::loadX(u32 index, lx::XLoader& xloader, const AccessoryInfo& info)
    {
        static const Char AccessoryDirectory[] = "Accessory/";
        static const u32 LenAccessoryDir = sizeof(AccessoryDirectory) - 1;

        LASSERT(accessoryPacks_ != NULL);

        u32 dlen = lenDirectory_ + LenAccessoryDir;
        Char* path = createPath(info.path_, AccessoryDirectory, LenAccessoryDir);

        if(false == xloader.open(path)){
            //AccessoryディレクトリになければPMMファイルと同じディレクトリがあるか調べる
            path = createPath(info.path_);

            if(false == xloader.open(path)){
                lcore::Log("loadx: fail to load %s", info.path_);
                return false;
            }

            dlen = lenDirectory_;
        }

        lscene::Object* object = LIME_NEW lscene::Object;
        if(object == NULL){
            return false;
        }

        if(dlen <= 0){
            buffer_.resize(1);
        }
        Char* directory = buffer_.get<Char>(0);

        directory[dlen] = '\0'; //ディレクトリパスを作成
        xloader.load(*object, directory, XToPMMScaling, false);
        xloader.close();

        if(lx::XLoader::Error_None != xloader.getErrorCode()){
            LIME_DELETE(object);
            lcore::Log("fail to load %s", info.path_);
            return false;
        }

        //デバッグ情報追加
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        debugLog_ += xloader.debugLog_;
#endif

        accessoryPacks_[index].setObject(object);
        return true;
    }


    Char* Loader::createPath(const Char* filename)
    {
        LASSERT(filename != NULL);

        u32 lenFilename = lcore::strlen(filename);
        u32 size = lenDirectory_ + lenFilename + 1;
        buffer_.resize(size);
        Char* path = buffer_.get<Char>(0);
        if(lenDirectory_>0){
            lcore::memcpy(path, directory_, lenDirectory_);
            path += lenDirectory_;
        }
        lcore::memcpy(path, filename, lenFilename);
        path[lenFilename] = '\0';
        return buffer_.get<Char>(0);
    }

    Char* Loader::createPath(const Char* filename, const Char* directory, const u32 lenDir)
    {
        LASSERT(filename != NULL);
        LASSERT(directory != NULL);

        u32 lenFilename = lcore::strlen(filename);
        u32 size = lenDirectory_ + lenDir + lenFilename + 1;
        buffer_.resize(size);
        Char* path = buffer_.get<Char>(0);
        if(lenDirectory_>0){
            lcore::memcpy(path, directory_, lenDirectory_);
            path += lenDirectory_;
        }
        lcore::memcpy(path, directory, lenDir);
        path += lenDir;

        lcore::memcpy(path, filename, lenFilename);
        path[lenFilename] = '\0';
        return buffer_.get<Char>(0);
    }


    Loader::Format Loader::getFormat(const Char* filename) const
    {
        LASSERT(filename != NULL);
        u32 lenFilename = lcore::strlen(filename);
        const Char* ext = lcore::rFindChr(filename, '.', lenFilename);
        if(NULL == ext){//.が見つからない場合はＸファイルを仮定
            return Format_X;
        }
        ++ext;
        if(lcore::strncmp(ext, "x", 1) == 0){
            return Format_X;
        }else if(lcore::strncmp(ext, "vac", 3) == 0){
            return Format_VAC;
        }
        return Format_X;
    }


    // VACファイルかチェックし、そうならばパラメータロード
    bool Loader::checkVac(AccessoryInfo& accInfo, lmath::Matrix43& matrix)
    {
        static const Char AccessoryDirectory[] = "Accessory/";
        static const u32 LenAccessoryDir = sizeof(AccessoryDirectory) - 1;

        if(NULL == lcore::strstr(accInfo.path_, ".vac")){
            return false;
        }

        Char* path = createPath(accInfo.path_, AccessoryDirectory, LenAccessoryDir);

        lcore::ifstream file(path, lcore::ios::binary);
        if(false == file.is_open()){
            //AccessoryディレクトリになければPMMファイルと同じディレクトリがあるか調べる
            path = createPath(accInfo.path_);

            if(false == file.open(path, lcore::ios::binary)){
                lcore::Log("fail to load %s", accInfo.path_);
                return false;
            }
        }

        Char* lines[7];

        file.seekg(0, lcore::ios::end);
        u32 size = file.tellg();
        file.seekg(0, lcore::ios::beg);

        buffer_.resize(size+1);
        Char* buffer = buffer_.get<Char>(0);
        file.read(buffer, size);
        buffer[size] = '\0';

        u32 lineCount = 0;
        lines[lineCount] = buffer;
        for(u32 i=0; i<size; ++i){
            if(buffer[i] == 0x0DU){
                buffer[i] = '\0';
            }else if(buffer[i] == 0x0AU){
                buffer[i] = '\0';
                ++lineCount;
                lines[lineCount] = &(buffer[i+1]);
                if(lineCount>=6){
                    break;
                }  
            }
        }

        if(lineCount<6){
            return false;
        }

        lcore::strncpy(accInfo.path_, 255, lines[1], lcore::strlen(lines[1])+1);
        f32 scale = static_cast<f32>( atof(lines[2]) );

        u32 i=0;
        f32 f[6];
        Char* str = lines[3];
        while(str != lines[5]){
            if(*str == ',' || *str == '\0'){
                *str = '\0';
                f[i] = static_cast<f32>( atof(lines[3]) );
                ++i;
                if(i>=6){
                    break;
                }
                lines[3] = str+1;
            }
            ++str;
        }

        static const f32 toRad = (PI/180.0f);
        matrix.identity();
        //vacの設定は無視
#if 0
        matrix.setScale(scale);
        matrix.rotateX(f[3]*toRad);
        matrix.rotateY(f[4]*toRad);
        matrix.rotateZ(f[5]*toRad);

        matrix.translate(f[0], f[1], f[2]);
#endif
        return true;
    }
}

#endif //INC_PMM_CPP__
