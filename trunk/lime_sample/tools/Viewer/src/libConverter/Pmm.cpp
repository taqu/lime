#ifndef INC_PMM_CPP__
#define INC_PMM_CPP__
/**
@file Pmm.cpp
@author t-sakai
@date 2011/02/05 create
*/
#include "Pmm.h"
#include <lcore/utility.h>

#include "XLoader.h"

#include "../scene/AnimObject.h"
#include "../scene/Object.h"
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
    //--- �G���[����
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
    // X�t�@�C���̂��߂̃X�P�[�����O
    const f32 Loader::XToPMMScaling = 10.0f;


    //----------------------------------------------
    // �A�j���[�V�����f�[�^���[�h
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


    //----------------------------------------------
    Loader::Loader()
        :errorCode_(Error_None)
        ,status_(Status_None)
        ,filename_(NULL)
        ,directory_(NULL)
        ,startFrame_(0)
        ,lastFrame_(0)
        ,loadModelIndex_(0)
        ,numModels_(0)
        ,modelPacks_(NULL)
        ,loadAccessoryIndex_(0)
        ,numAccessories_(0)
        ,accessoryPacks_(NULL)
    {
        pmdPack_.setNameTextureMap(&nameTexMap_);
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
    void Loader::open(const Char* filename, const Char* directory)
    {
        LASSERT(filename != NULL);
        status_ = Status_None;

        //�f�o�b�O��񏉊���
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        debugLog_.reset();
#endif

        //�p�X�����[�J���ɃR�s�[
        //--------------------------------------------------
        lenFilename_ = lcore::strlen(filename);
        lenDirectory_ = (directory != NULL)? lcore::strlen(directory) : 0;

        pathBuffer_.resize(lenFilename_ + lenDirectory_ + 2);
        u32 i;
        Char* str = pathBuffer_.get<Char>(0);
        for(i=0; i<lenFilename_; ++i){
            str[i] = filename[i];
        }
        str[i] = '\0';
        ++i;
        for(u32 j=0; j<lenDirectory_; ++j, ++i){
            str[i] = directory[j];
        }
        str[i] = '\0';

        filename_ = pathBuffer_.get<Char>(0);
        directory_ = (directory != NULL)? pathBuffer_.get<Char>(lenFilename_+1) : NULL;


        //�t�@�C���p�X���\�����ăI�[�v��
        {
            StringBuffer filepath;
            if(directory_ != NULL){
                filepath.push_back(directory, lenDirectory_);
                filepath.push_back(filename_, lenFilename_);

            }else{
                filepath.push_back(filename_, lenFilename_);
            }

            file_.open(filepath.c_str(), lcore::ios::binary);
            if(false == file_.is_open()){
                errorCode_ = Error_FileOpen;

                errorString_.print("pmm: fail to open %s", filename);
                return;
            }
        }

        status_ = Status_FileOpened;
    }

    //----------------------------------------------
    void Loader::load()
    {
        LASSERT(filename_ != NULL);
        LASSERT(file_.is_open());

        switch(status_)
        {
        case Status_None: //�����ɂ͂��Ȃ�
            {
                LASSERT(false);
                status_ = Status_Finish;
                if(Error_None == errorCode_){
                    errorCode_ = Error_Read;
                }
            }
            break;

            //---------------------------------------------
        case Status_FileOpened:
            {//�w�b�_���[�h
                readMagicVersion();
                readEnvironment();

                u8 numModels = 0;
                if(0 == io::read(file_, numModels)){
                    errorCode_ = Error_Read;
                }


                if(errorCode_ != Error_None){
                    errorString_.print("pmm: format error");
                    status_ = Status_Finish;
                    return;
                }

                numModels_ = numModels;

                LIME_DELETE_ARRAY(modelPacks_);
                modelPacks_ = LIME_NEW ModelPack[numModels_];

                readModelNames();
                if(errorCode_ != Error_None){
                    errorString_.print("pmm: fail to load model names");
                    status_ = Status_Finish;
                    return;
                }

                loadModelIndex_ = 0;
                status_ = Status_LoadModels;
            }
            break;

            //---------------------------------------------
        case Status_LoadModels:
            {
                readModels();
                if(errorCode_ != Error_None){
                    status_ = Status_Finish;
                    return;
                }

                if(numModels_<=loadModelIndex_){
                    status_ = Status_LoadCameraAndLights;
                }
            }
            break;

            //---------------------------------------------
        case Status_LoadCameraAndLights:
            {
                readCamera();
                if(errorCode_ != Error_None){
                    status_ = Status_Finish;
                    return;
                }

                readLight();
                if(errorCode_ != Error_None){
                    status_ = Status_Finish;
                    return;
                }

                loadAccessoryIndex_ = 0;
                readAccessoryNames();

                status_ = Status_LoadAccessories;
            }
            break;

            //---------------------------------------------
        case Status_LoadAccessories:
            {
                readAccessory();
                if(errorCode_ != Error_None
                    || numAccessories_<=loadAccessoryIndex_)
                {

                    //�����ŁA�J�n�E�I���t���[�������[�h
                    EnvironmentInfo info;
                    info.readAfter(file_);

                    startFrame_ = info.startFrame_;
                    if(info.lastFrame_ > 0){
                        lastFrame_ = info.lastFrame_;
                    }
                    if(startFrame_>lastFrame_){
                        startFrame_ = lastFrame_;
                    }

                    status_ = Status_Finish;
                    file_.close();
                }
            }
            break;

        default:
            break;
        };
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
        EnvironmentInfo info;
        info.readPrev(file_);
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
        static const Char ModelDirectory[] = "Model/";
        static const u32 LenModelDir = sizeof(ModelDirectory) - 1;

        if(loadModelIndex_>=numModels_){
            return;
        }

        switch(pmdPack_.getState())
        {
        case pmd::Pack::State_None:
            {
                file_ >> modelInfo_;

                //�t�@�C���I�[�v��
                u32 dlen = lenDirectory_ + LenModelDir;
                Char* path = createPath(modelInfo_.path_, ModelDirectory, LenModelDir);


                if(false == pmdPack_.open(path)){
                    //Model�f�B���N�g���ɂȂ����PMM�t�@�C���Ɠ����f�B���N�g�������邩���ׂ�
                    path = createPath(modelInfo_.path_);

                    if(false == pmdPack_.open(path)){
                        errorCode_ = Error_ReadPmd;
                        errorString_.print("pmd: fail to open %s", path);
                        return;
                    }

                    dlen = lenDirectory_;
                }

                if(dlen <= 0){
                    buffer_.resize(1);
                }
                Char* directory = buffer_.get<Char>(0);

                directory[dlen] = '\0'; //�f�B���N�g���p�X���쐬

            }
            break;

        case pmd::Pack::State_Finish:
            {
                lscene::AnimObject* object = LIME_NEW lscene::AnimObject;
                if(object == NULL){
                    errorCode_ = Error_ReadPmd;
                    return;
                }

                Char* directory = buffer_.get<Char>(0);
                if(false == pmdPack_.createObject(*object, directory, false)){
                    LIME_DELETE(object);
                    errorCode_ = Error_ReadPmd;
                    errorString_.print("pmd: fail to load");
                    return;
                }

                pmd::DispLabel dispLabel;
                pmdPack_.getDispLabel().swap(dispLabel);

                //�f�o�b�O���ǉ�
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
                debugLog_ += pmdPack_.debugLog_;
#endif
                modelPacks_[loadModelIndex_].set(object);
                modelPacks_[loadModelIndex_].setSkinPack(pmdPack_.getSkinPack());
                modelPacks_[loadModelIndex_].getRigidBodyPack().swap( pmdPack_.getRigidBodyPack());
                pmdPack_.release();

                //�t���[�����[�h
                readJointPoses(loadModelIndex_, dispLabel);

                readSkinMorphPoses(loadModelIndex_);

                u32 numIKs = (modelPacks_[loadModelIndex_].getObject()->getIKPack())?
                    modelPacks_[loadModelIndex_].getObject()->getIKPack()->getNumIKs()
                    : 0;

                u32 numIKFrames = 0;
                IKFrame ikFrame;
                IKFrame::readInit(file_, ikFrame, numIKs);

                io::read(file_, numIKFrames);
                for(u32 j=0; j<numIKFrames; ++j){
                    IKFrame::read(file_, ikFrame, numIKs);
                }

                //�ȉ��G�f�B�^��̌��ݒl
                u32 numJoints = modelPacks_[loadModelIndex_].getNumJoints();
                for(u32 j=0; j<numJoints; ++j){
                    JointFrame::readInit2(file_);
                }

                u32 numSkins = modelPacks_[loadModelIndex_].getSkinPack().getNumSkins();
                for(u32 j=0; j<numSkins; ++j){
                    SkinFrame::readInit2(file_);
                }
                for(u32 j=0; j<numIKs; ++j){
                    IKFrame::readInit2(file_);
                }

                u16* boneMap = NULL;
                dispLabel.swapBoneMap(boneMap);
                modelPacks_[loadModelIndex_].setBoneMap(boneMap);

                ++loadModelIndex_; //�ЂƂ��[�h����
            }
            break;

        case pmd::Pack::State_Error:
            {
                errorCode_ = Error_ReadPmd;
                errorString_.print("pmd: fail to load");
            }
            break;

        default:
            {
                Char* directory = buffer_.get<Char>(0);
                pmdPack_.updateLoad(directory);
            }
            break;
        };

        //StringBuffer path;
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
        //�����t���[�����΂��ăt���[�������[�h
        {
            s32 initFrameSize = sizeof(u32)*3 + 16 + sizeof(f32)*7 + 1;
            s32 offset = initFrameSize * numJoints;
            file_.seekg(offset, lcore::ios::cur);

            //�t���[����
            io::read(file_, numJointPoses);

            offset += sizeof(u32);
            file_.seekg(-offset, lcore::ios::cur);
        }

        //���[�h�p�o�b�t�@�쐬
        u32* numFrames = NULL;
        JointFrame* jointFrames = NULL;
        {
            u32 offset = sizeof(u32) * numJoints;
            u32 size = offset + (numJointPoses + numJoints) * sizeof(JointFrame);

            buffer_.resize(size);
            numFrames = buffer_.get<u32>(0);

            jointFrames = buffer_.get<JointFrame>(offset);
        }

        //�����t���[�����[�h
        for(u32 i=0; i<numJoints; ++i){
            JointFrame::readInit(file_, jointFrames[i]);
        }
        //
        io::read(file_, numJointPoses);

        //�t���[�����[�h
        for(u32 i=0; i<numJointPoses; ++i){
            file_ >> jointFrames[i + numJoints];
        }

        //�e�W���C���g�i�{�[���j�̃t���[�����𐔂���
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


        //�A�j���[�V�����N���b�v�쐬
        lanim::Skeleton::pointer& skeleton = modelPack.getObject()->getSkeleton();

        const u16* boneMap = dispLabel.getBoneMap();
        LASSERT(boneMap != NULL);

        u32 poseIndex = 0;
        u32 lastFrame = 0;
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
                if(lastFrame<jointPose.frameNo_){
                    lastFrame = jointPose.frameNo_;
                }

                ++poseIndex;
                next = jointFrames[next].nextDataID_;
                if(next == 0){
                    break;
                }
            }while(next<totalJointFrames);
        }

        animClip->setLastFrame( static_cast<f32>(lastFrame) );
        if(lastFrame_<lastFrame){
            lastFrame_ = lastFrame;
        }

        //���O�Z�b�g
        if(numJoints>0){
            animClip->getJointAnimation(0).setName(  skeleton->getJointName( boneMap[0] ) ); //�Z���^�[��0�ŌŒ�

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
    // ���f���̃X�L���A�j���[�V�������[�h
    void Loader::readSkinMorphPoses(u32 index)
    {
        ModelPack& modelPack = modelPacks_[index];
        u32 numSkins = modelPack.getSkinPack().getNumSkins();

        u32 numSkinPoses = 0;
        //�����t���[�����΂��ăt���[�������[�h
        {
            s32 initFrameSize = sizeof(u32)*3 + sizeof(f32) + 1;
            s32 offset = initFrameSize * numSkins;
            file_.seekg(offset, lcore::ios::cur);

            //�t���[����
            io::read(file_, numSkinPoses);

            if(numSkinPoses<=0){
                return;
            }

            offset += sizeof(u32);
            file_.seekg(-offset, lcore::ios::cur);
        }

        //���[�h�p�o�b�t�@�쐬
        u32* numFrames = NULL;
        SkinFrame* skinFrames = NULL;
        {
            u32 offset = sizeof(u32) * numSkins;
            u32 size = offset + (numSkinPoses + numSkins) * sizeof(SkinFrame);

            buffer_.resize(size);
            numFrames = buffer_.get<u32>(0);

            skinFrames = buffer_.get<SkinFrame>(offset);
        }

        //�����t���[�����[�h
        for(u32 i=0; i<numSkins; ++i){
            SkinFrame::readInit(file_, skinFrames[i]);
        }
        //
        io::read(file_, numSkinPoses);

        //�t���[�����[�h
        for(u32 i=0; i<numSkinPoses; ++i){
            file_ >> skinFrames[i + numSkins];
        }

        //�e�\��̃t���[�����𐔂���
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
        //�\��A�j���[�V�����쐬
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
    // �J�����A�j���[�V�������[�h
    void Loader::readCamera()
    {
        loadAnim<CameraAnimPack, CameraFrame>(cameraAnimPack_, true);
    }

    //----------------------------------------------
    // ���C�g�A�j���[�V�������[�h
    void Loader::readLight()
    {
        loadAnim<LightAnimPack, LightFrame>(lightAnimPack_, true);
    }

    //----------------------------------------------
    // �A�N�Z�T����񃍁[�h
    void Loader::readAccessoryNames()
    {
        file_.seekg(5, lcore::ios::cur); //unknown 5 bytes
        u8 numAccessories = 0;
        if(0 == io::read(file_, numAccessories)){
            errorCode_ = Error_Read;
            status_ = Status_Finish;
            return;
        }
        numAccessories_ = numAccessories;

        LIME_DELETE_ARRAY(accessoryPacks_);
        accessoryPacks_ = LIME_NEW AccessoryPack[numAccessories_];

        AccessoryInfo accInfo;

        for(u8 i=0; i<numAccessories; ++i){
            AccessoryInfo::readName(file_, accInfo);
        }
    }

    //----------------------------------------------
    // �A�N�Z�T���A�A�j���[�V�������[�h
    void Loader::readAccessory()
    {
        AccessoryInfo accInfo;
        lmath::Matrix34 matrix;
        lx::XLoader xloader;

        if(loadAccessoryIndex_<numAccessories_){
            file_ >> accInfo;

            AccessoryPack& accPack = accessoryPacks_[loadAccessoryIndex_];

            if(checkVac(accInfo, matrix)){
                accPack.setMatrix(matrix);
            }

            bool isLoaded = loadX(loadAccessoryIndex_, xloader, accInfo);
            xloader.release();

            loadAnim<AccessoryPack, AccessoryFrame>(accPack, isLoaded);

            //���[�h�ł��Ȃ���΁A�A�N�Z�T���������炷
            if(isLoaded){
                const AccessoryPose& pose = accPack.getPose(0);
                accPack.setIsDisp(pose.getDisp() == 1);

                ++loadAccessoryIndex_;
            }else{
                --numAccessories_;
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
    }

    bool Loader::loadX(u32 index, lx::XLoader& xloader, const AccessoryInfo& info)
    {
        static const Char AccessoryDirectory[] = "Accessory/";
        static const u32 LenAccessoryDir = sizeof(AccessoryDirectory) - 1;

        LASSERT(accessoryPacks_ != NULL);

        u32 dlen = lenDirectory_ + LenAccessoryDir;
        Char* path = createPath(info.path_, AccessoryDirectory, LenAccessoryDir);

        if(false == xloader.open(path)){
            //Accessory�f�B���N�g���ɂȂ����PMM�t�@�C���Ɠ����f�B���N�g�������邩���ׂ�
            path = createPath(info.path_);

            if(false == xloader.open(path)){
                errorString_.print("x: fail to open %s", path);
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

        directory[dlen] = '\0'; //�f�B���N�g���p�X���쐬
        xloader.load(*object, directory, XToPMMScaling, false);
        xloader.close();

        if(lx::XLoader::Error_None != xloader.getErrorCode()){
            LIME_DELETE(object);
            lcore::Log("x: fail to load %s", path);
            return false;
        }

        //�f�o�b�O���ǉ�
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
        if(NULL == ext){//.��������Ȃ��ꍇ�͂w�t�@�C��������
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


    // VAC�t�@�C�����`�F�b�N���A�����Ȃ�΃p�����[�^���[�h
    bool Loader::checkVac(AccessoryInfo& accInfo, lmath::Matrix34& matrix)
    {
        static const Char AccessoryDirectory[] = "Accessory/";
        static const u32 LenAccessoryDir = sizeof(AccessoryDirectory) - 1;

        if(NULL == lcore::strstr(accInfo.path_, ".vac")){
            return false;
        }

        Char* path = createPath(accInfo.path_, AccessoryDirectory, LenAccessoryDir);

        lcore::ifstream file(path, lcore::ios::binary);
        if(false == file.is_open()){
            //Accessory�f�B���N�g���ɂȂ����PMM�t�@�C���Ɠ����f�B���N�g�������邩���ׂ�
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
        //f32 scale = static_cast<f32>( atof(lines[2]) );

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

        matrix.identity();
        //vac�̐ݒ�͖���
#if 0
        static const f32 toRad = (PI/180.0f);
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
