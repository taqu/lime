#ifndef INC_EGDA_APPLICATION_H__
#define INC_EGDA_APPLICATION_H__
#include "Scene.h"
#include "TextRenderer.h"

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
        static const u32 MaxChars = 256;
        static const u32 CharW = 9;
        static const u32 CharH = 20;

        static const u32 Rows = 6;
        static const u32 Cols = 16;
        static const u32 RowsRatio = 2;
        static const u32 ColsRatio = 2;

        static const u32 MSecPerFrame = 32; //1フレームあたりの時間

        Application();
        ~Application();

        static Application& getInstance()
        {
            static Application instance;
            return instance;
        }

        bool initialize(const Char* textTexture, u32 size);
        void terminate();

        void update();

        void setViewport(s32 left, s32 top, s32 width, s32 height);

        bool loadPmm(const Char* filename, const Char* directory);

        void setState(s32 state){ scene_.setState( static_cast<Scene::State>(state) );}
        void setCameraMode(s32 mode){ scene_.setCameraMode(mode);}

        void resetProjection(){ scene_.resetProjection(); }
    private:
        Scene scene_;
        TextRenderer textRenderer_;
        u32 prevMSec_;
        u32 currentMSec_;
    };
}
#endif //INC_EGDA_APPLICATION_H__
