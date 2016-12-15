#ifndef INC_LCORE_VFSPACK_H__
#define INC_LCORE_VFSPACK_H__
/**
@file VFSPack.h
@author t-sakai
@date 2016/10/31 create
*/
#include "lio.h"

namespace lcore
{
    class DirectoryProxy;

    static const u32 VFSPackSignature = 'LPAK';

    struct VFSPackHeader
    {
        u32 signature_;
        u32 reserved_;
        s32 numEntries_;
        s64 offsetString_;
        s64 offsetData_;
    };

    struct VFSFileData
    {
        u16 type_;
        u16 flags_;
        s32 nameOffset_;
        s32 nameLength_;
        s64 dataOffset_;
        s64 dataUncompressedSize_;
        s64 dataCompressedSize_;
    };

    struct VFSDirectoryData
    {
        u16 type_;
        u16 flags_;
        s32 nameOffset_; 
        s32 nameLength_;
        s32 childOffset_;
        s32 childSize_;
    };

    struct VFSPackFooter
    {
        u32 filehash_;
    };

    union VFSData
    {
        VFSFileData file_;
        VFSDirectoryData directory_;
    };

    struct VFSPack
    {
        LHANDLE handle_;
        s32 numEntries_;
        u8* entries_;
        Char* stringBuffer_;
    };

    bool writeVFSPack(const Char* filepath, const Char* root);
    bool readVFSPack(VFSPack& pack, const Char* filepath, bool checkHash);
}
#endif //INC_LCORE_VFSPACK_H__
