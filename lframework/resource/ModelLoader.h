#ifndef INC_LFRAMEWORK_MODELLOADER_H__
#define INC_LFRAMEWORK_MODELLOADER_H__
/**
@file ModelLoader.h
@author t-sakai
@date 2016/11/23 create
*/
#include "load.h"
#include <lcore/Array.h>
#include <lcore/io/VirtualFileSystem.h>
#include <lgraphics/Enumerations.h>
#include "resource/Resource.h"

namespace lgfx
{
    class Texture2DRef;
    class SamplerStateRef;
    class ShaderResourceViewRef;
    class InputLayoutRef;
}

namespace lfw
{
    class Model;
    class AnimModel;
    class Geometry;
    class Mesh;
    class Material;
    class Node;
    class Texture2D;

    class Texture;
    class Skeleton;

    class LoadTexture;

    class ModelLoader
    {
    public:
        ModelLoader();
        explicit ModelLoader(lcore::FileSystem* fileSystem);
        ~ModelLoader();

        void set(lcore::FileSystem* fileSystem);

        /**
        @brief オブジェクトロード
        @return オブジェクト
        @param filepath ... ファイルパス
        */
        Model* loadModel(s32 setID, const Char* filepath);

        /**
        @brief オブジェクトロード
        @return オブジェクト
        @param filepath ... ファイルパス
        */
        AnimModel* loadAnimModel(s32 setID, const Char* filepath);

        void setReserveGeometryData(bool enable)
        {
            reserveGeometryData_ = enable;
        }

        void setConvertRefractiveIndex(bool enable)
        {
            convertRefractiveIndex_ = enable;
        }

        static bool save(const Char* filepath, Model& model, LoadTexture* textures, Skeleton* skeleton);

        inline s32 getNumTexture() const;
        inline Resource::pointer& getTexture(s32 index);
        inline void clearTextures();

    private:
        ModelLoader(const ModelLoader&);
        ModelLoader& operator=(const ModelLoader&);

        void setDirectoryPath(const Char* path);

        /**
        @brief ヘッダロード
        @return 成否
        @param 出力。ヘッダ
        */
        bool load(LoadHeader& header);

        /**
        @brief モデルロード
        @return 成否
        @param 出力。モデル
        */
        bool loadInternal(Model& model);
        bool loadSkeleton(AnimModel& model);
        bool loadTextures(s32 setID, Model& model);
        void calcBoundingSphere(Model& model);

        /**
        @brief ジオメトリロード
        @return 成否
        @param 出力。ジオメトリ
        */
        bool load(Geometry& geometry);

        /**
        @brief メッシュロード
        @return 成否
        @param 出力。メッシュ
        */
        bool load(Model& model, Mesh& mesh);

        /**
        @brief マテリアルロード
        @return 成否
        @param 出力。マテリアル
        */
        bool load(Material& material);

        /**
        @brief ノードロード
        @return 成否
        @param 出力。ノード
        */
        bool load(Model& model, Node& node);

        /**
        @brief テクスチャロード
        @return 成否
        @param 出力。テクスチャ
        */
        bool load(s32 setID, Texture2D& texture, const LoadTexture& loadTexture);

        /**
        @brief 頂点レイアウト作成
        @param header ... 頂点フラグ
        @param inputLayout ... 出力。頂点レイアウト
        */
        void createInputLayout(u32 vflag, lgfx::InputLayoutRef& inputLayout);

        lcore::FileSystem* fileSystem_;
        lcore::FileProxyIStream fileStream_;

        lcore::Array<Resource::pointer> textures_;

        LoadHeader header_; /// ヘッダ
        s32 directoryPathLength_;
        Char* path_;

        bool reserveGeometryData_;
        bool convertRefractiveIndex_;
    };

    inline s32 ModelLoader::getNumTexture() const
    {
        return textures_.size();
    }

    inline Resource::pointer& ModelLoader::getTexture(s32 index)
    {
        return textures_[index];
    }

    inline void ModelLoader::clearTextures()
    {
        textures_.clear();
    }
}
#endif //INC_LFRAMEWORK_MODELLOADER_H__
