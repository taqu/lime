#include "stdafx.h"
#include "DistanceField.h"

#include <math.h>

namespace
{
    inline float dot(float v0, float v1)
    {
        return (v0*v0 + v1*v1);
    }

    inline float dot(int v0, int v1)
    {
        return static_cast<float>(v0*v0 + v1*v1);
    }

    inline unsigned int sample(unsigned long* data, int index)
    {
        return ((data[index] & 0xFFU) >= 128)? 1 : 0;
    }

    inline float samplef(unsigned long* data, int index)
    {
        return ((data[index] & 0xFFU) >= 128)? 1.0f : 0.0f;
    }

    inline int absolute(int i)
    {
        return (i>=0)? i : -i;
    }
}

float signedDistance(int cx, int cy, float clamp, int width, int height, unsigned long* data)
{
    int w = width;
    int h = height;

    float cd = samplef(data, cy*w + cx) - 0.5f;
    int min_x = cx - static_cast<int>(clamp) - 1;
    if(min_x < 0){
        min_x = 0;
    }
    int max_x = cx + static_cast<int>(clamp) + 1;
    if(max_x>=w){
        max_x = w-1;
    }

    float distance = clamp;
    for(int y=0; y<=static_cast<int>(clamp) + 1; ++y){
        if(y>distance){
            continue;
        }

        int y1 = cy-y;
        if(y1>=0){
            for(int i=min_x; i<=max_x; ++i){
                if(absolute(i-cx)>distance){
                    continue;
                }
                float d = samplef(data, y1*w+i) - 0.5f;
                if(cd*d<0.0f){
                    float d2 = dot(y1-cy, i-cx);
                    if(d2 < distance*distance){
                        distance = sqrtf(d2);
                    }
                }
            }//for(int i=min_x
        }//if(y1>=0)

        int y2 = cy+y;
        if(y!=0 && y2<h){
            for(int i=min_x; i<=max_x; ++i){
                if(absolute(i-cx)>distance){
                    continue;
                }
                float d = samplef(data, y2*w+i) - 0.5f;
                if(cd*d<0.0f){
                    float d2 = dot(y2-cy, i-cx);
                    if(d2 < distance*distance){
                        distance = sqrtf(d2);
                    }
                }
            }//for(int i=min_x;
        }//if(y!=0

    }//for(int y=0;
    return (cd>0)? distance : -distance;
}
