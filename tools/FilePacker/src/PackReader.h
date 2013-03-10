#ifndef INC_PACK_PACKREADER_H__
#define INC_PACK_PACKREADER_H__
/**
@file PackReader.h
@author t-sakai
@date 2012/03/20 create

*/
#include <lcore/liostream.h>
#include "Pack.h"

namespace pack
{
    class PackReader
    {
    public:
        PackReader();
        ~PackReader();

        /// ファイルオープン
        inline bool open(const Char* path);

        /// ファイルクローズ
        inline void close();

        /// ヘッダーロード
        bool readHeader();

        /// ヘッダーサイズ
        inline u32 getHeaderSize() const;

        /// ファイル数取得
        inline u32 getNumFiles() const;

        /// エントリー取得
        inline const FileEntry& getEntry(u32 index) const;

        /**
        @brief エントリーロード
        @return 成否
        @param index ... 
        @param dst ... 
        */
        bool read(u32 index, void* dst);
    private:

        lcore::ifstream stream_;
        PackHeader header_;
        FileEntry* entries_;
    };

    // ファイルオープン
    inline bool PackReader::open(const Char* path)
    {
        LASSERT(NULL != path);
        return stream_.open(path, lcore::ios::binary);
    }

    // ファイルクローズ
    inline void PackReader::close()
    {
        stream_.close();
    }

    // ヘッダーサイズ
    inline u32 PackReader::getHeaderSize() const
    {
        return sizeof(PackHeader) + sizeof(FileEntry)*header_.numFiles_;
    }

    // ファイル数取得
    inline u32 PackReader::getNumFiles() const
    {
        return header_.numFiles_;
    }

    // エントリー取得
    inline const FileEntry& PackReader::getEntry(u32 index) const
    {
        LASSERT(0<=index && index<header_.numFiles_);
        return entries_[index];
    }
}
#endif //INC_PACK_PACKREADER_H__
