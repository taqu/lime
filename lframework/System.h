#ifndef INC_LFRAMEWORK_SYSTEM_H_
#define INC_LFRAMEWORK_SYSTEM_H_
/**
@file System.h
@author t-sakai
@date 2017/06/18 create
*/
#include "lframework.h"

namespace lcore
{
    class FileSystem;
}

namespace lgfx
{
    class Window;
}

namespace linput
{
    class Input;
}

namespace lfw
{
    class Application;
    class Timer;
    class Resources;
    class CollideManager;
    class Renderer;
    class ECSManager;
namespace graph
{
    class RenderGraph;
}

    class System
    {
    public:
        static void clear();

        static inline Application& getApplication();
        static void setApplication(Application* application);

        static inline lgfx::Window& getWindow();
        static void setWindow(lgfx::Window* window);

        static inline linput::Input& getInput();
        static void setInput(linput::Input* input);

        static inline Timer& getTimer();
        static void setTimer(Timer* timer);

        static inline lcore::FileSystem& getFileSystem();
        static void setFileSystem(lcore::FileSystem* fileSystem);

        static inline Resources& getResources();
        static void setResources(Resources* resources);

        static inline CollideManager& getCollideManager();
        static void setCollideManager(CollideManager* collideManager);

        static inline Renderer& getRenderer();
        static void setRenderer(Renderer* renderer);

        static inline ECSManager& getECSManager();
        static void setECSManager(ECSManager* ecsManager);

        static inline graph::RenderGraph& getRenderGraph();
        static void setRenderGraph(graph::RenderGraph* renderGraph);
    private:
        System() = delete;

        static Application* application_;
        static lgfx::Window* window_;
        static linput::Input* input_;
        static Timer* timer_;
        static lcore::FileSystem* fileSystem_;
        static Resources* resources_;
        static CollideManager* collideManager_;
        static Renderer* renderer_;
        static ECSManager* ecsManager_;
        static graph::RenderGraph* renderGraph_;
    };

    inline Application& System::getApplication()
    {
        return *application_;
    }

    inline lgfx::Window& System::getWindow()
    {
        return *window_;
    }

    inline linput::Input& System::getInput()
    {
        return *input_;
    }

    inline Timer& System::getTimer()
    {
        return *timer_;
    }

    inline lcore::FileSystem& System::getFileSystem()
    {
        return *fileSystem_;
    }

    inline Resources& System::getResources()
    {
        return *resources_;
    }

    inline CollideManager& System::getCollideManager()
    {
        return *collideManager_;
    }

    inline Renderer& System::getRenderer()
    {
        return *renderer_;
    }

    inline ECSManager& System::getECSManager()
    {
        return *ecsManager_;
    }

    inline graph::RenderGraph& System::getRenderGraph()
    {
        return *renderGraph_;
    }
}
#endif //INC_LFRAMEWORK_SYSTEM_H_
