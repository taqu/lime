#ifndef INC_LRENDER_PASSMAIN_H__
#define INC_LRENDER_PASSMAIN_H__
/**
@file PassMain.h
@author t-sakai
@date 2010/11/22 create

*/
#include <lcore/Vector.h>
#include "../scene/Scene.h"

namespace lrender
{
    class Batch;

    class PassMain
    {
    public:
        PassMain();
        ~PassMain();

        inline lscene::Scene& getScene();

        void add(Batch* batch);
        void remove(Batch* batch);

        void draw();
    private:
        typedef lcore::vector_arena<Batch*> BatchVector;

        lscene::Scene scene_;

        BatchVector batches_;
        BatchVector alphaBatches_;
    };

    inline lscene::Scene& PassMain::getScene()
    {
        return scene_;
    }
}
#endif //INC_LRENDER_PASSMAIN_H__
