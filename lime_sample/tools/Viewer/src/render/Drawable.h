#ifndef INC_DRAWABLE_H__
#define INC_DRAWABLE_H__
/**
@file Drawable.h
@author t-sakai
@date 2009/08/15 create
*/
#include <lcore/Buffer.h>

#include "lrender.h"
#include "Spatial.h"
#include "Batch.h"

namespace lmath
{
    class Matrix34;
}

namespace lscene
{
    class Scene;
    class Geometry;
    class Material;
}

namespace lrender
{
    class Batch;

    class Drawable : public Spatial
    {
    public:
        virtual u32 getNumJointPoses() const{ return 0; }
        virtual const lmath::Matrix34* getGlobalJointPoses() const{ return NULL;}


        void initializeShader();

        u32 getNumGeometries() const{ return numGeometries_;}
        lscene::Geometry& getGeometry(u32 index);

        u32 getNumMaterials() const{ return numMaterials_;}
        lscene::Material& getMaterial(s32 index);

        Batch& getBatch(u32 index){ return batches_[index];}

        inline Drawable* getNext();
        inline void setNext(Drawable* next);

        inline Drawable* getPrev();
        inline void setPrev(Drawable* prev);

        void addDraw();
        void removeDraw();

        void swap(Drawable& rhs);

    protected:
        Drawable();
        Drawable( u32 numGeometries, u32 numMaterials);
        ~Drawable();

        Drawable* next_;
        Drawable* prev_;

        u32 numGeometries_;
        lscene::Geometry *geometries_;

        u32 numMaterials_;
        lscene::Material *materials_;

        Batch *batches_;

        lcore::Buffer resourceBuffer_;
    };

    inline Drawable* Drawable::getNext()
    {
        return next_;
    }

    inline void Drawable::setNext(Drawable* next)
    {
        next_ = next;
    }

    inline Drawable* Drawable::getPrev()
    {
        return prev_;
    }

    inline void Drawable::setPrev(Drawable* prev)
    {
        prev_ = prev;
    }
}

#endif //INC_DRAWABLE_H__
