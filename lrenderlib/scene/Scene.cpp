/**
@file Scene.cpp
@author t-sakai
@date 2010/01/05 create

*/
#include "Scene.h"

namespace lrender
{
    Scene::Scene()
    {
    }

    Scene::~Scene()
    {
    }

    void Scene::add(Shape* shape)
    {
        for(ShapeList::iterator itr = shapes_.begin();
            itr != shapes_.end();
            ++itr)
        {
            if(*itr == shape){
                return;
            }
        }

        shapes_.push_back(shape);
    }
}
