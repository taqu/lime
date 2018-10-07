/**
@file IODDS.cpp
@author t-sakai
@date 2012/07/26 create
*/
#include "lgraphics.h"

#include "io/IODDS.h"
#include "TextureRef.h"

namespace lgfx
{
namespace io
{
    namespace
    {
        enum DDSD
        {
            DDSD_CAPS = 0x00000001,
            DDSD_HEIGHT = 0x00000002,
            DDSD_WIDTH = 0x00000004,
            DDSD_PITCH = 0x00000008,
            DDSD_PIXELFORMAT = 0x00001000,
            DDSD_MIPMAPCOUNT = 0x00020000,
            DDSD_LINEARSIZE = 0x00080000,
            DDSD_DEPTH = 0x00800000,
        };

        enum DDPF
        {
            DDPF_ALPHAPIXELS = 0x00000001,
            DDPF_ALPHA = 0x00000002,
            DDPF_FOURCC = 0x00000004,
            DDPF_RGB = 0x00000040,
            DDPF_RGBA = 0x00000041,
            DDPF_YUV = 0x200,
            DDPF_LUMINANCE = 0x20000,
        };

        enum DDSCAPS
        {
            DDSCAPS_COMPLEX = 0x00000008,
            DDSCAPS_TEXTURE = 0x00001000,
            DDSCAPS_MIPMAP = 0x00400000,
        };

        enum DDSCAPS2
        {
            DDSCAPS2_CUBEMAP = 0x00000200,
            DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
            DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
            DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
            DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
            DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
            DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
            DDSCAPS2_VOLUME = 0x00200000,
        };

        enum DXFOURCC
        {
            DXFOURCC_DXT1 = LMAKE_FOURCC('D', 'X', 'T', '1'),
            DXFOURCC_DXT3 = LMAKE_FOURCC('D', 'X', 'T', '3'),
            DXFOURCC_DXT5 = LMAKE_FOURCC('D', 'X', 'T', '5'),

            DXFOURCC_ATI1 = LMAKE_FOURCC('A', 'T', 'I', '1'),
            DXFOURCC_BC4U = LMAKE_FOURCC('B', 'C', '4', 'U'),
            DXFOURCC_BC4S = LMAKE_FOURCC('B', 'C', '4', 'S'),

            DXFOURCC_ATI2 = LMAKE_FOURCC('A', 'T', 'I', '2'),
            DXFOURCC_BC5U = LMAKE_FOURCC('B', 'C', '5', 'U'),
            DXFOURCC_BC5S = LMAKE_FOURCC('B', 'C', '5', 'S'),

            DXFOURCC_BC6U = LMAKE_FOURCC('B', 'C', '6', 'U'),
            DXFOURCC_BC6S = LMAKE_FOURCC('B', 'C', '6', 'S'),

            DXFOURCC_BC7U = LMAKE_FOURCC('B', 'C', '7', 'U'),
        };

        struct DDS_HEADER_DXT10
        {
            u32 format_;
            u32 dimension_;
            u32 miscFlag_;
            u32 arraySize_;
            u32 reserved_;
        };

        // old format
        enum D3DFMT
        {
            D3DFMT_A16B16G16R16 = 36,
            D3DFMT_R16F = 111,
            D3DFMT_G16R16F = 112,
            D3DFMT_A16B16G16R16F = 113,
            D3DFMT_R32F = 114,
            D3DFMT_G32R32F = 115,
            D3DFMT_A32B32G32R32F = 116,
        };

#define IsBitMask(r,g,b,a) ((r) == ddpf.RBitMask_ && (g) == ddpf.GBitMask_ && (b) == ddpf.BBitMask_)

        /**
        @param ddpf ... ピクセルフォーマット

        対応フォーマットはBufferFormatに入っているもののみ
        */
        DataFormat selectFormat(const IODDS::DDS_PIXELFORMAT& ddpf, bool sRGB)
        {
            if(ddpf.flags_ & DDPF_RGB){
                switch (ddpf.RGBBitCount_)
                {
                case 32:
                    if( IsBitMask(0x00ff0000,0x0000ff00,0x000000ff,0xff000000) ){
                        return (sRGB)? Data_B8G8R8A8_UNorm_SRGB : Data_B8G8R8A8_UNorm;
                    }

                    if( IsBitMask(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) ){
                        return (sRGB)? Data_B8G8R8X8_UNorm_SRGB : Data_B8G8R8X8_UNorm;
                    }
                    
                    if( IsBitMask(0x000000ff,0x0000ff00,0x00ff0000,0xff000000) ){
                        return (sRGB)? Data_R8G8B8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm;
                    }
                    
                    if( IsBitMask(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) ){
                        return (sRGB)? Data_R8G8B8A8_UNorm_SRGB : Data_R8G8B8A8_UNorm;
                    }

                    // Not support
                    if( IsBitMask(0x3ff00000,0x000ffc00,0x000003ff,0xc0000000) ){
                        return Data_Unknown;
                    }

                    if( IsBitMask(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) ){
                        return Data_R10G10B10A2_UNorm;
                    }

                    if( IsBitMask(0x0000ffff,0xffff0000,0x00000000,0x00000000) ){
                        return Data_R16G16_UNorm;
                    }

                    if( IsBitMask(0xffffffff,0x00000000,0x00000000,0x00000000) ){
                        return Data_R32_Float;
                    }
                    break;

                case 24:
                    // Not support B8G8A8
                    if( IsBitMask(0x00ff0000,0x0000ff00,0x000000ff,0x00000000) ){
                        return Data_Unknown;
                    }
                    break;

                case 16:
                    if( IsBitMask(0x0000f800,0x000007e0,0x0000001f,0x00000000) ){
                        return Data_B5G6R5_UNorm;
                    }

                    if( IsBitMask(0x00007c00,0x000003e0,0x0000001f,0x00008000) ){
                        return Data_B5G5R5A1_UNorm;
                    }

                    if( IsBitMask(0x00007c00,0x000003e0,0x0000001f,0x00000000) ){
                        return Data_B5G5R5A1_UNorm;
                    }

                    // Not support B4G4R4A4
                    if( IsBitMask(0x00000f00,0x000000f0,0x0000000f,0x0000f000) ){
                        return Data_Unknown;
                    }

                    // Not support B4G4R4X4
                    if( IsBitMask(0x00000f00,0x000000f0,0x0000000f,0x00000000) ){
                        return Data_Unknown;
                    }

                    // Not support B2G3R3A8
                    if( IsBitMask(0x000000e0,0x0000001c,0x00000003,0x0000ff00) ){
                        return Data_Unknown;
                    }
                    break;
                }

            }else if(ddpf.flags_ & DDPF_LUMINANCE){
                switch(ddpf.RGBBitCount_)
                {
                case 16:
                    if( IsBitMask(0x0000ffff,0x00000000,0x00000000,0x00000000) ){
                        return Data_R16_UNorm;
                    }

                    if( IsBitMask(0x000000ff,0x00000000,0x00000000,0x0000ff00) ){
                        return Data_R8G8_UNorm;
                    }
                    break;

                case 8:
                    //Not support A4L4
                    if( IsBitMask(0x0000000f,0x00000000,0x00000000,0x000000f0) ){
                        return Data_Unknown;
                    }

                    if( IsBitMask(0x000000ff,0x00000000,0x00000000,0x00000000) ){
                        return Data_R8_UNorm;
                    }
                    break;
                }

            }else if(ddpf.flags_ & DDPF_ALPHA){
                switch(ddpf.RGBBitCount_)
                {
                case 8:
                    return Data_A8_UNorm;
                    break;
                }

            }else if(ddpf.flags_ & DDPF_FOURCC){
                //圧縮形式
                switch(ddpf.fourCC_)
                {
                case DXFOURCC_DXT1:
                    return (sRGB)? Data_BC1_UNorm_SRGB : Data_BC1_UNorm;

                case DXFOURCC_DXT3:
                    return (sRGB)? Data_BC2_UNorm_SRGB : Data_BC2_UNorm;

                case DXFOURCC_DXT5:
                    return (sRGB)? Data_BC3_UNorm_SRGB : Data_BC3_UNorm;

                case DXFOURCC_ATI1:
                case DXFOURCC_BC4U:
                    return Data_BC4_UNorm;

                case DXFOURCC_BC4S:
                    return Data_BC4_SNorm;

                case DXFOURCC_ATI2:
                case DXFOURCC_BC5U:
                    return Data_BC5_UNorm;

                case DXFOURCC_BC5S:
                    return Data_BC5_SNorm;

                case DXFOURCC_BC6U:
                    return Data_BC6H_UF16;

                case DXFOURCC_BC6S:
                    return Data_BC6H_SF16;

                case DXFOURCC_BC7U:
                    return (sRGB)? Data_BC7_UNorm_SRGB : Data_BC7_UNorm;

                case D3DFMT_A16B16G16R16:
                    return Data_R16G16B16A16_UNorm;

                case D3DFMT_R16F:
                    return Data_R16_Float;

                case D3DFMT_G16R16F:
                    return Data_R16G16_Float;

                case D3DFMT_A16B16G16R16F:
                    return Data_R16G16B16A16_Float;

                case D3DFMT_R32F:
                    return Data_R32_Float;

                case D3DFMT_G32R32F:
                    return Data_R32G32_Float;

                case D3DFMT_A32B32G32R32F:
                    return Data_R32G32B32A32_Float;
                };
            }

            return Data_Unknown;
        }

        void getSize(DataFormat format, u32 width, u32 height, u32& size, u32& pitch)
        {
            bool compress = false;
            u32 minSize = 0;
            switch(format)
            {
            case Data_BC1_TypeLess:
            case Data_BC1_UNorm:
            case Data_BC1_UNorm_SRGB:

            case Data_BC4_TypeLess:
            case Data_BC4_UNorm:
            case Data_BC4_SNorm:
                compress = true;
                minSize = 8;
                break;

            case Data_BC2_TypeLess:
            case Data_BC2_UNorm:
            case Data_BC2_UNorm_SRGB:

            case Data_BC3_TypeLess:
            case Data_BC3_UNorm:
            case Data_BC3_UNorm_SRGB:

            case Data_BC5_TypeLess:
            case Data_BC5_UNorm:
            case Data_BC5_SNorm:

            case Data_BC6H_TypeLess:
            case Data_BC6H_UF16:
            case Data_BC6H_SF16:

            case Data_BC7_TypeLess:
            case Data_BC7_UNorm:
            case Data_BC7_UNorm_SRGB:
                compress = true;
                minSize = 16;
                break;
            };
            u32 numRows;
            if(compress){
                pitch = minSize * lcore::maximum<u32>(1, width>>2);
                numRows = lcore::maximum<u32>(1, height>>2);
            }else{
                pitch = (width * getBitsPerPixel(format) + 7) >> 3;
                numRows = height;
            }
            size = numRows * pitch;
        }

        void getSize(DataFormat format, u32 width, u32 height, u32 depth, u32& size, u32& pitch, u32& slicePitch)
        {
            bool compress = false;
            u32 minSize = 0;
            switch(format)
            {
            case Data_BC1_TypeLess:
            case Data_BC1_UNorm:
            case Data_BC1_UNorm_SRGB:

            case Data_BC4_TypeLess:
            case Data_BC4_UNorm:
            case Data_BC4_SNorm:
                compress = true;
                minSize = 8;
                break;

            case Data_BC2_TypeLess:
            case Data_BC2_UNorm:
            case Data_BC2_UNorm_SRGB:

            case Data_BC3_TypeLess:
            case Data_BC3_UNorm:
            case Data_BC3_UNorm_SRGB:

            case Data_BC5_TypeLess:
            case Data_BC5_UNorm:
            case Data_BC5_SNorm:

            case Data_BC6H_TypeLess:
            case Data_BC6H_UF16:
            case Data_BC6H_SF16:

            case Data_BC7_TypeLess:
            case Data_BC7_UNorm:
            case Data_BC7_UNorm_SRGB:
                compress = true;
                minSize = 16;
                break;
            };
            u32 numRows;
            u32 numSlices;
            if(compress){
                pitch = minSize * lcore::maximum<u32>(1, width>>2);
                numRows = lcore::maximum<u32>(1, height>>2);
                slicePitch = pitch * numRows;
                numSlices = lcore::maximum<u32>(1, depth>>2);
            }else{
                pitch = (width * getBitsPerPixel(format) + 7) >> 3;
                numRows = height;
                slicePitch = pitch * numRows;
                numSlices = depth;
            }
            size = slicePitch * depth;
        }
    }

    bool IODDS::checkSignature(Stream& is)
    {
        s64 pos = is.tell();
        u32 magic;
        s32 count = is.read(sizeof(u32), &magic);
        is.seek(pos, lcore::ios::beg);
        if(count<1){
            return false;
        }
        return (magic == DDS_MAGIC);
    }

    bool IODDS::read(
        Texture2DRef& texture,
        s32 size,
        const u8* data,
        Usage usage,
        BindFlag bindFlag,
        CPUAccessFlag access,
        ResourceMisc misc,
        s32& width,
        s32& height,
        DataFormat& format)
    {
        static const u32 MaxNumSubResourceData = 256;
        SubResourceData initData[MaxNumSubResourceData];

        ISStream stream(size, data);

        u32 magic = 0;
        stream.read(magic);
        if(magic != DDS_MAGIC){
            return false;
        }

        DDS_HEADER header;
        if(!stream.read(header)){
            return false;
        }

        if(header.size_ != DDS_HEADER_SIZE){
            return false;
        }

        format = Data_Unknown;
        u32 arraySize = 0;
        DDS_HEADER_DXT10 header10;
        if((header.ddpf_.flags_ & DDPF_FOURCC) != 0
            && header.ddpf_.fourCC_ == DDS_DX10)
        {
            stream.read(header10);
            format = static_cast<DataFormat>(header10.format_);
            arraySize = header10.arraySize_;
        }else{
            format = selectFormat(header.ddpf_, true);

            if((header.caps2_ & DDSCAPS2_CUBEMAP) != 0){
                arraySize = 0;
                u32 cubeMapFlags[] =
                {
                    DDSCAPS2_CUBEMAP_POSITIVEX,
                    DDSCAPS2_CUBEMAP_NEGATIVEX,
                    DDSCAPS2_CUBEMAP_POSITIVEY,
                    DDSCAPS2_CUBEMAP_NEGATIVEY,
                    DDSCAPS2_CUBEMAP_POSITIVEZ,
                    DDSCAPS2_CUBEMAP_NEGATIVEZ,
                };
                for(s32 i=0; i<6; ++i){
                    if((header.caps2_ & cubeMapFlags[i]) != 0){
                        ++arraySize;
                    }
                }

            }else{
                arraySize = 1;
            }
        }

        if(format == Data_Unknown){
            return false;
        }

        u32 mipmapLevel = (header.mipmap_ > 0)? header.mipmap_ : 1;

        u32 numSubResources = arraySize * mipmapLevel;
        if(MaxNumSubResourceData<numSubResources){
            return false;
        }

        u32 bytes = 0;
        u32 pitch = 0;
        u32 w, h;
        const u8* mem = data + stream.tell();
        u32 index = 0;
        for(u32 i=0; i<arraySize; ++i){
            w = header.width_;
            h = header.height_;
            for(u32 j=0; j<mipmapLevel; ++j, ++index){
                getSize(format, w, h, bytes, pitch);
                initData[index].mem_ = mem;
                initData[index].pitch_ = pitch;
                initData[index].slicePitch_ = 0;
                mem += bytes;

                w = lcore::maximum<u32>(1, w>>1);
                h = lcore::maximum<u32>(1, h>>1);
            }
        }

        //u32 fileSize = (u32)mem - (u32)data;

        SRVDesc viewDesc;
        viewDesc.format_ = format;

        if(arraySize>1){
            viewDesc.dimension_ = SRVDimension_Cube;
            viewDesc.texCube_.mostDetailedMip_ = 0;
            viewDesc.texCube_.mipLevels_ = mipmapLevel;
            misc = (lgfx::ResourceMisc)(misc | lgfx::ResourceMisc_TextureCube);
        }else{
            viewDesc.dimension_ = SRVDimension_Texture2D;
            viewDesc.tex2D_.mostDetailedMip_ = 0;
            viewDesc.tex2D_.mipLevels_ = mipmapLevel;
        }

        width = header.width_;
        height = header.height_;
        texture = Texture::create2D(
            width,
            height,
            mipmapLevel,
            arraySize,
            format,
            usage,
            bindFlag,
            access,
            misc,
            initData);

        return true;
    }

    bool IODDS::read(
        Texture3DRef& texture,
        s32 size,
        const u8* data,
        Usage usage,
        BindFlag bindFlag,
        CPUAccessFlag access,
        ResourceMisc misc,
        s32& width,
        s32& height,
        s32& depth,
        DataFormat& format)
    {
        static const u32 MaxNumSubResourceData = 256;
        SubResourceData initData[MaxNumSubResourceData];

        ISStream stream(size, data);

        u32 magic = 0;
        stream.read(magic);
        if(magic != DDS_MAGIC){
            return false;
        }

        DDS_HEADER header;
        if(!stream.read(header)){
            return false;
        }

        if(header.size_ != DDS_HEADER_SIZE){
            return false;
        }

        format = Data_Unknown;
        u32 arraySize = 0;
        DDS_HEADER_DXT10 header10;
        if((header.ddpf_.flags_ & DDPF_FOURCC) != 0
            && header.ddpf_.fourCC_ == DDS_DX10)
        {
            stream.read(header10);
            format = static_cast<DataFormat>(header10.format_);
            arraySize = header10.arraySize_;
            if(1<arraySize){
                return false;
            }

        }else{
            format = selectFormat(header.ddpf_, true);

            if((header.caps2_ & DDSCAPS2_CUBEMAP) != 0){
                return false;
            }
            arraySize = 1;
        }

        if(format == Data_Unknown){
            return false;
        }

        u32 mipmapLevel = (header.mipmap_ > 0)? header.mipmap_ : 1;

        u32 numSubResources = arraySize * mipmapLevel;
        if(MaxNumSubResourceData<numSubResources){
            return false;
        }

        u32 bytes = 0;
        u32 pitch = 0;
        u32 slicePitch = 0;
        u32 w, h, d;
        const u8* mem = data + stream.tell();
        u32 index = 0;
        {
            w = header.width_;
            h = header.height_;
            d = header.depth_;
            for(u32 j=0; j<mipmapLevel; ++j, ++index){
                getSize(format, w, h, d, bytes, pitch, slicePitch);
                initData[index].mem_ = mem;
                initData[index].pitch_ = pitch;
                initData[index].slicePitch_ = slicePitch;
                mem += bytes;

                w = lcore::maximum<u32>(1, w>>1);
                h = lcore::maximum<u32>(1, h>>1);
                d = lcore::maximum<u32>(1, d>>1);
            }
        }

        SRVDesc viewDesc;
        viewDesc.format_ = format;

        {
            viewDesc.dimension_ = SRVDimension_Texture3D;
            viewDesc.tex3D_.mostDetailedMip_ = 0;
            viewDesc.tex3D_.mipLevels_ = mipmapLevel;
        }

        width = header.width_;
        height = header.height_;
        depth = header.depth_;
        texture = Texture::create3D(
            width,
            height,
            depth,
            mipmapLevel,
            format,
            usage,
            bindFlag,
            access,
            misc,
            initData);

        return true;
    }
}
}
