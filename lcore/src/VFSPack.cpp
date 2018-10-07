/**
@file VFSPack.cpp
@author t-sakai
@date 2016/10/31 create
*/
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#endif

#include "VFSPack.h"
#include "FileSystem.h"
#include "File.h"
#include "Array.h"
#include "LString.h"
#include "Random.h"
#include "xxHash.h"

namespace lcore
{
namespace
{
    bool readFile(HANDLE handle, LPVOID buffer, DWORD numberOfBytesToRead)
    {
        DWORD numberOfBytesRead = 0;
        return ReadFile(handle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL)
            && numberOfBytesToRead == numberOfBytesRead;
    }

    bool writeFile(HANDLE handle, LPCVOID buffer, DWORD numberOfBytesToWrite)
    {
        DWORD numberOfBytesWritten = 0;
        return WriteFile(handle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL)
            && numberOfBytesToWrite == numberOfBytesWritten;
    }

    void release(s32 numEntries, u8* entries)
    {
        u32 entrySize = fs::getEntrySize();

        for(s32 i=0; i<numEntries; ++i){
            fs::Descriptor* desc = reinterpret_cast<fs::Descriptor*>(entries);
            switch(desc->getType()){
            case fs::Type_File:{
                fs::File* file = static_cast<fs::File*>(desc);
                file->~File();
            }
                break;

            case fs::Type_Directory:{
                fs::Directory* directory = static_cast<fs::Directory*>(desc);
                directory->~Directory();
            }
                break;
            }
            entries += entrySize;
        }
    }

    void setName(fs::Descriptor& desc, VFSData& vfsData, s32 stringBufferLength, Char* stringBuffer)
    {
        s32 length = vfsData.file_.nameOffset_ + vfsData.file_.nameLength_;
        if(length<0 || stringBufferLength<length){
            desc.setName(0, stringBuffer, true);
            return;
        }
        Char* name = stringBuffer+vfsData.file_.nameOffset_;
        desc.setName(vfsData.file_.nameLength_, name, true);
    }

    class ScopedHandle
    {
    public:
        explicit ScopedHandle(HANDLE handle)
            :handle_(handle)
        {}

        ~ScopedHandle()
        {
            CloseHandle(handle_);
        }

        HANDLE release()
        {
            HANDLE ret = handle_;
            handle_ = NULL;
            return ret;
        }

        HANDLE handle_;
    };

    //----------------------------------------------
    //---
    //--- Entry
    //---
    //----------------------------------------------
    class Entry
    {
    public:
        struct FileData
        {
            s64 dataUncompressedSize_;
            s64 dataCompressedSize_;
        };
        struct DirectoryData
        {
            s32 childSize_;
        };
        union Data
        {
            FileData file_;
            DirectoryData directory_;
        };

        Entry();
        ~Entry();
        u16 getType() const;
        u16 getFlags() const;
        void setFlags(u16 flags);
        s32 getPathLength() const;
        const Char* getPath() const;
        void setPath(const Char* path);
        s32 getNameLength() const;
        const Char* getName() const;
        void setName(const Char* name);

        void setFile(s64 dataSize);
        void setDirectory(s32 childSize);
        const Data& getData() const;
    private:
        Entry(const Entry&);
        Entry& operator=(const Entry&);

        u16 type_;
        u16 flags_;
        Char* name_;
        Char* path_;
        Data data_;
    };

    Entry::Entry()
        :type_(0)
        ,flags_(0)
        ,name_(NULL)
        ,path_(NULL)
    {
        data_.file_.dataUncompressedSize_ = 0;
        data_.file_.dataCompressedSize_ = 0;
    }

    Entry::~Entry()
    {
        LDELETE_ARRAY(path_);
        LDELETE_ARRAY(name_);
    }

    u16 Entry::getType() const
    {
        return type_;
    }

    u16 Entry::getFlags() const
    {
        return flags_;
    }
    void Entry::setFlags(u16 flags)
    {
        flags_ = flags;
    }

    s32 Entry::getPathLength() const
    {
        return (NULL == path_)? 0 : lcore::strlen_s32(path_);
    }

    const Char* Entry::getPath() const
    {
        return path_;
    }
    void Entry::setPath(const Char* path)
    {
        LASSERT(NULL != path);
        LDELETE_ARRAY(path_);
        s32 len = lcore::strlen_s32(path);
        path_ = LNEW Char[len+1];
        lcore::memcpy(path_, path, len+1);
    }

    s32 Entry::getNameLength() const
    {
        return (NULL == name_)? 0 : lcore::strlen_s32(name_);
    }

    const Char* Entry::getName() const
    {
        return name_;
    }
    void Entry::setName(const Char* name)
    {
        LASSERT(NULL != name);
        LDELETE_ARRAY(name_);
        s32 len = lcore::strlen_s32(name);
        name_ = LNEW Char[len+1];
        lcore::memcpy(name_, name, len+1);
    }

    void Entry::setFile(s64 dataSize)
    {
        type_ = fs::Type_File;
        data_.file_.dataUncompressedSize_ = dataSize;
        data_.file_.dataCompressedSize_ = dataSize;
    }

    void Entry::setDirectory(s32 childSize)
    {
        type_ = fs::Type_Directory;
        data_.directory_.childSize_ = childSize;
    }

    const Entry::Data& Entry::getData() const
    {
        return data_;
    }

    //----------------------------------------------
    //---
    //---
    //---
    //----------------------------------------------
    class Traversal
    {
    public:
        Traversal();
        ~Traversal();

        Entry* createFile(const Char* parentPath, const Char* name, s64 dataSize);
        Entry* createDirectory(const Char* parentPath, const Char* name, s32 childSize);

        bool traverse(const Char* root);
        bool traverseDirectory(Entry* rootEntry);

        bool write(const Char* filepath);
    private:
        typedef Array<Entry*> EntryArray;
        bool createNextPath(String& path, const Char* parentPath, const Char* name);

        EntryArray entries_;
    };

    Traversal::Traversal()
    {
    }

    Traversal::~Traversal()
    {
        for(s32 i=0; i<entries_.size(); ++i){
            LDELETE(entries_[i]);
        }
        entries_.clear();
    }

    Entry* Traversal::createFile(const Char* parentPath, const Char* name, s64 dataSize)
    {
        LASSERT(NULL != parentPath);
        LASSERT(NULL != name);
        String path;
        if(!createNextPath(path, parentPath, name)){
            return NULL;
        }
        Entry* entry = LNEW Entry;
        entry->setPath(path.c_str());
        entry->setName(name);
        entry->setFile(dataSize);
        return entry;
    }

    Entry* Traversal::createDirectory(const Char* parentPath, const Char* name, s32 childSize)
    {
        LASSERT(NULL != parentPath);
        LASSERT(NULL != name);
        String path;
        if(!createNextPath(path, parentPath, name)){
            return NULL;
        }
        Entry* entry = LNEW Entry;
        entry->setPath(path.c_str());
        entry->setName(name);
        entry->setDirectory(childSize);
        return entry;
    }

    bool Traversal::traverse(const Char* root)
    {
        LASSERT(NULL != root);
        s32 exists = Path::existsType(root);
        if(!exists){
            return false;
        }

        if(Path::Exists_Directory != exists){
            s32 fileLen, dirLen;
            fileLen = dirLen = lcore::strlen_s32(root);
            Char* filename = LNEW Char[fileLen+1];
            Char* dirpath = LNEW Char[dirLen+1];
            fileLen = Path::extractFileName(filename, fileLen, root);
            dirLen = Path::extractDirectoryPath(dirpath, dirLen, root);
            Entry* ret = createFile(dirpath, filename, 0);
            this->~Traversal();

            Entry* rootEntry = createDirectory("/", "", 1);
            entries_.push_back(rootEntry);
            entries_.push_back(ret);
            LDELETE_ARRAY(dirpath);
            LDELETE_ARRAY(filename);
            return true;
        }

        Entry* rootEntry = NULL;
        s32 start = 0;
        {
            String path(root);
            if(path.length()<=0){
                return false;
            }
            if(lcore::PathDelimiter != path[path.length()-1]){
                path.append(2, "/*");
            } else{
                path.append('*');
            }

            WIN32_FIND_DATA data;
            HANDLE handle = FindFirstFile(path.c_str(), &data);
            if(INVALID_HANDLE_VALUE == handle){
                return false;
            }
            this->~Traversal();
            rootEntry = createDirectory("/", "", 1);
            entries_.push_back(rootEntry);
            start = entries_.size();
            do{
                Entry* entry = NULL;
                if(Path::isFile(data.dwFileAttributes)){
                    s64 size = data.nFileSizeHigh;
                    size = (size<<32) | data.nFileSizeLow;
                    entry = createFile(path.c_str(), data.cFileName, size);

                } else if(Path::isNormalDirectory(data.dwFileAttributes, data.cFileName)){
                    entry = createDirectory(path.c_str(), data.cFileName, 0);
                }
                if(NULL != entry){
                    entries_.push_back(entry);
                }
            } while(FindNextFile(handle, &data));
            FindClose(handle);
        }

        s32 end = entries_.size();
        rootEntry->setDirectory(end-start);
        for(s32 i=start; i<end; ++i){
            if(fs::Type_Directory == entries_[i]->getType()){
                traverseDirectory(entries_[i]);
            }
        }
        return true;
    }

    bool Traversal::traverseDirectory(Entry* rootEntry)
    {
        LASSERT(NULL != rootEntry);

        s32 start = 0;
        {
            String path(rootEntry->getPath());
            path.append(2, "/*");

            WIN32_FIND_DATA data;
            HANDLE handle = FindFirstFile(path.c_str(), &data);
            if(INVALID_HANDLE_VALUE == handle){
                return false;
            }
            start = entries_.size();
            do{
                Entry* entry = NULL;
                if(Path::isFile(data.dwFileAttributes)){
                    s64 size = data.nFileSizeHigh;
                    size = (size<<32) | data.nFileSizeLow;
                    entry = createFile(path.c_str(), data.cFileName, size);

                } else if(Path::isNormalDirectory(data.dwFileAttributes, data.cFileName)){
                    entry = createDirectory(path.c_str(), data.cFileName, 0);
                }
                if(NULL != entry){
                    entries_.push_back(entry);
                }
            } while(FindNextFile(handle, &data));
            FindClose(handle);
        }

        s32 end = entries_.size();
        rootEntry->setDirectory(end-start);
        for(s32 i=start; i<end; ++i){
            if(fs::Type_Directory == entries_[i]->getType()){
                traverseDirectory(entries_[i]);
            }
        }
        return true;
    }

    bool Traversal::createNextPath(String& path, const Char* parentPath, const Char* name)
    {
        LASSERT(NULL != parentPath);
        LASSERT(NULL != name);
        path.clear();
        s32 len = lcore::strlen_s32(parentPath);
        if(len<=0){
            return false;
        }
        if(parentPath[len-1] == '*'){
            --len;
        }
        if(len<=0){
            return false;
        }
        path.assign(len, parentPath);
        if(lcore::PathDelimiter != path[path.length()-1]){
            path.append(lcore::PathDelimiter);
        }
        path.append(lcore::strlen_s32(name), name);
        return true;
    }

    bool Traversal::write(const Char* filepath)
    {
        LASSERT(NULL != filepath);

        HANDLE handle = CreateFile(
            filepath,
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if(INVALID_HANDLE_VALUE == handle){
            return false;
        }
        VFSPackHeader header;
        {
            ScopedHandle scopedHandle(handle);

            header.signature_ = VFSPackSignature;
            cryptRandom(sizeof(header.reserved_), &header.reserved_);
            header.numEntries_ = entries_.size();
            header.offsetString_ = sizeof(VFSPackHeader) + sizeof(VFSData)*entries_.size();

            ScopedArrayPtr<VFSData> data(LNEW VFSData[entries_.size()]);

            //文字列テーブル作成
            String stringBuffer(entries_.size()*String::ExpandSize*2);
            for(s32 i=0; i<entries_.size(); ++i){
                data[i].file_.type_ = entries_[i]->getType();
                data[i].file_.flags_ = entries_[i]->getFlags();
                data[i].file_.nameOffset_ = stringBuffer.length();
                data[i].file_.nameLength_ = entries_[i]->getNameLength();

                stringBuffer.append(entries_[i]->getNameLength(), entries_[i]->getName());
                stringBuffer.append(lcore::CharNull);
            }

            //データエントリ作成
            header.offsetData_ = header.offsetString_ + sizeof(Char)*stringBuffer.length();
            s64 offsetData = header.offsetData_;
            s32 offsetChild = 1;
            for(s32 i=0; i<entries_.size(); ++i){
                const Entry::Data& entryData = entries_[i]->getData();
                switch(entries_[i]->getType())
                {
                case fs::Type_File:
                    data[i].file_.dataOffset_ = offsetData;
                    data[i].file_.dataUncompressedSize_ = entryData.file_.dataUncompressedSize_;
                    data[i].file_.dataCompressedSize_ = entryData.file_.dataCompressedSize_;
                    offsetData += entryData.file_.dataUncompressedSize_;
                    break;

                case fs::Type_Directory:
                    data[i].directory_.childOffset_ = offsetChild;
                    data[i].directory_.childSize_ = entryData.directory_.childSize_;
                    offsetChild += entryData.directory_.childSize_;
                    break;
                }
            }

            //書き出し
            if(!File::write(scopedHandle.handle_, sizeof(VFSPackHeader), &header)){
                return false;
            }
            s64 entrySize = sizeof(VFSData) * entries_.size();
            if(!File::write(scopedHandle.handle_, entrySize, data)){
                return false;
            }
            s64 stringSize = sizeof(Char) * stringBuffer.length();
            if(!File::write(scopedHandle.handle_, stringSize, stringBuffer.c_str())){
                return false;
            }
            bool result = true;
            s64 bufferSize = 0;
            u8* buffer = NULL;
            for(s32 i=0; i<entries_.size(); ++i){
                if(entries_[i]->getType() != fs::Type_File){
                    continue;
                }
                HANDLE h = CreateFile(
                    entries_[i]->getPath(),
                    GENERIC_READ,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
                if(INVALID_HANDLE_VALUE == h){
                    result = false;
                    break;
                }
                ScopedHandle entryHandle(h);

                LARGE_INTEGER size ={0};
                if(!GetFileSizeEx(entryHandle.handle_, &size)){
                    result = false;
                    break;
                }
                if(data[i].file_.dataUncompressedSize_ != size.QuadPart){
                    result = false;
                    break;
                }
                if(bufferSize<size.QuadPart){
                    LDELETE_ARRAY(buffer);
                    bufferSize = size.QuadPart;
                    buffer = LNEW u8[size.QuadPart];
                }
                if(!File::read(entryHandle.handle_, size.QuadPart, buffer)){
                    result = false;
                    break;
                }
                if(!File::write(scopedHandle.handle_, size.QuadPart, buffer)){
                    result = false;
                    break;
                }
            }
            LDELETE_ARRAY(buffer);
            if(!result){
                return false;
            }
        }

        //ハッシュ値計算
        handle = CreateFile(
            filepath,
            GENERIC_READ|GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if(INVALID_HANDLE_VALUE == handle){
            return false;
        }

        const s64 bufferSize = 4*1024;
        u8* buffer = LNEW u8[bufferSize];
        LARGE_INTEGER fileSize;
        if(!GetFileSizeEx(handle, &fileSize)){
            LDELETE_ARRAY(buffer);
            CloseHandle(handle);
            return false;
        }
        xxHashContext32 xxHashContext;
        xxHash32Init(xxHashContext, header.reserved_);
        s64 count = fileSize.QuadPart/bufferSize;
        bool result = true;
        for(s64 i=0; i<count; ++i){
            if(!readFile(handle, buffer, bufferSize)){
                result = false;
                break;
            }
            xxHash32Update(xxHashContext, buffer, bufferSize);
        }
        u32 rest = static_cast<u32>(fileSize.QuadPart - count*bufferSize);
        if(!readFile(handle, buffer, rest)){
            result = false;
        }
        xxHash32Update(xxHashContext, buffer, rest);
        VFSPackFooter footer;
        footer.filehash_ = xxHash32Finalize(xxHashContext);
        if(!writeFile(handle, &footer, sizeof(VFSPackFooter))){
            result = false;
        }
        LDELETE_ARRAY(buffer);
        CloseHandle(handle);
        return result;
    }

}

    //--------------------------------------------------------------------------
    bool writeVFSPack(const Char* filepath, const Char* root)
    {
        LASSERT(NULL != filepath);
        LASSERT(NULL != root);

        Traversal traversal;
        if(!traversal.traverse(root)){
            return false;
        }
        return traversal.write(filepath);
    }

    //-------------------------------------------------------------------------
    bool readVFSPack(VFSPack& pack, const Char* filepath, bool checkHash)
    {
        LASSERT(NULL != filepath);
        lcore::memset(&pack, 0, sizeof(VFSPack));

        HANDLE h = CreateFile(
            filepath,
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if(INVALID_HANDLE_VALUE == h){
            return false;
        }
        ScopedHandle handle(h);

        VFSPackHeader header;
        if(!readFile(handle.handle_, &header, sizeof(VFSPackHeader))){
            return false;
        }
        if(VFSPackSignature != header.signature_){
            return false;
        }
        if(header.numEntries_<0
            || header.offsetString_<0
            || header.offsetData_<0
            || header.offsetData_<header.offsetString_)
        {
            return false;
        }

        LARGE_INTEGER offset;
        offset.QuadPart = 0;
        if(!SetFilePointerEx(handle.handle_, offset, &offset, FILE_CURRENT)){
            return false;
        }

        if(checkHash){
            //ハッシュチェック
            LARGE_INTEGER begin;
            begin.QuadPart = 0;
            if(!SetFilePointerEx(handle.handle_, begin, NULL, FILE_BEGIN)){
                return false;
            }

            LARGE_INTEGER fileSize;
            if(!GetFileSizeEx(handle.handle_, &fileSize)){
                return false;
            }
            //ハッシュ値は計算に含まない
            s64 totalSize = fileSize.QuadPart<sizeof(VFSPackFooter)? 0 : fileSize.QuadPart-sizeof(VFSPackFooter);

            xxHashContext32 xxHashContext;
            xxHash32Init(xxHashContext, header.reserved_);
            const s64 bufferSize = 4*1024;
            u8* buffer = LNEW u8[bufferSize];
            s64 count = totalSize/bufferSize;
            bool result = true;
            for(s64 i=0; i<count; ++i){
                if(!readFile(handle.handle_, buffer, bufferSize)){
                    result = false;
                    break;
                }
                xxHash32Update(xxHashContext, buffer, bufferSize);
            }
            u32 rest = static_cast<u32>(totalSize-count*bufferSize);
            if(!readFile(handle.handle_, buffer, rest)){
                result = false;
            }
            xxHash32Update(xxHashContext, buffer, rest);
            LDELETE_ARRAY(buffer);

            VFSPackFooter footer;
            if(!readFile(handle.handle_, &footer, sizeof(VFSPackFooter))){
                result = false;
            }
            if(!result){
                return false;
            }
            u32 hash = xxHash32Finalize(xxHashContext);
            if(footer.filehash_ != hash){
                return false;
            }
        }
        
        //文字列テーブルロード
        LARGE_INTEGER stringTop;
        stringTop.QuadPart = header.offsetString_;
        if(!SetFilePointerEx(handle.handle_, stringTop, NULL, FILE_BEGIN)){
            return false;
        }

        s32 stringBufferLength = static_cast<s32>(header.offsetData_ - header.offsetString_);
        if(stringBufferLength<0){
            return false;
        }
        Char* stringBuffer = LNEW Char[stringBufferLength+1];
        stringBuffer[stringBufferLength] = CharNull;
        if(!readFile(handle.handle_, stringBuffer, sizeof(Char)*stringBufferLength)){
            return false;
        }

        //エントリロード
        if(!SetFilePointerEx(handle.handle_, offset, NULL, FILE_BEGIN)){
            return false;
        }
        u32 entrySize = fs::getEntrySize();
        u8* entries = LNEW u8[entrySize*header.numEntries_];
        lcore::memset(entries, 0, entrySize*header.numEntries_);

        bool result = true;
        VFSData vfsData;
        u8* e = entries;
        s32 count = 0;
        s32 i=0;
        for(; i<header.numEntries_; ++i){
            if(!readFile(handle.handle_, &vfsData, sizeof(VFSData))){
                result = false;
                break;
            }
            switch(vfsData.file_.type_)
            {
            case fs::Type_File:
            {
                fs::File* file = LPLACEMENT_NEW(e) fs::File();
                setName(*file, vfsData, stringBufferLength, stringBuffer);
                file->set(
                    vfsData.file_.dataOffset_,
                    vfsData.file_.dataUncompressedSize_,
                    vfsData.file_.dataCompressedSize_,
                    handle.handle_,
                    false,
                    true);
                ++count;
            }
                break;
            case fs::Type_Directory:
            {
                fs::Directory* directory = LPLACEMENT_NEW(e) fs::Directory();
                setName(*directory, vfsData, stringBufferLength, stringBuffer);
                uintptr_t descs = reinterpret_cast<uintptr_t>(entries + entrySize*vfsData.directory_.childOffset_);
                directory->set(
                    vfsData.directory_.childSize_,
                    descs,
                    true);
                ++count;
            }
                break;
            default:
                i = header.numEntries_-1;
                result = false;
                break;
            }
            e += entrySize;
        }

        if(result){
            pack.handle_ = handle.release();
            pack.numEntries_ = header.numEntries_;
            pack.entries_ = entries;
            pack.stringBuffer_ = stringBuffer;

        } else{
            release(count, entries);
            LDELETE_ARRAY(entries);
            LDELETE_ARRAY(stringBuffer);
        }
        return result;
    }
}
