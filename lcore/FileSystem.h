#ifndef INC_LCORE_FILESYSTEM_H_
#define INC_LCORE_FILESYSTEM_H_
/**
@file FileSystem.h
@author t-sakai
@date 2016/10/28 create
*/
#include "lcore.h"
#include "LString.h"

namespace lcore
{
    class FileProxy;
    class DirectoryProxy;

    class VirtualFileSystemBase;
    class VirtualFileSystemPack;
    class VirtualFileSystemOS;

namespace fs
{
    enum Type
    {
        Type_None = 0x00U,
        Type_File = 0x01U,
        Type_Directory = 0x02U,
    };

    enum VFSType
    {
        VFSType_Pack = 0,
        VFSType_OS = 1,
        VFSType_OSRaw = 2,
    };

    struct FileStatus
    {
        u16 type_;
        u16 flags_;
        s64 umcompressedSize_;
        s64 compressedSize_;
        s32 nameLength_;
        const Char* name_;
    };

    class File;
    class Directory;

    //--------------------------------------------
    //---
    //--- Descriptor
    //---
    //--------------------------------------------
    class Descriptor
    {
    public:
        static const s32 SmallSize = 8;

        static const u16 Flag_ShareData = 0x01U<<0;
        static const u16 Flag_ShareName = 0x01U<<1;
        static const u16 Flag_Compressed = 0x01U<<2;
        static const u16 Flag_ShareDescs = 0x01U<<3;

        inline u16 getType() const;
        inline bool checkFlag(u16 flag) const;
        inline void setFlag(u16 flag);
        inline void resetFlag(u16 flag);

        inline s32 getNameLength() const;
        inline const Char* getName() const;
        void setName(s32 length, Char* filename, bool share);
        s32 compareName(s32 length, const Char* name) const;
        s32 compareName(const lcore::String& name) const;

        void swap(Descriptor& rhs);
    protected:
        friend class File;
        friend class Directory;
        friend class FileProxy;
        friend class DirectoryProxy;

        Descriptor();
        Descriptor(u16 type, u16 flags);
        ~Descriptor();

        void destroyName();

        union Buffer
        {
            Char small_[SmallSize];
            Char* elements_;
        };

        u16 type_;
        u16 flags_;
        s32 nameLength_;
        Buffer name_;

    private:
        Descriptor(const Descriptor&) = delete;
        Descriptor(Descriptor&&) = delete;
        Descriptor& operator=(const Descriptor&) = delete;
        Descriptor& operator=(Descriptor&&) = delete;
    };

    inline u16 Descriptor::getType() const
    {
        return type_;
    }

    inline bool Descriptor::checkFlag(u16 flag) const
    {
        return 0 != (flags_ & flag);
    }

    inline void Descriptor::setFlag(u16 flag)
    {
        flags_ |= flag;
    }

    inline void Descriptor::resetFlag(u16 flag)
    {
        flags_ &= ~flag;
    }

    inline s32 Descriptor::getNameLength() const
    {
        return nameLength_;
    }

    inline const Char* Descriptor::getName() const
    {
        return (checkFlag(Flag_ShareName) || (SmallSize<=nameLength_))? name_.elements_ : name_.small_;
    }

    //--------------------------------------------
    //---
    //--- File
    //---
    //--------------------------------------------
    class File : public Descriptor
    {
    public:
        File();
        ~File();

        inline bool isCompressed() const;
        inline s64 getUncompressedSize() const;
        inline s64 getCompressedSize() const;
        void set(
            s64 offset,
            s64 uncompressedSize,
            s64 compressedSize,
            void* data,
            bool compressed, bool share);

        inline bool read(s64 size, void* data);
        bool read(s64 offset, s64 size, void* data);
        inline bool write(s64 size, void* data);
        bool write(s64 offset, s64 size, void* data);

        void swap(File& rhs);
    private:
        File(const File&) = delete;
        File(File&&) = delete;
        File& operator=(const File&) = delete;
        File& operator=(File&&) = delete;

        friend class FileProxy;

        void destroy();

        s64 offset_;
        s64 uncompressedSize_;
        s64 compressedSize_;
        void* data_;
    };

    inline bool File::isCompressed() const
    {
        return checkFlag(Flag_Compressed);
    }

    inline s64 File::getUncompressedSize() const
    {
        return uncompressedSize_;
    }

    inline s64 File::getCompressedSize() const
    {
        return compressedSize_;
    }
    inline bool File::read(s64 size, void* data)
    {
        return read(0, size, data);
    }

    inline bool File::write(s64 size, void* data)
    {
        return write(0, size, data);
    }

    //--------------------------------------------
    //---
    //--- Directory
    //---
    //--------------------------------------------
    class Directory : public Descriptor
    {
    public:
        Directory();
        ~Directory();

        s32 findChild(s32 length, const Char* name) const;
        s32 findChild(const lcore::String& name) const;

        inline s32 getNumChildren() const;
        FileStatus getFileStatus(s32 index) const;
        inline File* getFile(s32 index);
        inline Directory* getDirectory(s32 index);

        //void addFile(File& file);
        //void addDirectory(Directory& directory);
        //void remove(s32 index);

        void set(s32 numChildren, uintptr_t descriptors, bool share);

        void swap(Directory& rhs);
    private:
        Directory(const Directory&) = delete;
        Directory(Directory&&) = delete;
        Directory& operator=(const Directory&) = delete;
        Directory& operator=(Directory&&) = delete;

        friend class DirectoryProxy;

        void destroy();
        inline const Descriptor* getDescriptor(s32 index) const;
        inline Descriptor* getDescriptor(s32 index);

        s32 numChildren_;
        uintptr_t descriptors_;
    };

    inline s32 Directory::getNumChildren() const
    {
        return numChildren_;
    }

    inline File* Directory::getFile(s32 index)
    {
        LASSERT(0<=index && index<numChildren_);
        Descriptor* descriptor = getDescriptor(index);
        return (Type_File == descriptor->type_)
            ? static_cast<File*>(descriptor)
            : NULL;
    }

    inline Directory* Directory::getDirectory(s32 index)
    {
        LASSERT(0<=index && index<numChildren_);
        Descriptor* descriptor = getDescriptor(index);
        return (Type_Directory == descriptor->type_)
            ? static_cast<Directory*>(descriptor)
            : NULL;
    }

    inline const Descriptor* Directory::getDescriptor(s32 index) const
    {
        return (sizeof(File)<sizeof(Directory))
            ? static_cast<const Descriptor*>( &reinterpret_cast<const Directory*>(descriptors_)[index] )
            : static_cast<const Descriptor*>( &reinterpret_cast<const File*>(descriptors_)[index] );
    }

    inline Descriptor* Directory::getDescriptor(s32 index)
    {
        return (sizeof(File)<sizeof(Directory))
            ? static_cast<Descriptor*>( &reinterpret_cast<Directory*>(descriptors_)[index] )
            : static_cast<Descriptor*>( &reinterpret_cast<File*>(descriptors_)[index] );
    }

    inline u32 getEntrySize()
    {
        return sizeof(fs::File)<sizeof(fs::Directory)? sizeof(fs::Directory) : sizeof(fs::File);
    }
}

    //--------------------------------------------
    //---
    //--- FileSystem
    //---
    //--------------------------------------------
    /**
    ID‚ÌŒã‚ë‚©‚çŒŸõ‚·‚é.
    */
    class FileSystem
    {
    public:
        static const s32 MaxMounts = 32;
        FileSystem();
        ~FileSystem();

        /**
        */
        bool mountOS(s32 id, const Char* path);
        /**
        */
        bool mountPack(s32 id, const Char* path, bool checkHash);
        /**
        */
        void unmount(s32 id);

        /**
        */
        VirtualFileSystemBase* get(s32 id);

        /**
        */
        FileProxy* openFile(const Char* path);
        /**
        */
        void closeFile(FileProxy* file);

        /**
        */
        DirectoryProxy* openDirectory(const Char* path);
        /**
        */
        void closeDirectory(DirectoryProxy* directory);

        /**
        */
        bool packDirectory(const Char* filepath, const Char* path);
    private:
        FileSystem(const FileSystem&) = delete;
        FileSystem(FileSystem&&) = delete;
        FileSystem& operator=(const FileSystem&) = delete;
        FileSystem& operator=(FileSystem&&) = delete;

        s32 maxID_;
        VirtualFileSystemBase* vfs_[MaxMounts];
    };
}
#endif //INC_LCORE_FILESYSTEM_H_
