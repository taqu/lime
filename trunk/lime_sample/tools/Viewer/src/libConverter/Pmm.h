#ifndef INC_PMM_H__
#define INC_PMM_H__
/**
@file Pmm.h
@author t-sakai
@date 2011/02/05 create
*/
#include "PmmDef.h"
#include "Pmd.h"

namespace lx
{
    class XLoader;
}

namespace pmm
{
    //----------------------------------------------
    //---
    //--- Loader
    //---
    //----------------------------------------------
    class Loader
    {
    public:
        /// X�t�@�C���̂��߂̃X�P�[�����O
        static const f32 XToPMMScaling; //=10.0f;

        Loader();
        ~Loader();

        enum ErrorCode
        {
            Error_None =0,
            Error_FileOpen,
            Error_Read,
            Error_ReadPmd,
            Error_ReadX,
            Error_Num,
        };

        enum Format
        {
            Format_X =0,
            Format_VAC,
            Format_Num,
        };

        enum Status
        {
            Status_None = 0,
            Status_FileOpened,
            Status_LoadModels,
            Status_LoadCameraAndLights,
            Status_LoadAccessories,
            Status_Finish,
        };

        inline ErrorCode getErrorCode() const;
        inline const Char* getErrorString() const;

        inline Status getStatus() const;

        void open(const Char* filename, const Char* directory);

        /**
        */
        void load();

        inline u32 getStartFrame() const;
        inline u32 getLastFrame() const;

        inline const ModelInfo& getModelInfo() const;

        //TODO:�C���^�[�t�F�C�X���悭����
        inline u32 getNumModels() const;
        inline ModelPack* releaseModelPacks();

        inline CameraAnimPack& getCameraAnimPack();
        inline LightAnimPack& getLightAnimPack();

        //TODO:�C���^�[�t�F�C�X���悭����
        inline u32 getNumAccessories() const;
        inline AccessoryPack* releaseAccessoryPacks();

    private:
        Loader(const Loader&);
        Loader& operator=(const Loader&);

        void readMagicVersion();
        void readEnvironment();

        /**
        @brief ���f�������[�h�B�\�����H
        */
        void readModelNames();

        /**
        @brief ���f�����[�h
        */
        void readModels();

        /**
        @brief ���f���̃W���C���g�A�j���[�V�������[�h
        */
        void readJointPoses(u32 index, pmd::DispLabel& dispLabel);

        /**
        @brief ���f���̃W���C���g�A�j���[�V�������[�h
        */
        void readSkinMorphPoses(u32 index);

        /**
        @brief �J�����A�j���[�V�������[�h
        */
        void readCamera();

        /**
        @brief ���C�g�A�j���[�V�������[�h
        */
        void readLight();

        /**
        @brief �A�N�Z�T����񃍁[�h
        */
        void readAccessoryNames();

        /**
        @brief �A�N�Z�T���A�A�j���[�V�������[�h
        */
        void readAccessory();

        /**
        @brief X�t�@�C�����[�h
        @return ����
        @param index ... ���f���C���f�b�N�X
        @param xloader ... x�t�@�C�����[�_
        @param info ... �A�N�Z�T�����
        */
        bool loadX(u32 index, lx::XLoader& xloader, const AccessoryInfo& info);

        /**
        @brief �p�X�����쐬
        
        buffer_�ɍ쐬����̂ŁA�󂳂Ȃ����ƁB
        */
        Char* createPath(const Char* filename);

        /**
        @brief �p�X�����쐬
        
        buffer_�ɍ쐬����̂ŁA�󂳂Ȃ����ƁB
        */
        Char* createPath(const Char* filename, const Char* directory, const u32 lenDir);

        /**
        @brief �g���q����t�@�C���t�H�[�}�b�g�擾
        */
        Format getFormat(const Char* filename) const;

        /**
        @brief VAC�t�@�C�����`�F�b�N���A�����Ȃ�΃p�����[�^���[�h
        @param accInfo ... path��vac�t�@�C���Ȃ烍�[�h����X�t�@�C�����ɏ�������
        @param matrix ... vac�t�@�C���Ȃ�I�t�Z�b�g�����[�h
        */
        bool checkVac(AccessoryInfo& accInfo, lmath::Matrix34& matrix);

        /**
        @brief �A�j���[�V�����f�[�^���[�h

        �L�����N�^�A�J�����A���C�g�̃A�j���[�V�����ŋ��ʂ̏���
        */
        template<class PACK_TYPE, class FRAME_TYPE>
        inline void loadAnim(PACK_TYPE& pack, bool convert);

        ErrorCode errorCode_;
        StringBuffer errorString_;

        Status status_;

        lcore::ifstream file_; ///�t�@�C��

        u32 lenFilename_;       /// �t�@�C��������
        const Char* filename_;  /// �t�@�C����
        u32 lenDirectory_;      /// �f�B���N�g��������
        const Char* directory_; /// �f�B���N�g����

        u32 startFrame_; /// �J�n�t���[��
        u32 lastFrame_; /// �I���t���[��

        pmd::Pack pmdPack_; ///PMD���[�_

        ModelInfo modelInfo_;

        u32 loadModelIndex_;
        u32 numModels_;
        ModelPack* modelPacks_;

        CameraAnimPack cameraAnimPack_;
        LightAnimPack lightAnimPack_;

        u32 loadAccessoryIndex_;
        u32 numAccessories_;
        AccessoryPack* accessoryPacks_;

        lconverter::NameTextureMap nameTexMap_;

        lcore::Buffer pathBuffer_; //�t�@�C���p�X�ۑ��p�o�b�t�@
        lcore::Buffer buffer_; //���[�h�p�e���|�����o�b�t�@

        //�f�o�b�O�p���O
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
    public:
        lconverter::DebugLog debugLog_;
#endif
    };



    //----------------------------------------------
    //--- Loader inline����
    //----------------------------------------------
    inline Loader::ErrorCode Loader::getErrorCode() const
    {
        return errorCode_;
    }

    inline const Char* Loader::getErrorString() const
    {
        return errorString_.c_str();
    }

    inline Loader::Status Loader::getStatus() const
    {
        return status_;
    }

    inline u32 Loader::getStartFrame() const
    {
        return startFrame_;
    }

    inline u32 Loader::getLastFrame() const
    {
        return lastFrame_;
    }

    inline u32 Loader::getNumModels() const
    {
        return numModels_;
    }

    inline ModelPack* Loader::releaseModelPacks()
    {
        ModelPack* modelPacks = modelPacks_;
        modelPacks_ = NULL;
        numModels_ = 0;
        return modelPacks;
    }

    inline CameraAnimPack& Loader::getCameraAnimPack()
    {
        return cameraAnimPack_;
    }

    inline LightAnimPack& Loader::getLightAnimPack()
    {
        return lightAnimPack_;
    }

    inline u32 Loader::getNumAccessories() const
    {
        return numAccessories_;
    }

    inline AccessoryPack* Loader::releaseAccessoryPacks()
    {
        AccessoryPack* accessoryPacks = accessoryPacks_;
        accessoryPacks_ = NULL;
        numAccessories_ = 0;
        return accessoryPacks;
    }
}
#endif //INC_PMM_H__
