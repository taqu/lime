#ifndef INC_EGDA_APPLICATION_H__
#define INC_EGDA_APPLICATION_H__
#include "Scene.h"

namespace egda
{
    enum ResourceType
    {
        ResType_Pmm = 0,
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

        bool loadPmm(const Char* filename, const Char* directory);

    private:
        f32 aspect_;
        Scene scene_;
    };
}
#endif //INC_EGDA_APPLICATION_H__
