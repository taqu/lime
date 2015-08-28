#ifndef INC_LSCENE_LLOAD_MODELLOADER_H__
#define INC_LSCENE_LLOAD_MODELLOADER_H__
/**
@file ModelLoader.h
@author t-sakai
@date 2014/12/09 create
*/
#include "load.h"
#include "../anim/Skeleton.h"
#include <lgraphics/api/Enumerations.h>

namespace lgraphics
{
    class Texture2DRef;
    class SamplerStateRef;
    class ShaderResourceViewRef;
    class InputLayoutRef;
}

namespace lscene
{
namespace lfile
{
    class SharedFile;
}

namespace lrender
{
    class Object;
    class AnimObject;
    class ObjectContent;
    class AnimObjectContent;
    class Geometry;
    class Mesh;
    class Material;
    class Node;
    class Texture2D;
}

namespace lload
{
    class Texture;

    class ModelLoader
    {
    public:
        ModelLoader();
        ~ModelLoader();

        /**
        @brief ディレクトリパスセット
        @param filepath
        */
        void setDirectoryPath(const Char* filepath);

        /**
        @brief オブジェクトロード
        @return 成否
        @param 出力。オブジェクト
        */
        bool load(lrender::Object& obj, lscene::lfile::SharedFile* file);

        /**
        @brief オブジェクトロード
        @return 成否
        @param 出力。オブジェクト
        */
        bool load(lrender::AnimObject& obj, lscene::lfile::SharedFile* file);

        lanim::Skeleton::pointer getSkeleton(){ return skeleton_;}

        void getTextureNameTable(u32 numTextures, lload::Texture* textures);

        void setTextureAddress(lgraphics::TextureAddress texAddress)
        {
            textureAddress_ = texAddress;
        }

        void setReserveGeometryData(bool enable)
        {
            reserveGeometryData_ = enable;
        }

        void setConvertRefractiveIndex(bool enable)
        {
            convertRefractiveIndex_ = enable;
        }

        void setOpenForceOSFileSystem(bool enable)
        {
            openForceOSFileSystem_ = enable;
        }

        static bool save(lrender::Object& obj, lload::Texture* textures, lanim::Skeleton::pointer& skeleton, const Char* filepath);
    private:
        ModelLoader(const ModelLoader&);
        ModelLoader& operator=(const ModelLoader&);

        /**
        @brief ヘッダロード
        @return 成否
        @param 出力。ヘッダ
        */
        bool load(Header& header);

        /**
        @brief オブジェクトロード
        @return 成否
        @param 出力。オブジェクト
        */
        bool loadInternal(lrender::ObjectContent& obj);

        /**
        @brief ジオメトリロード
        @return 成否
        @param 出力。ジオメトリ
        */
        bool load(lrender::Geometry& geometry);

        /**
        @brief メッシュロード
        @return 成否
        @param 出力。メッシュ
        */
        bool load(lrender::ObjectContent& obj, lrender::Mesh& mesh);

        /**
        @brief マテリアルロード
        @return 成否
        @param 出力。マテリアル
        */
        bool load(lrender::Material& material);

        /**
        @brief ノードロード
        @return 成否
        @param 出力。ノード
        */
        bool load(lrender::ObjectContent& obj, lrender::Node& node);

        /**
        @brief スケルトンロード
        */
        lanim::Skeleton* loadSkeleton(u32 numJoints);

        /**
        @brief テクスチャロード
        @return 成否
        @param 出力。テクスチャ
        */
        bool load(lrender::Texture2D& texture, const lload::Texture& loadTexture);

        /**
        @brief 頂点レイアウト作成
        @param header ... 頂点フラグ
        @param inputLayout ... 出力。頂点レイアウト
        */
        void createInputLayout(u32 vflag, lgraphics::InputLayoutRef& inputLayout);

        lscene::lfile::SharedFile* file_;

        Header header_; /// ヘッダ
        u32 directoryPathLength_;
        Char directoryPath_[MaxPathSize+MaxFileNameSize];

        lanim::Skeleton::pointer skeleton_;

        lgraphics::TextureAddress textureAddress_;

        bool reserveGeometryData_;
        bool convertRefractiveIndex_;
        bool openForceOSFileSystem_;
    };
}
}
#endif //INC_LSCENE_LLOAD_MODELLOADER_H__
