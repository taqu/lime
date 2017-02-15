/**
@file PackReader.cpp
@author t-sakai
@date 2013/02/12 create
*/
#include "opus/PackReader.h"

namespace lsound
{

    //------------------------------------------
    //---
    //--- PackReader
    //---
    //------------------------------------------
    PackReader::PackReader()
    {
    }

    PackReader::~PackReader()
    {
        close();
    }

    // �t�@�C���I�[�v��
    bool PackReader::open(const Char* path)
    {
        LASSERT(NULL != path);
        bool isOpen = stream_.open(path, lcore::ios::in | lcore::ios::binary);
        if(isOpen){
            lcore::lsize_t size;
            dataSize_ = 0;

            size = lcore::io::read(stream_, pack_.header_);
            if(size<1){
                return false;
            }

            pack_.releaseEntries();
            pack_.entries_ = LNEW FileEntry[pack_.header_.numFiles_];

            u32 entrySize = pack_.header_.numFiles_ * sizeof(FileEntry);
            size = lcore::io::read(stream_, pack_.entries_, entrySize);
            if(size<1){
                return false;
            }

            //�f�[�^�T�C�Y�v�Z
            s32 dataTop = stream_.tellg();
            stream_.seekg(0, lcore::ios::end);
            dataSize_ = stream_.tellg() - dataTop;
            stream_.seekg(dataTop, lcore::ios::beg);
            pack_.dataTopOffset_ = dataTop;
        }
        return isOpen;
    }

    // �t�@�C���N���[�Y
    void PackReader::close()
    {
        stream_.close();
    }

    bool PackReader::readEntries(Pack& pack)
    {
        LASSERT(stream_.is_open());
        pack_.swap(pack);
        return true;
    }

    bool PackReader::readAll(Pack& pack)
    {
        LASSERT(stream_.is_open());

        pack_.releaseData();
        pack_.data_ = LNEW u8[dataSize_];
        u32 size = lcore::io::read(stream_, pack_.data_, dataSize_);
        if(size < 1){
            return false;
        }
        pack_.swap(pack);
        return true;
    }

}
