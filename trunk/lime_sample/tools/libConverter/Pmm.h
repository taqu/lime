#ifndef INC_PMM_H__
#define INC_PMM_H__
/**
@file Pmm.h
@author t-sakai
@date 2011/02/05 create
*/
#include "PmmDef.h"
namespace lx
{
    class XLoader;
}

namespace pmm
{
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

        inline ErrorCode getErrorCode() const;

        void load(const Char* filename, const Char* directory);

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
        @brief PMD���[�h
        */
        void loadPmd(u32 index, ModelInfo& modelInfo, pmd::DispLabel& dispLabel);

        /**
        @brief �J�����A�j���[�V�������[�h
        */
        void readCamera();

        /**
        @brief ���C�g�A�j���[�V�������[�h
        */
        void readLight();

        /**
        @brief �A�N�Z�T���A�A�j���[�V�������[�h
        */
        void readAccessory();

        bool loadX(u32 index, lx::XLoader& xloader, const AccessoryInfo& info);

        Char* createPath(const Char* filename);
        Char* createPath(const Char* filename, const Char* directory, const u32 lenDir);

        Format getFormat(const Char* filename) const;

        /**
        @brief VAC�t�@�C�����`�F�b�N���A�����Ȃ�΃p�����[�^���[�h
        @param accInfo ... path��vac�t�@�C���Ȃ烍�[�h����X�t�@�C�����ɏ�������
        @param matrix ... vac�t�@�C���Ȃ�I�t�Z�b�g�����[�h
        */
        bool checkVac(AccessoryInfo& accInfo, lmath::Matrix43& matrix);

        template<class PACK_TYPE, class FRAME_TYPE>
        inline void loadAnim(PACK_TYPE& pack, bool convert);

        ErrorCode errorCode_;

        lcore::ifstream file_;

        u32 lenFilename_;
        const Char* filename_;
        u32 lenDirectory_;
        const Char* directory_;

        u32 startFrame_;
        u32 endFrame_;

        u32 numModels_;
        ModelPack* modelPacks_;

        CameraAnimPack cameraAnimPack_;
        LightAnimPack lightAnimPack_;

        u32 numAccessories_;
        AccessoryPack* accessoryPacks_;

        lconverter::NameTextureMap nameTexMap_;

        lcore::Buffer buffer_; //���[�h�p�e���|�����o�b�t�@

        //�f�o�b�O�p���O
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
    public:
        lconverter::DebugLog debugLog_;
#endif
    };

    inline Loader::ErrorCode Loader::getErrorCode() const
    {
        return errorCode_;
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
