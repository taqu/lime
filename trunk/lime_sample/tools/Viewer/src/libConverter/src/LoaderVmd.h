#ifndef INC_LOADERVMD_H__
#define INC_LOADERVMD_H__
/**
@file LoaderVmd.h
@author t-sakai
@date 2009/11/11 create
*/
namespace lgraphics
{
    class AnimDatabase;

    class AnimObject;
}



namespace vmd
{
    /**
    @brief VMDファイルローダ
    */
    class LoaderVmd
    {
    public:
        LoaderVmd();
        ~LoaderVmd();

        /**
        @brief ファイルからロード
        @return 成否
        @param filename
        */
        bool readFile(const char* filename);

        /**
        @brief アニメーションデータ作成
        */
        lgraphics::AnimDatabase* createAnimDB();
    private:
        class Impl;
        Impl *impl_;
    };
}

#endif //INC_LOADERVMD_H__
