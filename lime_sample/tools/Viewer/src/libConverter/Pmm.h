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
        /// Xファイルのためのスケーリング
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

        //TODO:インターフェイスをよくする
        inline u32 getNumModels() const;
        inline ModelPack* releaseModelPacks();

        inline CameraAnimPack& getCameraAnimPack();
        inline LightAnimPack& getLightAnimPack();

        //TODO:インターフェイスをよくする
        inline u32 getNumAccessories() const;
        inline AccessoryPack* releaseAccessoryPacks();

    private:
        Loader(const Loader&);
        Loader& operator=(const Loader&);

        void readMagicVersion();
        void readEnvironment();

        /**
        @brief モデル名ロード。表示名？
        */
        void readModelNames();

        /**
        @brief モデルロード
        */
        void readModels();

        /**
        @brief モデルのジョイントアニメーションロード
        */
        void readJointPoses(u32 index, pmd::DispLabel& dispLabel);

        /**
        @brief モデルのジョイントアニメーションロード
        */
        void readSkinMorphPoses(u32 index);

        /**
        @brief カメラアニメーションロード
        */
        void readCamera();

        /**
        @brief ライトアニメーションロード
        */
        void readLight();

        /**
        @brief アクセサリ情報ロード
        */
        void readAccessoryNames();

        /**
        @brief アクセサリ、アニメーションロード
        */
        void readAccessory();

        /**
        @brief Xファイルロード
        @return 成否
        @param index ... モデルインデックス
        @param xloader ... xファイルローダ
        @param info ... アクセサリ情報
        */
        bool loadX(u32 index, lx::XLoader& xloader, const AccessoryInfo& info);

        /**
        @brief パス文字作成
        
        buffer_に作成するので、壊さないこと。
        */
        Char* createPath(const Char* filename);

        /**
        @brief パス文字作成
        
        buffer_に作成するので、壊さないこと。
        */
        Char* createPath(const Char* filename, const Char* directory, const u32 lenDir);

        /**
        @brief 拡張子からファイルフォーマット取得
        */
        Format getFormat(const Char* filename) const;

        /**
        @brief VACファイルかチェックし、そうならばパラメータロード
        @param accInfo ... pathがvacファイルならロードしてXファイル名に書き換え
        @param matrix ... vacファイルならオフセットをロード
        */
        bool checkVac(AccessoryInfo& accInfo, lmath::Matrix34& matrix);

        /**
        @brief アニメーションデータロード

        キャラクタ、カメラ、ライトのアニメーションで共通の処理
        */
        template<class PACK_TYPE, class FRAME_TYPE>
        inline void loadAnim(PACK_TYPE& pack, bool convert);

        ErrorCode errorCode_;
        StringBuffer errorString_;

        Status status_;

        lcore::ifstream file_; ///ファイル

        u32 lenFilename_;       /// ファイル名長さ
        const Char* filename_;  /// ファイル名
        u32 lenDirectory_;      /// ディレクトリ名長さ
        const Char* directory_; /// ディレクトリ名

        u32 startFrame_; /// 開始フレーム
        u32 lastFrame_; /// 終了フレーム

        pmd::Pack pmdPack_; ///PMDローダ

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

        lcore::Buffer pathBuffer_; //ファイルパス保存用バッファ
        lcore::Buffer buffer_; //ロード用テンポラリバッファ

        //デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
    public:
        lconverter::DebugLog debugLog_;
#endif
    };



    //----------------------------------------------
    //--- Loader inline実装
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
