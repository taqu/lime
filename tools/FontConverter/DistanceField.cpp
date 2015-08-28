/**
@file DistanceField.cpp
@author t-sakai
@date 2015/08/06 create
*/
#include "DistanceField.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace df
{
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

    inline unsigned int sample(const unsigned char* data, int index)
    {
        return (data[index] >= 128)? 1 : 0;
    }

    inline float samplef(const unsigned char* data, int index)
    {
        return (data[index] >= 128)? 1.0f : 0.0f;
    }

    inline int absolute(int i)
    {
        return (i>=0)? i : -i;
    }

    float sampleClamp(const float* data, int x, int y, int w, int h)
    {
        if(x<0){
            x = 0;
        }else if(w<=x){
            x = w-1;
        }

        if(y<0){
            y = 0;
        }else if(h<=y){
            y = h-1;
        }
        return data[y*w + x];
    }
}


void rgb2gray(unsigned char* dst, int width, int height, int numChannels, const unsigned char* src)
{
    int n = (3<numChannels)? 3 : numChannels;
    for(int i=0; i<height; ++i){
        for(int j=0; j<width; ++j){
            int idst = (i*width+j);
            int isrc = idst*numChannels;

            unsigned char rgb[3] = {0, 0, 0};

            for(int k=0; k<n; ++k){
                rgb[k] = src[isrc+k];
            }
            int gray = static_cast<int>(rgb[0]*0.299f + rgb[1]*0.587f + rgb[2] * 0.144f + 0.5f);
            dst[idst] = static_cast<unsigned char>((gray<0)? 0 : (255<gray)? 255 : gray);
        }
    }
}

void gaussblur(float* dst, int width, int height, float sigma, const float* src)
{
    float kernel[3][3];
    float sum=0.0f;
    float r = 1.0f/(2.0f*M_PI*sigma*sigma);
    for(int i=-1; i<=1; ++i){
        for(int j=-1; j<=1; ++j){
            float d = sqrtf(i*i+j*j)/sqrtf(2.0f);
            float f = r*exp(-d*d/(2.0f*sigma*sigma));
            sum += f;
            kernel[i+1][j+1] = f;
        }
    }

    sum = 1.0f/sum;
    for(int i=-1; i<=1; ++i){
        for(int j=-1; j<=1; ++j){
            kernel[i+1][j+1] *= sum;
        }
    }

    for(int i=0; i<height; ++i){
        for(int j=0; j<width; ++j){

            dst[i*width+j] = 0.0f;
            for(int y=i-1; y<=i+1; ++y){
                for(int x=j-1; x<=j+1; ++x){
                    dst[i*width+j] += kernel[x-j+1][y-i+1]*sampleClamp(src, x, y, width, height);
                }
            }
        }
    }
}

float signedDistance(int cx, int cy, float clamp, int width, int height, int channel, int numChannels, const unsigned char* data)
{
    int w = width;
    int h = height;

    data += channel;

    float cd = samplef(data, (cy*w + cx)*numChannels) - 0.5f;
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
                float d = samplef(data, (y1*w+i)*numChannels) - 0.5f;
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
                float d = samplef(data, (y2*w+i)*numChannels) - 0.5f;
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
}
