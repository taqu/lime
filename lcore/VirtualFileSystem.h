#ifndef INC_LCORE_VIRTUALFILESYSTEM_H_
#define INC_LCORE_VIRTUALFILESYSTEM_H_
/**
@file VirtualFileSystem.h
@author t-sakai
@date 2016/11/06 create
*/
#include "lcore.h"
#include "FileSystem.h"
#include "File.h"

namespace lcore
{
    struct VFSPack;

    //--------------------------------------------
    //---
    //--- FileProxy
    //---
    //--------------------------------------------
    class FileProxy
    {
    public:
        virtual ~FileProxy()
        {}
        virtual bool isCompressed() const =0;
        virtual s64 getUncompressedSize() const =0;
        virtual s64 getCompressedSize() const =0;

        inline bool read(s64 size, void* data)
        {
            return read(0, size, data);
        }

        virtual bool read(s64 offset, s64 size, void* data) =0;

        inline bool write(s64 size, void* data)
        {
            return write(0, size, data);
        }

        virtual bool write(s64 offset, s64 size, void* data) =0;

        virtual bool thisParent(VirtualFileSystemBase* vfs) const =0;
    protected:
        FileProxy()
        {}
    };

    //----------------------------------------------------------
    //---
    //--- FileProxyIStream
    //---
    //----------------------------------------------------------
    class FileProxyIStream
    {
    public:
        FileProxyIStream();
        explicit FileProxyIStream(FileProxy* file);
        virtual ~FileProxyIStream();

        inline void reset(FileProxy* file);

        virtual bool eof();
        virtual bool good();

        virtual bool seekg(off_t offset, int dir);
        virtual off_t tellg();

        virtual bool read(s64 size, void* data);

        virtual s64 getSize();

        template<class T>
        inline bool read(T& dst);
    protected:
        FileProxyIStream(const FileProxyIStream&) = delete;
        FileProxyIStream& operator=(const FileProxyIStream&) = delete;

        FileProxy* file_;
        s64 current_;
    };

    inline void FileProxyIStream::reset(FileProxy* file)
    {
        file_ = file;
        current_ = 0;
    }

    template<class T>
    inline bool FileProxyIStream::read(T& dst)
    {
        return read(sizeof(T), &dst);
    }

    //--------------------------------------------
    //---
    //--- DirectoryProxy
    //---
    //--------------------------------------------
    class DirectoryProxy
    {
    public:
        virtual s32 getNumChildren() const =0;

        virtual s32 findChild(s32 length, const Char* name) const =0;
        s32 findChild(const Char* name) const
        {
            return findChild(lcore::strlen_s32(name), name);
        }
        s32 findChild(const lcore::String& name) const
        {
            return findChild(name.length(), name.c_str());
        }

        virtual fs::FileStatus getStatus(s32 index) const =0;
        virtual fs::FileStatus getStatus(s32 length, const Char* name) const =0;
        fs::FileStatus getStatus(const Char* name) const
        {
            return getStatus(lcore::strlen_s32(name), name);
        }
        fs::FileStatus getStatus(const lcore::String& name) const
        {
            return getStatus(name.length(), name.c_str());
        }

        virtual FileProxy* openFile(s32 index) =0;
        virtual FileProxy* openFile(s32 length, const Char* name) =0;
        FileProxy* openFile(const Char* name)
        {
            return openFile(lcore::strlen_s32(name), name);
        }
        FileProxy* openFile(const lcore::String& name)
        {
            return openFile(name.length(), name.c_str());
        }

        virtual DirectoryProxy* openDirectory(s32 index) =0;
        virtual DirectoryProxy* openDirectory(s32 length, const Char* name) =0;
        DirectoryProxy* openDirectory(const Char* name)
        {
            return openDirectory(lcore::strlen_s32(name), name);
        }

        DirectoryProxy* openDirectory(const lcore::String& name)
        {
            return openDirectory(name.length(), name.c_str());
        }
        virtual bool thisParent(VirtualFileSystemBase* vfs) const =0;
    protected:
        DirectoryProxy()
        {}
        virtual ~DirectoryProxy()
        {}
    };

    //--------------------------------------------
    //---
    //--- VirtualFileSystemBase
    //---
    //--------------------------------------------
    class VirtualFileSystemBase
    {
    public:
        virtual ~VirtualFileSystemBase()
        {}

        virtual s32 getType() =0;
        virtual DirectoryProxy* openRoot() =0;
        virtual FileProxy* openFile(const Char* path) =0;
        virtual void closeFile(FileProxy* file) =0;
        virtual DirectoryProxy* openDirectory(const Char* path) =0;
        virtual void closeDirectory(DirectoryProxy* directory) =0;

    protected:
        VirtualFileSystemBase()
        {}

    private:
        VirtualFileSystemBase(const VirtualFileSystemBase&);
        VirtualFileSystemBase& operator=(const VirtualFileSystemBase&);
    };

    //--------------------------------------------
    //---
    //--- FileProxyPack
    //---
    //--------------------------------------------
    class FileProxyPack : public FileProxy
    {
    public:
        virtual bool isCompressed() const;
        virtual s64 getUncompressedSize() const;
        virtual s64 getCompressedSize() const;
        virtual bool read(s64 offset, s64 size, void* data);
        virtual bool write(s64 offset, s64 size, void* data);

        virtual bool thisParent(VirtualFileSystemBase* vfs) const;
    protected:
        friend class VirtualFileSystemPack;

        FileProxyPack(fs::File* file, VirtualFileSystemPack* parent);
        virtual ~FileProxyPack();

        fs::File* file_;
        VirtualFileSystemPack* parent_;
    };

    //--------------------------------------------
    //---
    //--- DirectoryProxyPack
    //---
    //--------------------------------------------
    class DirectoryProxyPack : public DirectoryProxy
    {
    public:
        virtual s32 getNumChildren() const;

        virtual s32 findChild(s32 length, const Char* name) const;

        virtual fs::FileStatus getStatus(s32 index) const;
        virtual fs::FileStatus getStatus(s32 length, const Char* name) const;

        virtual FileProxy* openFile(s32 index);
        virtual FileProxy* openFile(s32 length, const Char* name);

        virtual DirectoryProxy* openDirectory(s32 index);
        virtual DirectoryProxy* openDirectory(s32 length, const Char* name);

        virtual bool thisParent(VirtualFileSystemBase* vfs) const;
    protected:
        friend class VirtualFileSystemPack;

        DirectoryProxyPack(fs::Directory* directory, VirtualFileSystemPack* parent);
        virtual ~DirectoryProxyPack();

        fs::Directory* directory_;
        VirtualFileSystemPack* parent_;
    };

    //--------------------------------------------
    //---
    //--- VirtualFileSystemPack
    //---
    //--------------------------------------------
    class VirtualFileSystemPack : public VirtualFileSystemBase
    {
    public:
        explicit VirtualFileSystemPack(VFSPack& vfsPack);
        virtual ~VirtualFileSystemPack();

        virtual s32 getType();
        virtual DirectoryProxy* openRoot();
        virtual FileProxy* openFile(const Char* path);
        virtual void closeFile(FileProxy* file);
        virtual DirectoryProxy* openDirectory(const Char* path);
        virtual void closeDirectory(DirectoryProxy* directory);

    private:
        friend class FileProxyPack;
        friend class DirectoryProxyPack;

        VirtualFileSystemPack(const VirtualFileSystemPack&);
        VirtualFileSystemPack& operator=(const VirtualFileSystemPack&);

        FileProxyPack* popFile(fs::File* file);
        void pushFile(FileProxyPack* file);
        DirectoryProxyPack* popDirectory(fs::Directory* directory);
        void pushDirectory(DirectoryProxyPack* directory);

        void expand();

        static const s32 Bins = 8;
        static const s32 ExpandSize = 32;
        static const s32 NameBufferSize = 128;

        Char name_[NameBufferSize];
        fs::Directory emptyRoot_;
        fs::Directory* localRoot_;

        union Proxy
        {
            Proxy* next_;
            u8 file_[sizeof(FileProxyPack)];
            u8 directory_[sizeof(DirectoryProxyPack)];
        };
        s32 size_;
        Proxy* proxies_[Bins];
        Proxy* top_;

        LHANDLE handle_;
        s32 numEntries_;
        u8* entries_;
        Char* stringBuffer_;
    };


    //--------------------------------------------
    //---
    //--- FileProxyOS
    //---
    //--------------------------------------------
    class FileProxyOS : public FileProxy
    {
    public:
        virtual bool isCompressed() const;
        virtual s64 getUncompressedSize() const;
        virtual s64 getCompressedSize() const;
        virtual bool read(s64 offset, s64 size, void* data);
        virtual bool write(s64 offset, s64 size, void* data);

        virtual bool thisParent(VirtualFileSystemBase* vfs) const;
    protected:
        friend class VirtualFileSystemOS;

        FileProxyOS(LHANDLE handle, VirtualFileSystemOS* parent);
        virtual ~FileProxyOS();

        LHANDLE handle_;
        VirtualFileSystemOS* parent_;
    };

    //--------------------------------------------
    //---
    //--- DirectoryProxyOS
    //---
    //--------------------------------------------
    class DirectoryProxyOS : public DirectoryProxy
    {
    public:
        virtual s32 getNumChildren() const;

        virtual s32 findChild(s32 length, const Char* name) const;

        virtual fs::FileStatus getStatus(s32 index) const;
        virtual fs::FileStatus getStatus(s32 length, const Char* name) const;

        virtual FileProxy* openFile(s32 index);
        virtual FileProxy* openFile(s32 length, const Char* name);

        virtual DirectoryProxy* openDirectory(s32 index);
        virtual DirectoryProxy* openDirectory(s32 length, const Char* name);

        virtual bool thisParent(VirtualFileSystemBase* vfs) const;
    protected:
        friend class VirtualFileSystemOS;

        DirectoryProxyOS(s32 length, Char* path, VirtualFileSystemOS* parent);
        virtual ~DirectoryProxyOS();

        inline String& getPath() const;
        inline String& getSearchPath() const;
        inline String& getSearchPath(s32 length, const Char* name) const;
        inline const Char* getTemporaryName(s32 length, const Char* name) const;
        inline const Char* getTemporaryPath(const Char* name) const;
        inline String& getTemporaryDirectoryPath(const Char* name) const;

        s32 length_;
        Char* path_;
        VirtualFileSystemOS* parent_;
    };

    //--------------------------------------------
    //---
    //--- VirtualFileSystemOS
    //---
    //--------------------------------------------
    class VirtualFileSystemOS : public VirtualFileSystemBase
    {
    public:
        explicit VirtualFileSystemOS(const Char* path);
        virtual ~VirtualFileSystemOS();

        virtual s32 getType();
        virtual DirectoryProxy* openRoot();
        virtual FileProxy* openFile(const Char* path);
        virtual void closeFile(FileProxy* file);
        virtual DirectoryProxy* openDirectory(const Char* path);
        virtual void closeDirectory(DirectoryProxy* directory);

    private:
        friend class FileProxyOS;
        friend class DirectoryProxyOS;

        VirtualFileSystemOS(const VirtualFileSystemOS&);
        VirtualFileSystemOS& operator=(const VirtualFileSystemOS&);

        FileProxyOS* popFile(LHANDLE handle);
        void pushFile(FileProxyOS* file);
        DirectoryProxyOS* popDirectory(s32 length, Char* path);
        void pushDirectory(DirectoryProxyOS* directory);

        void expand();

        static const s32 Bins = 8;
        static const s32 ExpandSize = 32;

        lcore::String path_;
        lcore::String globalRoot_;

        union Proxy
        {
            Proxy* next_;
            u8 file_[sizeof(FileProxyOS)];
            u8 directory_[sizeof(DirectoryProxyOS)];
        };
        s32 size_;
        Proxy* proxies_[Bins];
        Proxy* top_;
    };

    //--------------------------------------------
    //---
    //--- FileProxyOSRaw
    //---
    //--------------------------------------------
    class FileProxyOSRaw : public FileProxy
    {
    public:
        FileProxyOSRaw(LHANDLE handle);
        virtual ~FileProxyOSRaw();

        virtual bool isCompressed() const;
        virtual s64 getUncompressedSize() const;
        virtual s64 getCompressedSize() const;
        virtual bool read(s64 offset, s64 size, void* data);
        virtual bool write(s64 offset, s64 size, void* data);

        virtual bool thisParent(VirtualFileSystemBase* vfs) const;
    protected:
        LHANDLE handle_;
    };
}
#endif //INC_LCORE_VIRTUALFILESYSTEM_H_
