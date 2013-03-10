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

        /// �t�@�C���I�[�v��
        inline bool open(const Char* path);

        /// �t�@�C���N���[�Y
        inline void close();

        /// �w�b�_�[���[�h
        bool readHeader();

        /// �w�b�_�[�T�C�Y
        inline u32 getHeaderSize() const;

        /// �t�@�C�����擾
        inline u32 getNumFiles() const;

        /// �G���g���[�擾
        inline const FileEntry& getEntry(u32 index) const;

        /**
        @brief �G���g���[���[�h
        @return ����
        @param index ... 
        @param dst ... 
        */
        bool read(u32 index, void* dst);
    private:

        lcore::ifstream stream_;
        PackHeader header_;
        FileEntry* entries_;
    };

    // �t�@�C���I�[�v��
    inline bool PackReader::open(const Char* path)
    {
        LASSERT(NULL != path);
        return stream_.open(path, lcore::ios::binary);
    }

    // �t�@�C���N���[�Y
    inline void PackReader::close()
    {
        stream_.close();
    }

    // �w�b�_�[�T�C�Y
    inline u32 PackReader::getHeaderSize() const
    {
        return sizeof(PackHeader) + sizeof(FileEntry)*header_.numFiles_;
    }

    // �t�@�C�����擾
    inline u32 PackReader::getNumFiles() const
    {
        return header_.numFiles_;
    }

    // �G���g���[�擾
    inline const FileEntry& PackReader::getEntry(u32 index) const
    {
        LASSERT(0<=index && index<header_.numFiles_);
        return entries_[index];
    }
}
#endif //INC_PACK_PACKREADER_H__
