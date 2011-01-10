#ifndef INC_LRENDER_SCENE_H__
#define INC_LRENDER_SCENE_H__
/**
@file Scene.h
@author t-sakai
@date 2009/12/24 create
*/
#include <list>
#include "../lrendercore.h"

namespace lrender
{
    class Light;
    class Camera;
    class Shape;

    class Scene
    {
    public:
        struct InitParam
        {
            LightType lightType_;
            CameraType cameraType_;
        };

        Scene();
        ~Scene();

        void add(Shape* shape);

    private:
        typedef std::list<Shape*> ShapeList;
        ShapeList shapes_;
    };
}
#endif //INC_LRENDER_SCENE_H__
