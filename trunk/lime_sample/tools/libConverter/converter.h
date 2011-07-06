#ifndef INC_CONVERTER_H__
#define INC_CONVERTER_H__
/**
@file converter.h
@author t-sakai
@date 2011/01/25 create

*/
#include <lcore/lcore.h>
#include <lcore/HashMap.h>
#include <lcore/vector.h>
#include <lcore/String.h>

//#define LIME_LIBCONVERTER_DEBUGLOG_ENABLE (1)
#define LIME_LIBCONVERT_PMD_USE_MULTISTREAM
#define LIME_LIBCONVERT_USE_ETC1
//#define LIME_LIBCONVERT_USE_MIPMAP_DEBUG

namespace lgraphics
{
    class TextureRef;
    class SamplerState;
}

namespace lconverter
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;

    using lcore::u8;
    using lcore::u16;
    using lcore::u32;

    using lcore::f32;

    using lcore::Char;

    static const u32 MinTextureSizeToCreateMipMap = 32; //テクスチャの１辺がこれ以上ならミップマップ作成
    static const u32 MinTextureSizeToCompress = 32; //テクスチャの１辺がこれ以上なら圧縮

    s16 F32ToS16(f32 value);
    s16 F32ToS16Clamp(f32 value);
    u16 F32ToU16(f32 value);

    typedef lcore::HashMapCharArray<lgraphics::TextureRef*> NameTextureMap;
    lgraphics::TextureRef* loadTexture(const Char* path, u32 size, const Char* directory, NameTextureMap& texMap, lgraphics::SamplerState& sampler, bool transpose=false);


    enum TextureAddress
    {
        TexAddress_Wrap,
        //TexAddress_Mirror,
        TexAddress_Clamp,
    };

    f32 texAddress(f32 value, TextureAddress op);

    void strSJISToUTF8(Char* dst, u32 buffSize);
    void extractFileNameUTF8(Char* dst);

    typedef lcore::vector_arena<Char> S8Vector;
    typedef lcore::vector_arena<s16> S16Vector;
    typedef lcore::vector_arena<f32> F32Vector;
    typedef lcore::vector_arena<u16> U16Vector;
    typedef lcore::vector_arena<u32> U32Vector;

    typedef lcore::String<256> StringBuffer;

    //--------------------------------------------------------------
    //---
    //--- Config
    //---
    //--------------------------------------------------------------
    /**
    @brief ロードコンフィグ
    */
    class Config
    {
    public:
        static Config& getInstance(){ return instance_;}

        void reset()
        {
            alphaTest_ = false;
            textureCompress_ = false;
        }

        bool isAlphaTest() const{ return alphaTest_;}
        void setAlphaTest(bool enable){ alphaTest_ = enable;}

        bool isTextureCompress() const{ return textureCompress_;}
        void setTextureCompress(bool enable){ textureCompress_ = enable;}

        bool useMipMap() const{ return useMipMap_;}
        void setUseMipMap(bool enable){ useMipMap_ = enable;}
    private:
        Config()
            :alphaTest_(false)
            ,textureCompress_(false)
            ,useMipMap_(false)
        {}

        static Config instance_;

        bool alphaTest_;
        bool textureCompress_;
        bool useMipMap_;
    };

    /**
    @brief 挿入ソート
    */
    template<class T, class U, class LessThan>
    void insertionSort(U num, T* elems, LessThan lessThan)
    {
        T tmp;
        U j;
        for(U i=1; i<num; ++i){
            tmp = elems[i];
            for(j=i-1; 0<=j; --j){
                if(! lessThan(tmp, elems[j])){
                    break;
                }
                elems[j+1] = elems[j];
            }
            elems[j+1] = tmp;
        }
    }

    /**
    @brief クイックソート
    */
    template<class T, class U, class LessThan, unsigned int Threshold, unsigned int StackSize>
    void quickSort(U num, T* elems, LessThan lessThan)
    {
        U leftStack[StackSize];
        U rightStack[StackSize];

        U left = 0;
        U right = num - 1;
        U p = 0;
        U i, j;
        T elem;
        T tmp;
        u32 leftNum, rightNum;

        for(;;){
            if((right - left) <= Threshold){
                if(p==0){
                    break;
                }
                --p;
                left = leftStack[p];
                right = rightStack[p];
            }

            elem = elems[(left+right)>>1]; //真ん中選ぶ
            i = left;
            j = right;

            for(;;){
                while(lessThan(elems[i], elem)){
                    ++i;
                }

                while(lessThan(elem, elems[j])){
                    --j;
                }

                if(i>=j){
                    break;
                }
                tmp = elems[i];
                elems[i] = elems[j];
                elems[j] = tmp;
                ++i;
                --j;
            }

            leftNum = (i-left);
            rightNum = (right-j);
            if(leftNum>rightNum){
                if(leftNum>Threshold){
                    leftStack[p] = left;
                    rightStack[p] = i-1;
                    ++p;
                }
                left = j + 1;
            }else{
                if(rightNum>Threshold){
                    leftStack[p] = j + 1;
                    rightStack[p] = right;
                    ++p;
                }
                right = i-1;
            }
        }

        lconverter::insertionSort(num, elems, lessThan);
    }


    //----------------------------------------------------------------------
    //---
    //--- DebugLog
    //---
    //----------------------------------------------------------------------
    class DebugLog
    {
    public:
        inline DebugLog();
        inline ~DebugLog();

        inline void reset();

        inline u32 getNumVertices() const;
        inline u32 getNumBatches() const;

        inline void setNumVertices(u32 num);
        inline void setNumBatches(u32 num);

        inline void addNumVertices(u32 num);
        inline void addNumBatches(u32 num);

        inline DebugLog& operator=(const DebugLog& rhs);
        inline DebugLog& operator+=(const DebugLog& rhs);

    private:
        u32 numVertices_;
        u32 numBatches_;
    };

    inline DebugLog::DebugLog()
        :numVertices_(0)
        ,numBatches_(0)
    {
    }

    inline DebugLog::~DebugLog()
    {
    }

    inline void DebugLog::reset()
    {
        numVertices_ = 0;
        numBatches_ = 0;
    }

    inline u32 DebugLog::getNumVertices() const
    {
        return numVertices_;
    }

    inline u32 DebugLog::getNumBatches() const
    {
        return numBatches_;
    }

    inline void DebugLog::setNumVertices(u32 num)
    {
        numVertices_ = num;
    }

    inline void DebugLog::setNumBatches(u32 num)
    {
        numBatches_ = num;
    }

    inline void DebugLog::addNumVertices(u32 num)
    {
        numVertices_ += num;
    }

    inline void DebugLog::addNumBatches(u32 num)
    {
        numBatches_ += num;
    }

    inline DebugLog& DebugLog::operator=(const DebugLog& rhs)
    {
        numVertices_ = rhs.numVertices_;
        numBatches_ = rhs.numBatches_;
        return *this;
    }

    inline DebugLog& DebugLog::operator+=(const DebugLog& rhs)
    {
        numVertices_ += rhs.numVertices_;
        numBatches_ += rhs.numBatches_;
        return *this;
    }
}

#endif //INC_CONVERTER_H__
