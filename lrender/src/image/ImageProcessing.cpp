/**
@file ImageProcessing.cpp
@author t-sakai
@date 2015/10/07 create
*/
#include "image/ImageProcessing.h"
#include "image/Image.h"

namespace lrender
{
    void ImageProcessing::dilate(Image& dst, const Image& src)
    {
        LASSERT(dst.getWidth() == src.getWidth());
        LASSERT(dst.getHeight() == src.getHeight());

        for(s32 i=0; i<src.getHeight(); ++i){
            for(s32 j=0; j<src.getWidth(); ++j){
                const Color4& sc = src.getRGBA(j, i);
                if(0.0f<sc[3]){
                    dst.set(sc, j, i);
                    continue;
                }

                s32 count = 0;
                s32 sx = lcore::maximum(j-1, 0);
                s32 ex = lcore::minimum(j+2, src.getWidth());
                s32 sy = lcore::maximum(i-1, 0);
                s32 ey = lcore::minimum(i+2, src.getHeight());
                Color4 sum = Color4::black();
                for(s32 y=sy; y<ey; ++y){
                    for(s32 x=sx; x<ex; ++x){
                        const Color4& c = src.getRGBA(x, y);
                        if(0.0f<c[3]){
                            ++count;
                            sum += c;
                        }
                    }
                }
                if(0<count){
                    sum /= count;
                    dst.set(sum, j, i);
                }else{
                    dst.set(Color4::black(), j, i);
                }
            }//for(s32 j=0;
        }//for(s32 i=0;
    }
}
