#ifndef INC_LSCENE_LFILE_FILESYSTEM_H__
#define INC_LSCENE_LFILE_FILESYSTEM_H__
/**
@file FileSystem.h
@author t-sakai
@date 2014/11/25 create
*/
#include "../lscene.h"
#include <lcore/liostream.h>
#include <lcore/Vector.h>
#include "File.h"

namespace lscene
{
namespace lfile
{
    enum PackFlag
    {
        PackFlag_None = 0x00U,
        PackFlag_Directory = (0x01U<<0),
    };

    union PackPointer
    {
        void* pointer_;
        s32 offset_;
    };

    struct PackEntry
    {
        bool isFile() const{ return 0 == (flags_&PackFlag_Directory);}
        const Char* getName() const{ return (const Char*)name_.pointer_;}
        const PackEntry* getChildren() const{ return (const PackEntry*)data_.pointer_;}
        const PackEntry* findChild(u32 length, const Char* name) const;

        u64 id_;
        s32 flags_;
        u32 nameLength_;
        s32 size_; //data size or num children
        PackPointer name_;
        PackPointer data_; //data or children
    };

    struct PackHeader
    {
        s32 numEntries_;
        s32 stringBufferSize_;
        s32 dataSize_;
        s32 dataOffset_;
    };


    //------------------------------------------------
    //---
    //--- FileSystemBase
    //---
    //------------------------------------------------
    class FileSystemBase : public FileSystemAllocator
    {
    public:
        virtual ~FileSystemBase()
        {}

        virtual bool isMounted() const =0;

        virtual bool mount(const Char* path) =0;
        virtual SharedFile open(const Char* path) =0;

        SharedFile openForceOSFileSystem(const Char* path);
    protected:
        FileSystemBase(const FileSystemBase&);
        FileSystemBase& operator=(const FileSystemBase&);

        FileSystemBase()
        {}
    };

    //------------------------------------------------
    //---
    //--- VirtualFileSystem
    //---
    //------------------------------------------------
    class VirtualFileSystem : public FileSystemBase
    {
    public:
        VirtualFileSystem();
        virtual ~VirtualFileSystem();

        virtual bool isMounted() const;

        virtual bool mount(const Char* path);
        virtual SharedFile open(const Char* path);
    private:
        VirtualFileSystem(const VirtualFileSystem&);
        VirtualFileSystem& operator=(const VirtualFileSystem&);

        FileDesc file_;
        PackHeader header_;
        PackEntry* entries_;
        Char* metaInfo_;
        Char name_[MaxFileNameBufferSize];
    };

    //------------------------------------------------
    //---
    //--- FileSystem
    //---
    //------------------------------------------------
    class FileSystem : public FileSystemBase
    {
    public:
        FileSystem();
        virtual ~FileSystem();

        virtual bool isMounted() const;

        virtual bool mount(const Char* path);
        virtual SharedFile open(const Char* path);
    private:
        FileSystem(const FileSystem&);
        FileSystem& operator=(const FileSystem&);

        Char root_[MaxFileNameBufferSize];
        u32 size_;
        Char* path_;
    };

    //------------------------------------------------
    //---
    //--- OFileSystem
    //---
    //------------------------------------------------
    class OFileSystem
    {
    public:
        OFileSystem();
        ~OFileSystem();

        void add(const Char* path);

        void save(const Char* path);
    private:
        OFileSystem(const OFileSystem&);
        OFileSystem& operator=(const OFileSystem&);

        class File;
        typedef lcore::vector_arena<File*> FileVector;

        //------------------------------------------------
        //--- String
        //------------------------------------------------
        class String
        {
        public:
            String();
            String(const String& rhs);
            explicit String(const Char* str);
            ~String();

            void assign(const Char* str);
            u32 size() const;
            const Char* c_str() const;

            void push_back(Char c);
            void push_back(const Char* str);
            void pop_back();
            void pop_back(u32 size);

            Char operator[](s32 index) const;

            String& operator=(const String& rhs);
        private:
            static const u32 AlignMask = 16-1;

            void assign(const Char* str, u32 size);
            void resize(u32 capacity);

            u32 capacity_;
            u32 size_;
            Char* buffer_;
        };

        //------------------------------------------------
        //--- File
        //------------------------------------------------
        class File
        {
        public:
            File();
            virtual ~File();

            virtual bool isFile() const{ return true;}

            u64 getID() const;
            void setID(u64 id);

            String& getName();
            const String& getName() const;

            s32 getSize() const;
            const Char* getData() const;
            void setData(s32 size, Char* data);
        protected:
            u64 id_;
            String name_;
            s32 size_;
            Char* data_;
        };

        //------------------------------------------------
        //--- Directory
        //------------------------------------------------
        class Directory : public File
        {
        public:
            Directory();
            virtual ~Directory();

            virtual bool isFile() const{ return false;}
            void addChild(File* child);
            void clearChildren();

            void sortChildren();

            s32 getNumChildren() const;
            const File* getChild(s32 index) const;
            File* getChild(s32 index);
        protected:
            FileVector children_;
        };

        //------------------------------------------------
        //--- SortFileByName
        //------------------------------------------------
        struct SortFileByName
        {
            bool operator()(File* lhs, File* rhs) const;
        };

        //------------------------------------------------
        //--- CountEntry
        //------------------------------------------------
        struct CountEntry
        {
            CountEntry();

            void operator()(File& file);

            s32 count_;
        };

        //------------------------------------------------
        //--- WriteEntry
        //------------------------------------------------
        struct WriteEntry
        {
            WriteEntry(
                PackEntry* entries,
                lcore::osstream& bufferStrings,
                lcore::osstream& bufferData);

            void operator()(File& file);

            s32 count_;
            PackEntry* entries_;
            lcore::osstream& bufferStrings_;
            lcore::osstream& bufferData_;

        private:
            WriteEntry(const WriteEntry&);
            WriteEntry& operator=(const WriteEntry&);
        };

        void add(Directory& parent);
        void save(
            Directory& parent,
            PackEntry* parentEntry,
            WriteEntry& writeEntry);

        template<class T>
        void visit(Directory& parent, T& visitor);

        String path_;
        Directory root_;
    };

    template<class T>
    void OFileSystem::visit(Directory& parent, T& visitor)
    {
        for(s32 i=0; i<parent.getNumChildren(); ++i){
            File* file = parent.getChild(i);
            visitor(*file);

            if(!file->isFile()){
                visit(reinterpret_cast<Directory&>(*file), visitor);
            }
        }
    }
}
}
#endif //INC_LSCENE_LFILE_FILESYSTEM_H__
