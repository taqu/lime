#ifndef INC_LCORE_LIOSTREAM_
#define INC_LCORE_LIOSTREAM_
/**
@file lfstream
@author t-sakai
@date 2010/05/25 create
*/
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#endif

#include "lcore.h"

namespace lcore
{
    typedef s32 OpenMode;

    class ios
    {
    public:
        enum OpenMode
        {
            OpenMode_ForceByte = 0xFFU,
        };
        static const s32 in = 0x01;
	    static const s32 out = 0x02;
	    static const s32 ate = 0x04;
	    static const s32 app = 0x08;
	    static const s32 trunc = 0x10;
	    static const s32 binary = 0x20;

	    enum SeekDir
	    {
	        SeekDir_ForceByte = 0xFFU,
	    };

#if defined(_WIN32) || defined(_WIN64)
        static const s32 beg = FILE_BEGIN;
	    static const s32 cur = FILE_CURRENT;
	    static const s32 end = FILE_END;
#else
	    static const s32 beg = SEEK_SET;
	    static const s32 cur = SEEK_CUR;
	    static const s32 end = SEEK_END;
#endif

	    enum Mode
        {
            Mode_R = 0,
            Mode_W,
            Mode_A,
            Mode_RB,
            Mode_WB,
            Mode_AB,
            Mode_RP,
            Mode_WP,
            Mode_AP,
            Mode_RBP,
            Mode_WBP,
            Mode_ABP,
            Mode_Num,
        };

        static int ModeInt[Mode_Num];

        static const Char* ModeString[Mode_Num];

        static const Char* getModeString(s32 mode);

#if defined(_WIN32) || defined(_WIN64)
        static u32 getDesiredAccess(s32 mode);
        static u32 getCreationDisposition(s32 mode);
#endif
    };

    //----------------------------------------------------------
    //---
    //--- istream
    //---
    //----------------------------------------------------------
    class istream
    {
    public:
        virtual bool eof() =0;
        virtual bool good() =0;

        virtual bool seekg(s32 offset, s32 dir) =0;
        virtual s32 tellg() =0;

        virtual s32 read(void* dst, s32 count) =0;

        virtual u32 getSize() =0;
    protected:
        istream(){}
        ~istream(){}

        istream(const istream&);
        istream& operator=(const istream&);

        template<class T> friend istream& operator>>(istream& strm, T& t);
    };

    template<class T>
    inline istream& operator>>(istream& strm, T& t)
    {
        strm.read((Char*)&t, sizeof(T));
        return strm;
    }

    //----------------------------------------------------------
    //---
    //--- ostream
    //---
    //----------------------------------------------------------
    class ostream
    {
    public:
        virtual bool eof() =0;
        virtual bool good() =0;

        virtual bool seekg(s32 offset, s32 dir) =0;
        virtual s32 tellg() =0;

        virtual s32 write(const void* src, s32 count) =0;

        virtual u32 getSize() =0;
    protected:
        ostream(){}
        ~ostream(){}
        ostream(const ostream&);
        ostream& operator=(const ostream&);
    };


    //----------------------------------------------------------
    //---
    //--- fstream_base
    //---
    //----------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
    template<class Base>
    class fstream_base : public Base
    {
    public:
        bool is_open() const{ return (file_ != NULL);}
        void close();

        virtual bool eof();
        virtual bool good();

        virtual bool seekg(s32 offset, s32 dir);
        virtual s32 tellg();

        virtual u32 getSize();

        void swap(fstream_base& rhs)
        {
            lcore::swap(file_, rhs.file_);
        }

        inline bool flush();

        u64 getID();
    protected:
        fstream_base();
        fstream_base(const Char* filepath, s32 mode);
        ~fstream_base();

        bool open(const Char* filepath, s32 mode);

        virtual s32 read(void* dst, s32 count);
        virtual s32 write(const void* src, s32 count);

        HANDLE file_;
    };

    template<class Base>
    fstream_base<Base>::fstream_base()
        :file_(NULL)
    {
    }

    template<class Base>
    fstream_base<Base>::fstream_base(const Char* filepath, s32 mode)
        :file_(NULL)
    {
        open(filepath, mode);
    }

    template<class Base>
    fstream_base<Base>::~fstream_base()
    {
        close();
    }

    template<class Base>
    bool fstream_base<Base>::open(const Char* filepath, s32 mode)
    {
        LASSERT(NULL != filepath);
        close();
        mode &= ~ios::binary;

        HANDLE file = CreateFile(
            filepath,
            ios::getDesiredAccess(mode),
            FILE_SHARE_READ,
            NULL,
            ios::getCreationDisposition(mode),
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if(INVALID_HANDLE_VALUE != file && (HANDLE)ERROR_ALREADY_EXISTS != file){
            file_ = file;
            return true;
        }else{
            return false;
        }
    }

    template<class Base>
    void fstream_base<Base>::close()
    {
        if(file_){
            CloseHandle(file_);
            file_ = NULL;
        }
    }

    template<class Base>
    bool fstream_base<Base>::eof()
    {
        LASSERT(NULL != file_);
        u32 pos = SetFilePointer(file_, 0, NULL, FILE_CURRENT);
        u32 size = GetFileSize(file_, NULL);
        return (size<=pos);
    }

    template<class Base>
    bool fstream_base<Base>::good()
    {
        if(NULL == file_){
            return false;
        }
        return true;
    }

    template<class Base>
    inline bool fstream_base<Base>::seekg(s32 offset, s32 dir)
    {
        LASSERT(NULL != file_);
        LASSERT(0<=ios::beg && dir<=ios::end);
        return (INVALID_SET_FILE_POINTER != SetFilePointer(file_, offset, NULL, dir));

    }

    template<class Base>
    inline s32 fstream_base<Base>::tellg()
    {
        LASSERT(NULL != file_);
        return SetFilePointer(file_, 0, NULL, FILE_CURRENT);
    }

    template<class Base>
    u32 fstream_base<Base>::getSize()
    {
        LASSERT(NULL != file_);
        return GetFileSize(file_, NULL);
    }

    template<class Base>
    inline s32 fstream_base<Base>::read(void* dst, s32 count)
    {
        LASSERT(NULL != file_);
        DWORD numBytesRead = 0;
        return ReadFile(file_, dst, count, &numBytesRead, NULL)? static_cast<s32>(numBytesRead) : 0;
    }

    template<class Base>
    inline s32 fstream_base<Base>::write(const void* src, s32 count)
    {
        LASSERT(NULL != file_);
        DWORD numBytesWritten = 0;
        WriteFile(file_, src, count, &numBytesWritten, NULL);
        return static_cast<s32>(numBytesWritten);
    }

    template<class Base>
    inline bool fstream_base<Base>::flush()
    {
        LASSERT(NULL != file_);
        return TRUE == FlushFileBuffers(file_);
    }

    template<class Base>
    u64 fstream_base<Base>::getID()
    {
        LASSERT(NULL != file_);
        BY_HANDLE_FILE_INFORMATION fileInfo;
        if(GetFileInformationByHandle(file_, &fileInfo)){
            u64 ret = fileInfo.nFileIndexHigh;
            return (ret<<32) | fileInfo.nFileIndexLow;
        }else{
            return 0;
        }
    }

#else
    template<class Base>
    class fstream_base : public Base
    {
    public:
        bool is_open() const{ return (file_ != NULL);}
        void close();

        virtual bool eof();
        virtual bool good();

        virtual bool seekg(s32 offset, s32 dir);
        virtual s32 tellg();

        virtual u32 getSize()
        {
            return getFileSize(file_);
        }

        void swap(fstream_base& rhs)
        {
            lcore::swap(file_, rhs.file_);
        }

        inline bool flush();

        u64 getID();
    protected:
        fstream_base();
        fstream_base(const Char* filepath, s32 mode);
        ~fstream_base();

        bool open(const Char* filepath, s32 mode);

        virtual s32 read(void* dst, s32 count);
        virtual s32 write(const void* src, s32 count);

        FILE* file_;
    };

    template<class Base>
    fstream_base<Base>::fstream_base()
        :file_(NULL)
    {
    }

    template<class Base>
    fstream_base<Base>::fstream_base(const Char* filepath, s32 mode)
        :file_(NULL)
    {
        open(filepath, mode);
    }

    template<class Base>
    fstream_base<Base>::~fstream_base()
    {
        close();
    }

    template<class Base>
    bool fstream_base<Base>::open(const Char* filepath, s32 mode)
    {
        LASSERT(filepath != NULL);
        close();
        const Char* modeStr = ios::getModeString(mode);
        LASSERT(modeStr != NULL);

        file_ = fopen(filepath, modeStr);
        return (file_ != NULL);
    }

    template<class Base>
    void fstream_base<Base>::close()
    {
        if(file_){
            fclose(file_);
            file_ = NULL;
        }
    }

    template<class Base>
    bool fstream_base<Base>::eof()
    {
        LASSERT(NULL != file_);
        return (0 == ferror(file_)) && (0 != feof(file_));
    }

    template<class Base>
    bool fstream_base<Base>::good()
    {
        if(NULL == file_){
            return false;
        }
        return (0 == ferror(file_));
    }

    template<class Base>
    inline bool fstream_base<Base>::seekg(s32 offset, s32 dir)
    {
        LASSERT(NULL != file_);
        LASSERT(0<=ios::beg && dir<=ios::end);
        return (0 == fseek(file_, offset, dir));

    }

    template<class Base>
    inline s32 fstream_base<Base>::tellg()
    {
        LASSERT(NULL != file_);
        return ftell(file_);
    }

    template<class Base>
    inline s32 fstream_base<Base>::read(void* dst, s32 count)
    {
        LASSERT(NULL != file_);
        return fread(dst, count, 1, file_);
    }

    template<class Base>
    inline s32 fstream_base<Base>::write(const void* src, s32 count)
    {
        return fwrite(src, count, 1, file_);
    }

    template<class Base>
    inline bool fstream_base<Base>::flush()
    {
        LASSERT(NULL != file_);
        return 0 == fflush(file_);
    }

    template<class Base>
    u64 fstream_base<Base>::getID()
    {
        s32 fd = fileno(file_);
        struct stat fileStat;
        if(fstat(fd, &fileStat)<0){
            return 0;
        }
        return fileStat.st_ino;
    }
#endif

    //----------------------------------------------------------
    //---
    //--- ifstream
    //---
    //----------------------------------------------------------
    class ifstream : public fstream_base<istream>
    {
    public:
        typedef fstream_base<istream> super_type;

        ifstream(){}
        ifstream(const Char* filepath, s32 mode=ios::in|ios::binary)
            :super_type(filepath, mode|ios::in)
        {
        }

        bool open(const Char* filepath, s32 mode=ios::in|ios::binary)
        {
            return super_type::open(filepath, mode|ios::in);
        }

        virtual s32 read(void* dst, s32 count){ return super_type::read(dst, count);}

        s32 get();
        s32 peek();
    private:
        ifstream(const ifstream&);
        ifstream& operator=(const ifstream&);
    };

    //----------------------------------------------------------
    //---
    //--- ofstream
    //---
    //----------------------------------------------------------
    class ofstream : public fstream_base<ostream>
    {
    public:
        typedef fstream_base<ostream> super_type;

        ofstream(){}
        ofstream(const Char* filepath, s32 mode=ios::out|ios::binary)
            :super_type(filepath, mode|ios::out)
        {
        }

        bool open(const Char* filepath, s32 mode=ios::out|ios::binary)
        {
            return super_type::open(filepath, mode|ios::out);
        }

        virtual s32 write(const void* src, s32 count){ return super_type::write(src, count);}

        s32 print(const Char* format, ... );
        s32 print(const Char16* format, ... );
    private:
        ofstream(const ofstream&);
        ofstream& operator=(const ofstream&);
    };



    //----------------------------------------------------------
    //---
    //--- sstream_base
    //---
    //----------------------------------------------------------
    template<class Base>
    class bstream_base : public Base
    {
    public:
        virtual bool eof();
        virtual bool good();

        virtual bool seekg(s32 offset, s32 dir);
        virtual s32 tellg();
        virtual u32 getSize();
    protected:
        static const s32 PageSize = 4*1024;

        bstream_base();
        bstream_base(s32 size, u8* buffer);
        ~bstream_base()
        {}

        virtual s32 read(void* dst, s32 count);
        virtual s32 write(const void* src, s32 count);

        void expand();

        s32 current_;
        s32 size_;
        u8* buffer_;
    };

    template<class Base>
    bstream_base<Base>::bstream_base()
        :current_(0)
        ,size_(0)
        ,buffer_(NULL)
    {
    }

    template<class Base>
    bstream_base<Base>::bstream_base(s32 size, u8* buffer)
        :current_(0)
        ,size_(size)
        ,buffer_(buffer)
    {
        LASSERT(0<=size_);
    }

    template<class Base>
    bool bstream_base<Base>::eof()
    {
        return (size_<=current_);
    }

    template<class Base>
    bool bstream_base<Base>::good()
    {
        return (current_<=size_);
    }

    template<class Base>
    bool bstream_base<Base>::seekg(s32 offset, s32 dir)
    {
        switch(dir)
        {
        case ios::beg:
            current_ = offset;
            break;
        case ios::cur:
            current_ += offset;
            break;
        case ios::end:
            current_ = size_-offset-1;
            break;
        default:
            return false;
        };
        s32 end = (0<size_)? size_-1 : 0;
        current_ = lcore::clamp(current_, 0, end);
        return true;
    }

    template<class Base>
    s32 bstream_base<Base>::tellg()
    {
        return current_;
    }

    template<class Base>
    u32 bstream_base<Base>::getSize()
    {
        return size_;
    }

    template<class Base>
    s32 bstream_base<Base>::read(void* dst, s32 count)
    {
        LASSERT(0<=count);
        s32 size = size_-current_;
        count = lcore::minimum(count, size);
        lcore::memcpy(dst, buffer_+current_, count);
        current_ += count;
        return count;
    }

    template<class Base>
    s32 bstream_base<Base>::write(const void* src, s32 count)
    {
        s32 end = current_ + count;
        if(size_<=end){
            expand();
        }

        const Char* s = static_cast<const Char*>(src);
        while(current_<end){
            buffer_[current_] = *s;
            ++s;
            ++current_;
        }
        return count;
    }

    template<class Base>
    void bstream_base<Base>::expand()
    {
        s32 newSize;
        if(size_<8){
            newSize = 16;
        }else if(size_<PageSize){
            newSize = (size_<<1);
        }else{
            newSize = (size_ + (PageSize-1)) & ~(PageSize-1);
        }

        u8* newBuffer = LNEW u8[newSize];

        for(s32 i=0; i<size_; ++i){
            newBuffer[i] = buffer_[i];
        }
        LDELETE_ARRAY(buffer_);

        buffer_ = newBuffer;
        size_ = newSize;
    }


    //----------------------------------------------------------
    //---
    //--- ibstream
    //---
    //----------------------------------------------------------
    class ibstream : public bstream_base<istream>
    {
    public:
        typedef bstream_base<istream> super_type;

        ibstream()
        {}
        ibstream(s32 size, const u8* buffer)
            :super_type(size, const_cast<u8*>(buffer))
        {}

        ~ibstream()
        {}

        virtual s32 read(void* dst, s32 count){ return super_type::read(dst, count);}
    private:
        ibstream(const ibstream&);
        ibstream& operator=(const ibstream&);
    };


    //----------------------------------------------------------
    //---
    //--- obstream
    //---
    //----------------------------------------------------------
    class obstream : public bstream_base<ostream>
    {
    public:
        typedef bstream_base<ostream> super_type;

        obstream()
        {}

        explicit obstream(s32 size)
        {
            LASSERT(0<=size);
            buffer_ = LNEW u8[size];
            current_ = 0;
            size_ = size;
        }

        ~obstream()
        {
            LDELETE_ARRAY(buffer_);
        }

        virtual u32 getSize(){ return current_;}
        virtual s32 write(const void* src, s32 count){ return super_type::write(src, count);}

        const u8* get() const{ return buffer_;}
    private:
        obstream(const obstream&);
        obstream& operator=(const obstream&);
    };

    //----------------------------------------------------------
    //---
    //--- range_stream_base
    //---
    //----------------------------------------------------------
    template<class Base>
    class range_stream_base : public Base
    {
    public:
        range_stream_base(Base* stream, s32 begin, s32 end)
            :stream_(stream)
            ,begin_(begin)
            ,end_(end)
            ,pos_(begin)
        {
            LASSERT(NULL != stream_);
            LASSERT(0<=begin_);
            LASSERT(begin_<=end_);
            LASSERT(check());
        }

        ~range_stream_base()
        {}

        virtual bool eof()
        {
            return (end_<=pos_);
        }

        virtual bool good()
        {
            return stream_->good();
        }

        virtual bool seekg(s32 offset, int dir);

        virtual s32 tellg()
        {
            return pos_;
        }

    protected:
        bool check()
        {
            s32 cur = stream_->tellg();
            stream_->seekg(0, lcore::ios::end);
            s32 end = stream_->tellg();
            stream_->seekg(cur, lcore::ios::beg);

            return (end_<=end);
        }

        Base* stream_;
        s32 begin_;
        s32 end_;
        s32 pos_;
    };

    template<class Base>
    bool range_stream_base<Base>::seekg(s32 offset, int dir)
    {
        switch(dir)
        {
        case ios::beg:
            if(stream_->seekg(offset+begin_, ios::beg)){
                pos_ = offset;
            }else{
                return false;
            }
            break;

        case ios::cur:
            if(stream_->seekg(offset+pos_+begin_, ios::beg)){
                pos_ += offset;
            }else{
                return false;
            }
            break;

        case ios::end:
            if(stream_->seekg(offset+end_, ios::beg)){
                pos_ = end_ - begin_ - offset;
            }else{
                return false;
            }
            break;

        default:
            return false;
        }
        return true;
    }

    //----------------------------------------------------------
    //---
    //--- range_istream
    //---
    //----------------------------------------------------------
    class range_istream : public range_stream_base<istream>
    {
    public:
        typedef range_stream_base<istream> parent_type;

        range_istream(istream* stream, s32 begin, s32 end)
            :parent_type(stream, begin, end)
        {
        }

        ~range_istream()
        {}

        virtual s32 read(void* dst, s32 count)
        {
            if(!stream_->seekg(begin_+pos_, lcore::ios::beg)){
                return 0;
            }
            s32 size = stream_->read(dst, count);

            pos_ += size;
            return size;
        }
    };

    //----------------------------------------------------------
    //---
    //--- range_ostream
    //---
    //----------------------------------------------------------
    class range_ostream : public range_stream_base<ostream>
    {
    public:
        typedef range_stream_base<ostream> parent_type;

        range_ostream(ostream* stream, s32 begin, s32 end)
            :parent_type(stream, begin, end)
        {
        }

        ~range_ostream()
        {}

        virtual s32 write(const void* src, s32 count)
        {
            if(!stream_->seekg(begin_+pos_, lcore::ios::beg)){
                return 0;
            }
            s32 size = stream_->write(src, count);

            pos_ += size;
            return size;
        }
    };

namespace io
{
    template<class T>
    inline s32 write(lcore::ostream& of, const T& value)
    {
        return of.write(static_cast<const void*>(&value), sizeof(T));
    }

    template<class T>
    inline s32 write(lcore::ostream& of, const T* value, s32 size)
    {
        return of.write(static_cast<const void*>(value), size);
    }

    template<class T>
    inline s32 read(lcore::istream& in, T& value)
    {
        return in.read(static_cast<void*>(&value), sizeof(T));
    }

    template<class T>
    inline s32 read(lcore::istream& in, T* value, u32 size)
    {
        return in.read(static_cast<void*>(value), size);
    }

    Char16 readAsUTF16(lcore::istream& is);
    Char16 peekAsUTF16(lcore::istream& is);
    void writeAsUTF8(lcore::ofstream& os, Char16 c);
}
}

#endif //INC_LCORE_LIOSTREAM_
