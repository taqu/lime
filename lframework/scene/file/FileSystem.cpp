/**
@file FileSystem.cpp
@author t-sakai
@date 2014/11/25 create
*/
#include "FileSystem.h"
#include <lcore/CLibrary.h>
#include <lcore/Sort.h>
#include <lcore/liostream.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

namespace lscene
{
namespace lfile
{
    //------------------------------------------------
    //---
    //--- PackEntry
    //---
    //------------------------------------------------
    const PackEntry* PackEntry::findChild(u32 length, const Char* name) const
    {
        LASSERT(NULL != name);
        LASSERT(!isFile());

        const PackEntry* children = getChildren();
        for(s32 i=0; i<size_; ++i){
            if(length == children[i].nameLength_ && 0 == lcore::strncmp(name, children[i].getName(), length)){
                return &children[i];
            }
        }
        return NULL;
    }

    //------------------------------------------------
    //---
    //--- FileSystemBase
    //---
    //------------------------------------------------
    SharedFile FileSystemBase::openForceOSFileSystem(const Char* path)
    {
        LASSERT(NULL != path);

        lcore::ifstream file(path, lcore::ios::binary);
        if(!file.is_open()){
            return SharedFile();
        }

        FileDesc* fileDesc = (FileDesc*)LSCENE_MALLOC(sizeof(FileDesc));
        if(NULL == fileDesc){
            return SharedFile();
        }
        fileDesc = LIME_PLACEMENT_NEW(fileDesc) FileDesc();
        fileDesc->file_.swap(file);

        s32 fileSize = static_cast<s32>(fileDesc->file_.getSize());
        return SharedFile(fileDesc->file_.getID(), fileDesc, 0, fileSize);
    }

    //------------------------------------------------
    //---
    //--- VirtualFileSystem
    //---
    //------------------------------------------------
    VirtualFileSystem::VirtualFileSystem()
        :entries_(NULL)
        ,metaInfo_(NULL)
    {
        file_.refCount_ = 1;
    }

    VirtualFileSystem::~VirtualFileSystem()
    {
        LSCENE_FREE(metaInfo_);
    }

    bool VirtualFileSystem::isMounted() const
    {
        return NULL != entries_;
    }

    bool VirtualFileSystem::mount(const Char* path)
    {
        LASSERT(NULL != path);

        lcore::ifstream file(path, lcore::ios::binary);
        if(!file.is_open()){
            return false;
        }

        PackHeader header;
        if(file.read(&header, sizeof(PackHeader)) <= 0){
            return false;
        }
        s32 fileSize = file.getSize(sizeof(PackHeader));
        s32 desiredSize = sizeof(PackHeader)
            + header.numEntries_*sizeof(PackEntry)
            + header.stringBufferSize_
            + header.dataSize_;

        if(fileSize != desiredSize){
            return false;
        }

        s32 metaSize = fileSize - header.dataSize_ - sizeof(PackHeader);
        Char* metaInfo = (Char*)LSCENE_MALLOC( metaSize );
        if(file.read(metaInfo, metaSize) <= 0){
            LSCENE_FREE(metaInfo);
            return false;
        }

        LSCENE_FREE(metaInfo_);

        file_.file_.swap(file);
        header_ = header;
        metaInfo_ = metaInfo;
        entries_ = reinterpret_cast<PackEntry*>(metaInfo_);
        Char* stringBuffer = metaInfo_ + sizeof(PackEntry) * header_.numEntries_;
        for(s32 i=0; i<header_.numEntries_; ++i){
            if(entries_[i].flags_ & PackFlag_Directory){
                entries_[i].data_.pointer_ = entries_ + entries_[i].data_.offset_; //offsetはentries_のインデックス
            }
            entries_[i].name_.pointer_ = stringBuffer + entries_[i].name_.offset_; //offsetはstringBufferのバイトオフセット
        }
        return true;
    }

    SharedFile VirtualFileSystem::open(const Char* path)
    {
        LASSERT(NULL != path);
        if(NULL == entries_){
            return SharedFile();
        }

        if(lcore::Delimiter == path[0]){
            ++path;
        }
        u32 length;
        path = lcore::parseNextNameFromPath(path, length, name_, MaxFileNameBufferSize);
        const PackEntry* current = &entries_[0];
        while(0<length){
            current = current->findChild(length, name_);
            if(NULL == current){
                return SharedFile();
            }
            path = lcore::parseNextNameFromPath(path, length, name_, MaxFileNameBufferSize);
            if(0<length && current->isFile()){
                return SharedFile();
            }
        }
        if(!current->isFile()){
            return SharedFile();
        }

        return SharedFile(current->id_, &file_, current->data_.offset_, current->size_);
    }

    //------------------------------------------------
    //---
    //--- FileSystem
    //---
    //------------------------------------------------
    FileSystem::FileSystem()
        :size_(0)
        ,path_(NULL)
    {
        root_[0] = '\0';
    }

    FileSystem::~FileSystem()
    {
        LSCENE_FREE(path_);
    }

    bool FileSystem::isMounted() const
    {
        return root_[0] != '\0';
    }

    bool FileSystem::mount(const Char* path)
    {
        LASSERT(NULL != path);

        lcore::lsize_t length = lcore::strlen(path);

        if(length<=0){
            root_[0] = '\0';
            return true;
        }

        if(lcore::Delimiter == path[length-1]){
            --length;
        }

        if(MaxFileNameBufferSize<=length){
            return false;
        }

        root_[0] = '\0';
        lcore::strncpy(root_, MaxFileNameBufferSize, path, length);
        return true;
    }

    SharedFile FileSystem::open(const Char* path)
    {
        LASSERT(NULL != path);

        lcore::lsize_t size = lcore::strlen(path) + 2 + MaxFileNameBufferSize;
        if(size_<size){
            LSCENE_FREE(path_);
            path_ = (Char*)LSCENE_MALLOC(size);
            size_ = size;
        }

        path_[0] = '\0';
        lcore::strcat(path_, size, root_);
        if(lcore::Delimiter != path[0]){
            lcore::strcat(path_, size, "/");
        }
        lcore::strcat(path_, size, path);

        lcore::ifstream file(path_, lcore::ios::binary);
        if(!file.is_open()){
            return SharedFile();
        }

        FileDesc* fileDesc = (FileDesc*)LSCENE_MALLOC(sizeof(FileDesc));
        if(NULL == fileDesc){
            return SharedFile();
        }
        fileDesc = LIME_PLACEMENT_NEW(fileDesc) FileDesc();
        fileDesc->file_.swap(file);

        s32 fileSize = static_cast<s32>(fileDesc->file_.getSize());
        return SharedFile(fileDesc->file_.getID(), fileDesc, 0, fileSize);
    }

namespace
{
    bool isFileOrDirectory(const WIN32_FIND_DATA& data)
    {
        if(0 == _tcscmp(data.cFileName, TEXT("."))
            || 0 == _tcscmp(data.cFileName, TEXT("..")))
        {
            return false;
        }

        //長すぎるファイル名は無視
        lsize_t size = lcore::strnlen(data.cFileName, MAX_PATH);
        if((MaxFileNameBufferSize-1)<size){
            return false;
        }

        static const DWORD checks[] =
        {
            FILE_ATTRIBUTE_HIDDEN,
            FILE_ATTRIBUTE_SYSTEM,
            FILE_ATTRIBUTE_ARCHIVE,
            FILE_ATTRIBUTE_DEVICE,
            FILE_ATTRIBUTE_TEMPORARY,
            FILE_ATTRIBUTE_SPARSE_FILE,
            FILE_ATTRIBUTE_REPARSE_POINT,
            FILE_ATTRIBUTE_ENCRYPTED,
            FILE_ATTRIBUTE_INTEGRITY_STREAM,
            FILE_ATTRIBUTE_VIRTUAL,
            FILE_ATTRIBUTE_NO_SCRUB_DATA,
        };
        static const s32 Num = sizeof(checks)/sizeof(DWORD);
        for(int i=0; i<Num; ++i){
            DWORD check = checks[i] & data.dwFileAttributes;
            if(check != 0){
                return false;
            }
        }
        return true;
    }
}

    //------------------------------------------------
    //---
    //--- OFileSystem
    //---
    //------------------------------------------------
    OFileSystem::OFileSystem()
    {
        root_.getName().assign("");
    }

    OFileSystem::~OFileSystem()
    {
    }

    //------------------------------------------------
    void OFileSystem::add(const Char* path)
    {
        LASSERT(NULL != path);
        path_.assign(path);
        if(path_.size()<=0){
            return;
        }

        Char c = path_[path_.size()-1];

        if('\\' == c || '/' == c){
            path_.pop_back();
        }
        add(root_);
    }

    //------------------------------------------------
    void OFileSystem::add(Directory& parent)
    {
        WIN32_FIND_DATA findData;

        path_.push_back("/*");
        HANDLE handle = FindFirstFile(path_.c_str(), &findData);
        if(INVALID_HANDLE_VALUE == handle){
            return;
        }
        path_.pop_back();
        for(;;){
            if(isFileOrDirectory(findData)){

                if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                    Directory* directory = LIME_NEW Directory();
                    directory->getName().assign(findData.cFileName);
                    parent.addChild(directory);

                    path_.push_back(directory->getName().c_str());
                    add(*directory);
                    path_.pop_back(directory->getName().size());

                }else{
                    File* file = LIME_NEW File();
                    file->getName().assign(findData.cFileName);

                    path_.push_back(file->getName().c_str());
                    lcore::ifstream infile(path_.c_str(), lcore::ios::binary);
                    if(infile.is_open()){
                        file->setID(infile.getID());

                        s32 size = infile.getSize();
                        Char* data = LIME_NEW Char[size];
                        infile.read(data, size);
                        infile.close();
                        file->setData(size, data);
                    }
                    path_.pop_back(file->getName().size());

                    parent.addChild(file);
                }
            }
            if(FALSE == FindNextFile(handle, &findData)){
                break;
            }
        }
        FindClose(handle);
        path_.pop_back();
    }


    //------------------------------------------------
    void OFileSystem::save(const Char* path)
    {
        LASSERT(NULL != path);
        lcore::ofstream file(path, lcore::ios::binary);
        if(!file.is_open()){
            return;
        }

        //エントリ数カウント
        CountEntry countEntry;
        visit(root_, countEntry);
        s32 numEntries = countEntry.count_ + 1; //ルート分+1
        if(numEntries<=0){
            return;
        }

        PackEntry* entries = LIME_NEW PackEntry[numEntries];
        lcore::osstream bufferStrings(1024);
        lcore::osstream bufferData(1024);

        WriteEntry writeEntry(entries, bufferStrings, bufferData);

        writeEntry(root_);
        save(root_, writeEntry.entries_, writeEntry);

        PackHeader header;
        header.numEntries_ = numEntries;
        header.stringBufferSize_ = bufferStrings.tellg();
        header.dataSize_ = bufferData.tellg();
        header.dataOffset_ = sizeof(PackHeader) + sizeof(PackEntry)*numEntries + header.stringBufferSize_;

        //オフセット補正
        for(s32 i=0; i<numEntries; ++i){
            if(0 == (entries[i].flags_ & PackFlag_Directory)){
                entries[i].data_.offset_ += header.dataOffset_;
            }
        }

        file.write(&header, sizeof(PackHeader));
        file.write(entries, sizeof(PackEntry)*numEntries);
        file.write(bufferStrings.c_str(), bufferStrings.tellg());
        file.write(bufferData.c_str(), bufferData.tellg());
        file.close();
    }

    //------------------------------------------------
    void OFileSystem::save(
        Directory& parent,
        PackEntry* parentEntry,
        WriteEntry& writeEntry)
    {
        s32 startIndex = writeEntry.count_;
        if(NULL != parentEntry){
            parentEntry->data_.offset_ = startIndex;
        }

        for(s32 i=0; i<parent.getNumChildren(); ++i){
            writeEntry(*parent.getChild(i));
        }

        for(s32 i=0; i<parent.getNumChildren(); ++i){
            File* file = parent.getChild(i);
            if(!file->isFile()){
                save(
                    reinterpret_cast<Directory&>(*file),
                    writeEntry.entries_ + startIndex + i,
                    writeEntry);
            }
        }
    }

    //------------------------------------------------
    //---
    //--- OFileSystem::String
    //---
    //------------------------------------------------
    OFileSystem::String::String()
        :capacity_(0)
        ,size_(0)
        ,buffer_(NULL)
    {
    }

    OFileSystem::String::String(const String& rhs)
        :capacity_(0)
        ,size_(0)
        ,buffer_(NULL)
    {
        assign(rhs.buffer_, rhs.size_);
    }

    OFileSystem::String::String(const Char* str)
        :capacity_(0)
        ,size_(0)
        ,buffer_(NULL)
    {
        assign(str);
    }

    OFileSystem::String::~String()
    {
        LIME_DELETE_ARRAY(buffer_);
    }

    void OFileSystem::String::assign(const Char* str)
    {
        LASSERT(NULL != str);
        assign(str, lcore::strlen(str));
    }

    void OFileSystem::String::assign(const Char* str, u32 size)
    {
        resize(size+1);
        size_ = size;
        lcore::strncpy(buffer_, size_, str, size_);
        buffer_[size_] = '\0';
    }

    void OFileSystem::String::resize(u32 capacity)
    {
        if(capacity_<capacity){
            capacity_ = (capacity +  AlignMask) & ~AlignMask;
            LIME_DELETE_ARRAY(buffer_);
            buffer_ = LIME_NEW Char[capacity_];
        }
    }

    u32 OFileSystem::String::size() const
    {
        return size_;
    }

    const Char* OFileSystem::String::c_str() const
    {
        return buffer_;
    }

    void OFileSystem::String::push_back(Char c)
    {
        resize(size_+2);
        buffer_[size_] = c;
        ++size_;
        buffer_[size_] = '\0';
    }

    void OFileSystem::String::push_back(const Char* str)
    {
        LASSERT(NULL != str);
        s32 len = lcore::strlen(str);
        resize(size_+len+1);
        for(s32 i=0; i<len; ++i){
            buffer_[size_] = str[i];
            ++size_;
        }
        buffer_[size_] = '\0';
    }

    void OFileSystem::String::pop_back()
    {
        if(0<size_){
            --size_;
            buffer_[size_] = '\0';
        }
    }

    void OFileSystem::String::pop_back(u32 size)
    {
        size = lcore::maximum(size_-size, 0U);
        for(u32 i=size; i<size_; ++i){
            buffer_[i] = '\0';
        }
        size_ = size;
    }

    Char OFileSystem::String::operator[](s32 index) const
    {
        return buffer_[index];
    }

    OFileSystem::String& OFileSystem::String::operator=(const String& rhs)
    {
        assign(rhs.buffer_, rhs.size_);
        return *this;
    }

    //------------------------------------------------
    //---
    //--- OFileSystem::File
    //---
    //------------------------------------------------
    OFileSystem::File::File()
        :id_(0)
        ,size_(0)
        ,data_(NULL)
    {
    }

    OFileSystem::File::~File()
    {
        LIME_DELETE_ARRAY(data_);
    }

    u64 OFileSystem::File::getID() const
    {
        return id_;
    }

    void OFileSystem::File::setID(u64 id)
    {
        id_ = id;
    }

    OFileSystem::String& OFileSystem::File::getName()
    {
        return name_;
    }

    const OFileSystem::String& OFileSystem::File::getName() const
    {
        return name_;
    }

    s32 OFileSystem::File::getSize() const
    {
        return size_;
    }

    const Char* OFileSystem::File::getData() const
    {
        return data_;
    }

    void OFileSystem::File::setData(s32 size, Char* data)
    {
        LIME_DELETE_ARRAY(data_);
        size_ = size;
        data_ = data;
    }

    //------------------------------------------------
    //---
    //--- OFileSystem::Directory
    //---
    //------------------------------------------------
    OFileSystem::Directory::Directory()
    {
    }

    OFileSystem::Directory::~Directory()
    {
        clearChildren();
    }

    void OFileSystem::Directory::addChild(File* child)
    {
        LASSERT(NULL != child);
    }

    void OFileSystem::Directory::clearChildren()
    {
        for(s32 i=0; i<children_.size(); ++i){
            LIME_DELETE(children_[i]);
        }
        children_.clear();
    }

    void OFileSystem::Directory::sortChildren()
    {
        lcore::introsort(children_.size(), children_.begin(), SortFileByName());
    }

    s32 OFileSystem::Directory::getNumChildren() const
    {
        return children_.size();
    }

    const OFileSystem::File* OFileSystem::Directory::getChild(s32 index) const
    {
        return children_[index];
    }

    OFileSystem::File* OFileSystem::Directory::getChild(s32 index)
    {
        return children_[index];
    }

    //------------------------------------------------
    //---
    //--- OFileSystem::SortFileByName
    //---
    //------------------------------------------------
    bool OFileSystem::SortFileByName::operator()(File* lhs, File* rhs) const
    {
        s32 maxSize = lcore::maximum(lhs->getName().size(), rhs->getName().size());
        return lcore::strncmp(lhs->getName().c_str(), rhs->getName().c_str(), maxSize) < 0;
    }

    //------------------------------------------------
    //---
    //--- OFileSystem::CountEntry
    //---
    //------------------------------------------------
    OFileSystem::CountEntry::CountEntry()
        :count_(0)
    {
    }

    void OFileSystem::CountEntry::operator()(OFileSystem::File& /*file*/)
    {
        ++count_;
    }

    //------------------------------------------------
    //---
    //--- OFileSystem::WriteEntry
    //---
    //------------------------------------------------
    OFileSystem::WriteEntry::WriteEntry(
        PackEntry* entries,
        lcore::osstream& bufferStrings,
        lcore::osstream& bufferData)
        :count_(0)
        ,entries_(entries)
        ,bufferStrings_(bufferStrings)
        ,bufferData_(bufferData)
    {
    }

    void OFileSystem::WriteEntry::operator()(OFileSystem::File& file)
    {
        ++count_;
        PackEntry entry;
        if(file.isFile()){
            entry.flags_ = PackFlag_None;
            entry.size_ = file.getSize();
            entry.data_.offset_ = bufferData_.tellg();
            bufferData_.write(file.getData(), file.getSize());

        }else{
            entry.flags_ = PackFlag_Directory;
            entry.size_ = reinterpret_cast<Directory*>(&file)->getNumChildren();
            entry.data_.offset_ = 0; //正しいオフセットは後でセット
        }

        String& name = file.getName();
        entry.nameLength_ = name.size();
        entry.name_.offset_ = bufferStrings_.tellg();
        s32 size = name.size() + 1;
        bufferStrings_.write(name.c_str(), size);
        
    }
}
}
