#ifndef INC_LRENDER_SAMPLETABLE_H__
#define INC_LRENDER_SAMPLETABLE_H__
/**
@file SampleTable.h
@author t-sakai
@date 2015/11/26 create
*/
#include "../lrender.h"

namespace lrender
{
    class SampleTable
    {
    public:
        SampleTable();
        ~SampleTable();

        template<class T>
        void create(s32 resolution, T func, f32 x0, f32 x1, f32 epsilon);

        f32 interpolateLinear(f32 x) const;
    private:
        SampleTable(const SampleTable&);
        SampleTable& operator=(const SampleTable&);

        s32 resolution_;
        f32* table_;

        template<class T>
        static f32 estimate(T func,  f32 v, f32 x0, f32 x1, f32 epsilon);
    };

    template<class T>
    void SampleTable::create(s32 resolution, T func, f32 x0, f32 x1, f32 epsilon)
    {
        if(resolution_<resolution){
            resolution_ = resolution;
            LDELETE_ARRAY(table_);
            table_ = LNEW f32[resolution_+1];
        }

        f32 inv = 1.0f/resolution_;
        f32*  definitions = LNEW f32[resolution_+1];
        for(s32 i=0; i<=resolution_; ++i){
            definitions[i] = func(inv*i);
        }

        for(s32 i=0; i<=resolution_; ++i){
            f32 x = inv*i;

            s32 j;
            for(j=0; definitions[j]<x; ++j);
            
            f32 weight = (0<j)? (x - definitions[j-1])/(definitions[j]-definitions[j-1]) : 0.0f;
            table_[i] = (weight*j + (1.0f-weight)*(j-1)) * inv;
        }
        LDELETE_ARRAY(definitions);
    }

    template<class T>
    f32 SampleTable::estimate(T func, f32 v, f32 x0, f32 x1, f32 epsilon)
    {
        s32 count = 0;
        f32 x2;
        f32 v0 = func(x0) - v;
        f32 v1 = func(x1) - v;
        while(epsilon < lcore::absolute(v1)) {
            f32 d = v1-v0;
            if(lcore::absolute(d) < 1.0e-6f) {
                break;
            }
            x2 = x1 - v1*(x1-x0)/d;
            x0 = x1;
            x1 = x2;
            if(50 < ++count) {
                break;
            }
            v0 = v1;
            v1 = func(x1)-v;
        }
        return x1;
    }
}
#endif //INC_LRENDER_SAMPLETABLE_H__
