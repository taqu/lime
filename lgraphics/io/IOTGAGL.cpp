/**
@file IOTGADX.cpp
@author t-sakai
@date 2010/05/20 create

OpenGL�p
*/
namespace lgraphics
{
namespace
{
    /**
    @brief ��������̘A�������o�C�g���A2�o�C�g�����ɕϊ�
    @param low ... �������A�h���X�I�ɉ���
    @param high ... �������A�h���X�I�ɏ��
    */
    inline u16 getU16(u8 low, u8 high)
    {
        return (static_cast<u16>(high)<<8) + low;
    }


    //-------------------------------------------------------------------------
    void read32(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //�o�b�t�@�Ƀ��[�h
        u32 pitch = width * bpp;
        u32 size = pitch*height;
        lcore::io::read(src, buffer, size);
    }

    void read32RLE(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //�o�b�t�@�Ƀ��[�h
        u32 pixels = width*height;
        u8 tmp[bpp];
        u8 byte;
        for(u32 i=0; i<pixels;){
            lcore::io::read(src, byte);
            //�ŏ�ʃr�b�g���O�Ȃ�A�����Ȃ��f�[�^��
            //�P�Ȃ�A������f�[�^��
            bool b = ((byte & 0x80U) == 0)? false : true;
            u32 count = (byte & 0x7FU) + 1;
            if(b){
                //�A������f�[�^
                lcore::io::read(src, tmp, bpp);
                for(u32 j=0; j<count; ++j){
                    for(u32 k=0; k<bpp; ++k){
                        *buffer = tmp[k];
                        ++buffer;
                    }
                }
            }else{
                //�A�����Ȃ��f�[�^
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, buffer, bpp);
                    buffer += bpp;
                }
            }
            i += count;
        }
    }


    void read24(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //�o�b�t�@�Ƀ��[�h
        u32 pitch = width * bpp;
        u32 size = pitch*height;
        lcore::io::read(src, buffer, size);
    }

    void read24RLE(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //�o�b�t�@�Ƀ��[�h���ē]��
        u32 pixels = width*height;
        u8 tmp[bpp];
        for(u32 i=0; i<pixels;){
            u8 byte;
            lcore::io::read(src, byte);
            //�ŏ�ʃr�b�g���O�Ȃ�A�����Ȃ��f�[�^��
            //�P�Ȃ�A������f�[�^��
            bool b = ((byte & 0x80U) == 0)? false : true;
            u32 count = (byte & 0x7FU) + 1;
            if(b){
                //�A������f�[�^
                lcore::io::read(src, tmp, bpp);
                for(u32 j=0; j<count; ++j){
                    buffer[0] = tmp[0];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[2];
                    buffer += bpp;
                }
            }else{
                //�A�����Ȃ��f�[�^
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    buffer[0] = tmp[0];
                    buffer[1] = tmp[1];
                    buffer[2] = tmp[2];
                    buffer += bpp;
                }
            }
            i += count;
        }
    }


    //-------------------------------------------------------------------------
    void read32Transpose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //�o�b�t�@�Ƀ��[�h���ē]��
        u32 rowBytes = width * bpp;

        //�]�n����
        u8* tmpBuffer = buffer + rowBytes * (height-1);
        u8 tmp[bpp];
        for(u32 i=0; i<height; ++i){

            u8* row = tmpBuffer;
            for(u32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                row[0] = tmp[2];
                row[1] = tmp[1];
                row[2] = tmp[0];
                row[3] = tmp[3];
                row += bpp;
            }
            tmpBuffer -= rowBytes;
        }
    }

    void read32RLETranspose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 4;

        //�o�b�t�@�Ƀ��[�h���ē]��
        u32 rowBytes = width * bpp;

        //�]�n����
        u32 pixels = width*height;
        u8* tmpBuffer = buffer + rowBytes * (height-1);
        u8* row = tmpBuffer;
        u32 x = 0;
        u32 count = 0;
        u8 byte = 0;
        u8 tmp[bpp];
        for(u32 i=0; i<pixels;){

            lcore::io::read(src, byte);
            //�ŏ�ʃr�b�g���O�Ȃ�A�����Ȃ��f�[�^��
            //�P�Ȃ�A������f�[�^��
            count = (byte & 0x7FU) + 1;
            bool b = ((byte & 0x80U) == 0)? false : true;
            if(b){
                //�A������f�[�^
                lcore::io::read(src, tmp, bpp);
                for(u32 j=0; j<count; ++j){

                    row[0] = tmp[2];
                    row[1] = tmp[1];
                    row[2] = tmp[0];
                    row[3] = tmp[3];

                    row += bpp;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= rowBytes;
                        row = tmpBuffer;
                    }
                }
            }else{
                //�A�����Ȃ��f�[�^
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, tmp, bpp);
                    row[0] = tmp[2];
                    row[1] = tmp[1];
                    row[2] = tmp[0];
                    row[3] = tmp[3];

                    row += bpp;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= rowBytes;
                        row = tmpBuffer;
                    }
                }
            }
            i += count;
        }
    }


    void read24Transpose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //�o�b�t�@�Ƀ��[�h���ē]��
        u32 rowBytes = width * bpp;

        //�]�n����
        u8* tmpBuffer = buffer + rowBytes * (height-1);
        u8 tmp[bpp];
        for(u32 i=0; i<height; ++i){

            u8* row = tmpBuffer;
            for(u32 j=0; j<width; ++j){
                lcore::io::read(src, tmp, bpp);
                row[0] = tmp[0];
                row[1] = tmp[1];
                row[2] = tmp[2];
                row += bpp;
            }
            tmpBuffer -= rowBytes;
        }
    }

    void read24RLETranspose(u8* buffer, lcore::istream& src, u32 width, u32 height)
    {
        static const u32 bpp = 3;

        //�o�b�t�@�Ƀ��[�h���ē]��
        u32 rowBytes = width * bpp;

        //�]�n����
        u32 pixels = width*height;
        u8* tmpBuffer = buffer + rowBytes * (height-1);
        u8* row = tmpBuffer;
        u32 x = 0;
        u32 count = 0;
        u8 byte = 0;
        u8 tmp[bpp];
        for(u32 i=0; i<pixels;){
            lcore::io::read(src, byte);
            //�ŏ�ʃr�b�g���O�Ȃ�A�����Ȃ��f�[�^��
            //�P�Ȃ�A������f�[�^��
            count = (byte & 0x7FU) + 1;
            bool b = ((byte & 0x80U) == 0)? false : true;

            if(b){
                //�A������f�[�^
                lcore::io::read(src, tmp, bpp);
                for(u32 j=0; j<count; ++j){
                    row[0] = tmp[0];
                    row[1] = tmp[1];
                    row[2] = tmp[2];
                    row += bpp;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= rowBytes;
                        row = tmpBuffer;
                    }
                }

            }else{
                //�A�����Ȃ��f�[�^
                for(u32 j=0; j<count; ++j){
                    lcore::io::read(src, row, bpp);
                    row += bpp;

                    if(++x>=width){
                        x = 0;
                        tmpBuffer -= rowBytes;
                        row = tmpBuffer;
                    }
                }
            }
            i += count;
        }
    }

}
}
