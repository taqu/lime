/**
@file LoaderRadiance.cpp
@author t-sakai
@date 2013/05/22 create
*/
#include "loader/LoaderRadiance.h"
#include <stdio.h>
#include <lcore/liostream.h>
#include "image/Image.h"

namespace lrender
{
namespace
{
    static const s32 RGBE_COMPONENT_R = 0;
    static const s32 RGBE_COMPONENT_G = 1;
    static const s32 RGBE_COMPONENT_B = 2;
    static const s32 RGBE_COMPONENT_A = 3;
    static const s32 RGBE_NUM_COMPONENTS = 3;

    class Buffer
    {
    public:
        Buffer()
            :buffer_(NULL)
        {}

        ~Buffer()
        {
            LDELETE_ARRAY(buffer_);
        }

        bool create(s32 size)
        {
            buffer_ = LNEW u8[size];
            return (NULL != buffer_);
        }

        u8& operator[](s32 index){ return buffer_[index];}
        u8 operator[](s32 index) const{ return buffer_[index];}

        u8* buffer_;
    };

    void rgbe2float(f32& r, f32& g, f32& b, const u8 rgbe[4])
    {
        f32 f;
        if(0 != rgbe[3]){
            f = ldexp(1.0, rgbe[3] - (s32)(128+8));
            r = rgbe[0] * f;
            g = rgbe[1] * f;
            b = rgbe[2] * f;

        }else{
            r = g = b = 0.0f;
        }
    }

    void float2rgbe(u8 rgbe[4], f32 r, f32 g, f32 b)
    {
        f32 v = lcore::maximum(r, g);
        v = lcore::maximum(v, b);

        if(v<1e-32){
            rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
        }else{
            s32 e;
            v = frexp(v, &e) * 256.0f/v;
            rgbe[0] = (u8)(r*v);
            rgbe[1] = (u8)(g*v);
            rgbe[2] = (u8)(b*v);
            rgbe[3] = (u8)(e + 128);
        }
    }

    bool readLine(Char* buffer, s32 bufferLen, lcore::istream& in)
    {
        buffer[0] = CharNull;

        for(s32 i=0; i<bufferLen; ++i){
            Char c;
            if(1 != in.read(&c, 1)){
                return false;
            }
            if('\n' == c){
                buffer[i] = CharNull;
                break;
            }
            buffer[i] = c;
        }
        return true;
    }

    s32 readRaw(lcore::istream& in, Color4* data, s32 numPixels)
    {
        u8 rgbe[4];
        while(--numPixels >= 0){
            if(1 != in.read((Char*)rgbe, 4)){
                return LoaderRadiance::Code_ReadError;
            }

            Color4& rgb = *data;
            rgbe2float(
                rgb[RGBE_COMPONENT_R],
                rgb[RGBE_COMPONENT_G],
                rgb[RGBE_COMPONENT_B],
                rgbe);
            rgb[RGBE_COMPONENT_A] = 1.0f;
            //data += RGBE_NUM_COMPONENTS;
            data += 1;
        }
        return LoaderRadiance::Code_Success;
    }
}

    s32 LoaderRadiance::readHeader(lcore::istream& in, f32& gamma, f32& exposure, s32& width, s32& height)
    {
        static const s32 BufferSize = 128;
        Char buffer[BufferSize+1];
        buffer[BufferSize] = CharNull;

        if(!readLine(buffer, BufferSize, in)){
            return Code_ReadError;
        }

        gamma = 1.0f;
        exposure = 1.0f;
        bool validFormat = false;
        for(;;){
            if(!readLine(buffer, BufferSize, in)){
                return Code_ReadError;
            }

            if(CharNull == buffer[0]){
                //read blank line
                break;
            }

            if(0 == strcmp(buffer, "FORMAT=32-bit_rle_rgbe")){
                validFormat = true;
                continue;
            }

            f32 temp;
            if(1 == sscanf_s(buffer, "GAMMA=%g", &temp)){
                gamma = temp;
            }
            if(1 == sscanf_s(buffer, "EXPOSURE=%g", &temp)){
                exposure = temp;
            }
        }
        if(!validFormat){
            return Code_ReadError;
        }

        if(!readLine(buffer, BufferSize, in)){
            return Code_ReadError;
        }

        s32 w, h;
        if(2 > sscanf_s(buffer, "-Y %d +X %d", &h, &w)){
            return Code_ReadError;
        }

        width = w;
        height = h;
        return Code_Success;
    }


    s32 LoaderRadiance::readRLE(lcore::istream& in, Image& image)
    {
        Color4* data = image.getData();

        LASSERT(NULL != data);

        s32 width = image.getWidth();
        s32 height = image.getHeight();

        if(width<8 || 0x7FFF < width){
            return readRaw(in, data, width*height);
        }

        u8 rgbe[4];
        u8 bytes2[2];

        Buffer buffer;
        if(!buffer.create(4*width)){
            return Code_ReadError;
        }

        while(0 < height){
            if(1 != in.read((Char*)rgbe, 4)){
                return Code_ReadError;
            }

            if((rgbe[0] != 2) || (rgbe[1] != 2) || (rgbe[2] & 0x80U)){
                //not run length

                Color4& rgb = *data;
                rgbe2float(
                    rgb[RGBE_COMPONENT_R],
                    rgb[RGBE_COMPONENT_G],
                    rgb[RGBE_COMPONENT_B],
                    rgbe);
                rgb[RGBE_COMPONENT_A] = 1.0f;
                //data += RGBE_NUM_COMPONENTS;
                data += 1;
                return readRaw(in, data, width*height);
            }

            s32 w = ((s32)rgbe[2] << 8) | rgbe[3];

            if(w != width){
                return Code_ReadError;
            }

            u8* ptr = &buffer[0];
            for(s32 i=0; i<4; ++i){
                u8* ptr_end = &buffer[(i+1)*width];

                while(ptr < ptr_end){
                    if(1 != in.read((Char*)bytes2, 2)){
                        return Code_ReadError;
                    }

                    if(bytes2[0] > 128){
                        //run of the same value
                        s32 count = bytes2[0] - 128;
                        if(0 == count || count>(ptr_end - ptr)){
                            return Code_ReadError;
                        }
                        while(--count>=0){
                            *ptr = bytes2[1];
                            ++ptr;
                        }

                    }else{
                        //not run
                        s32 count = bytes2[0];
                        if(0 == count || count>(ptr_end - ptr)){
                            return Code_ReadError;
                        }
                        *ptr = bytes2[1];
                        ++ptr;
                        if(--count>0){
                            if(1 != in.read((Char*)ptr, count)){
                                return Code_ReadError;
                            }
                            ptr += count;
                        }
                    }
                } //while(ptr < ptr_end)
            } //for(s32 i=0;

            for(s32 i=0; i<width; ++i){
                rgbe[0] = buffer[i];
                rgbe[1] = buffer[i + width];
                rgbe[2] = buffer[i + 2*width];
                rgbe[3] = buffer[i + 3*width];

                Color4& rgb = *data;
                rgbe2float(
                    rgb[RGBE_COMPONENT_R],
                    rgb[RGBE_COMPONENT_G],
                    rgb[RGBE_COMPONENT_B],
                    rgbe);
                rgb[RGBE_COMPONENT_A] = 1.0f;
                //data += RGBE_NUM_COMPONENTS;
                data += 1;
            }
            --height;
        } //while(0 < height)

        return Code_Success;
    }
}
