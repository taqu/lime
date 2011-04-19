/**
@file IOJPEG.cpp
@author t-sakai
@date 2011/01/08
*/
#include <stdio.h>
#include <setjmp.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef XMD_H
#ifndef _BASETSD_H_
#ifndef _BASETSD_H
#ifndef QGLOBAL_H
#define XMD_H
#endif
#endif
#endif
#endif

#include <libjpeg/jpeglib.h>
#ifdef __cplusplus
}
#endif

#include "../lgraphicsAPIInclude.h"

#include "IOJPEG.h"
#include "../api/Enumerations.h"
#include "../api/TextureRef.h"

namespace lgraphics
{
namespace io
{
    namespace
    {
        static const s32 JUMP_OK = 0;
        static const s32 JUMP_ERROR = 1;

        void jpg_out_msg(j_common_ptr cinfo)
        {
            Char buffer[JMSG_LENGTH_MAX];
            buffer[JMSG_LENGTH_MAX-1] = '\0';

            (*cinfo->err->format_message)(cinfo, buffer);
            lcore::Log("%s", buffer);
        }

        void jpg_error_return(j_common_ptr cinfo)
        {
            jpg_out_msg(cinfo);

            jmp_buf *jbuf = (jmp_buf*)cinfo->client_data;
            longjmp(*jbuf, JUMP_ERROR);
        }

#if defined(LIME_GL)
        bool readJPG(TextureRef& dst, jpeg_decompress_struct& cinfo)
        {
            //long jump用ステートセット
            jmp_buf jbuf;
            cinfo.client_data = (void*)&jbuf;

            u8* buffer = NULL;
            bool ret = true;
            if(JUMP_OK == setjmp(jbuf)){
                jpeg_read_header(&cinfo, TRUE);


                jpeg_start_decompress(&cinfo);

                u32 width = cinfo.output_width;
                u32 height = cinfo.output_height;
                s32 component = cinfo.output_components;

                BufferFormat format;
                switch(component)
                {
                case 3:
                    format = Buffer_B8G8R8;
                    break;

                case 1:
                    format = Buffer_L8;
                    break;

                default:
                    return false;
                };

                dst = Texture::create(width, height, 1, Usage_None, format, Pool_Managed);
                if(dst.valid() == false){
                    return false;
                }

                u32 pitch = width*component;
                buffer = LIME_NEW u8[ pitch*height ];

                JSAMPROW rows[1];

                rows[0] = reinterpret_cast<JSAMPLE*>( buffer + pitch*(height-1) );

                while(cinfo.output_scanline < cinfo.output_height){
                    jpeg_read_scanlines(&cinfo, rows, 1);
                    rows[0] -= pitch;
                }

                dst.blit(0, buffer);

            }else{
                ret = false;
            }

            LIME_DELETE_ARRAY(buffer);
            return ret;
        }
#else
        bool readJPG(jpeg_decompress_struct& cinfo)
        {
        }
#endif
    }

    IOJPEG::IOJPEG()
    {
    }

    IOJPEG::~IOJPEG()
    {
    }

    bool IOJPEG::read(const Char* filepath, TextureRef& texture)
    {
        LASSERT(filepath != NULL);

        FILE *file = fopen(filepath, "rb");
        if(NULL == file){
            return false;
        }

        jpeg_decompress_struct cinfo;
        jpeg_error_mgr jerror;

        // エラー処理初期化
        cinfo.err = jpeg_std_error(&jerror);
        jerror.error_exit = jpg_error_return; //エラー終了コールバックセット
        jerror.output_message = jpg_out_msg;  //メッセージ出力コールバックセット


        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, file);
        bool ret = readJPG(texture, cinfo);

        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        fclose(file);
        return ret;
    }

}
}
