#ifndef INC_EGDA_APPLICATION_H__
#define INC_EGDA_APPLICATION_H__

namespace lanim
{
    class AnimationControler;
}

namespace lscene
{
    class AnimObject;
}

namespace egda
{
    enum ResourceType
    {
        ResType_Model = 0,
        ResType_Anim,
        ResType_Num,
    };

    /// 処理ループを持ったアプリケーションフレームワーク
    class Application
    {
    public:
        Application();
        ~Application();

        static Application& getInstance()
        {
            static Application instance;
            return instance;
        }

        bool initialize();
        void terminate();

        void update();

        void setViewport(s32 left, s32 top, s32 width, s32 height);

        bool openModel(const Char* path, const Char* directory);
        bool openAnim(const Char* path);

    private:
        void initAnimControler();

        f32 counter_;

        lscene::AnimObject *animObj_;
        lanim::AnimationControler *animControler_;
    };
}
#endif //INC_EGDA_APPLICATION_H__
