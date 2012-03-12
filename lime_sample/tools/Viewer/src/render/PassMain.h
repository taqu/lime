#ifndef INC_LRENDER_PASSMAIN_H__
#define INC_LRENDER_PASSMAIN_H__
/**
@file PassMain.h
@author t-sakai
@date 2010/11/22 create

*/
#include <lcore/Vector.h>
#include <lcore/intrusive/List.h>
#include <lframework/scene/Scene.h>

namespace lgraphics
{
    class GeometryBuffer;
}

namespace lscene
{
    class Geometry;
}

namespace lrender
{
    class Batch;
    class Drawable;

    class PassMain
    {
    public:
        PassMain();
        ~PassMain();

        inline lscene::Scene& getScene();

        void add(Drawable* drawable);
        void remove(Drawable* drawable);

        void draw();
    private:
        typedef lcore::intrusive::List<Drawable> DrawableList;
        typedef lcore::vector_arena<Batch*> BatchVector;

        void sortBatches();

        inline void drawBatch(Batch& batch, lgraphics::GeometryBuffer*& geomBuffer, lscene::Geometry*& geometry);

        lscene::Scene scene_;
        DrawableList drawableList_;

        BatchVector batches_;
        BatchVector alphaBatches_;

        bool drawablesChanged_;
    };

    inline lscene::Scene& PassMain::getScene()
    {
        return scene_;
    }
}
#endif //INC_LRENDER_PASSMAIN_H__
