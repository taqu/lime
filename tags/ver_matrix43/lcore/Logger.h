#ifndef INC_LCORE_LOGGER_H__
#define INC_LCORE_LOGGER_H__
/**
@file Logger.h
@author t-sakai
@date 2010/03/16 create
*/
#include "lcore.h"

namespace lcore
{
    struct CharTraitsMultiByte
    {
        typedef char element_type;
        typedef char* pointer_type;
        typedef char& reference_type;

        typedef const char* const_pointer_type;
        typedef const char& const_reference_type;

        static const char line_feed = '\n';
        static const char null = '\0';
    };

    template<typename char_traits>
    class DebugOutputter
    {
    public:
        typedef typename char_traits::element_type element_type;
        typedef typename char_traits::pointer_type pointer_type;
        typedef typename char_traits::reference_type reference_type;

        typedef typename char_traits::const_pointer_type const_pointer_type;
        typedef typename char_traits::const_reference_type const_reference_type;

        inline static void print(const_pointer_type str)
        {
            if(str != NULL){
                OutputDebugString(str);
            }
        }

        inline static void print(element_type c)
        {
            element_type buff[2];
            buff[0] = c;
            buff[1] = char_traits::null;
            OutputDebugString(buff);
        }
    };

    template<typename char_traits>
    class DebugOutputterStdOut
    {
    public:
        typedef typename char_traits::element_type element_type;
        typedef typename char_traits::pointer_type pointer_type;
        typedef typename char_traits::reference_type reference_type;

        typedef typename char_traits::const_pointer_type const_pointer_type;
        typedef typename char_traits::const_reference_type const_reference_type;

        inline static void print(const_pointer_type str)
        {
            if(str != NULL){
                fputs(str, stderr);
            }
        }

        inline static void print(element_type c)
        {
            int i = c;
            fputc(i, stderr);
        }
    };

#if defined(ANDROID)
    template<typename char_traits>
    class DebugOutputterAndroid
    {
    public:
        typedef typename char_traits::element_type element_type;
        typedef typename char_traits::pointer_type pointer_type;
        typedef typename char_traits::reference_type reference_type;

        typedef typename char_traits::const_pointer_type const_pointer_type;
        typedef typename char_traits::const_reference_type const_reference_type;

        inline static void print(const_pointer_type str)
        {
            if(str != NULL){
                __android_log_print(ANDROID_LOG_DEBUG, "lime", str);
            }
        }

        inline static void print(element_type c)
        {
            element_type buff[2];
            buff[0] = c;
            buff[1] = char_traits::null;
            __android_log_print(ANDROID_LOG_DEBUG, "lime", buff);
        }
    };
#endif
  
    template<
        typename char_traits = CharTraitsMultiByte,
        typename Outputter = DebugOutputter<char_traits> >
    class TLogger
    {
    public:
        typedef typename char_traits::element_type element_type;
        typedef typename char_traits::pointer_type pointer_type;
        typedef typename char_traits::reference_type reference_type;

        typedef typename char_traits::const_pointer_type const_pointer_type;
        typedef typename char_traits::const_reference_type const_reference_type;

        static const_pointer_type InfoPrefix;
        static const_pointer_type WarnPrefix;
        static const_pointer_type ErrorPrefix;

        static void initialize()
        {
            if(instance_ == NULL){
                instance_ = LIME_NEW TLogger;
            }
        }

        static void terminate()
        {
            LIME_DELETE(instance_);
        }

        class Info
        {
        public:
            ~Info()
            {
                logger_->print(char_traits::line_feed);
            }

            const Info& operator<<(typename TLogger::const_pointer_type str) const
            {
                logger_->print(str);
                return *this;
            }

        private:
            Info(TLogger* logger)
                :logger_(logger)
            {
                logger_->print(TLogger::InfoPrefix);
            }

            friend class TLogger;

            const TLogger* logger_;
        };

        class Warn
        {
        public:
            ~Warn()
            {
                logger_->print(char_traits::line_feed);
            }

            const Warn& operator<<(typename TLogger::const_pointer_type str) const
            {
                logger_->print(str);
                return *this;
            }

        private:
            Warn(TLogger* logger)
                :logger_(logger)
            {
                logger_->print(TLogger::WarnPrefix);
            }

            friend class TLogger;

            const TLogger* logger_;
        };

        class Error
        {
        public:
            
            ~Error()
            {
                logger_->print(char_traits::line_feed);
            }

            const Error& operator<<(typename TLogger::const_pointer_type str) const
            {
                logger_->print(str);
                return *this;
            }

        private:
            Error(TLogger* logger)
                :logger_(logger)
            {
                logger_->print(TLogger::ErrorPrefix);
            }

            friend class TLogger;

            const TLogger* logger_;
        };

        static Info info(){ return Info(instance_); }
        static Warn warn(){ return Warn(instance_); }
        static Error error(){ return Error(instance_); }

    protected:
        friend class TLogger::Info;
        friend class TLogger::Warn;
        friend class TLogger::Error;

        TLogger()
        {}
        ~TLogger()
        {}


        void print(const_pointer_type str) const
        {
            Outputter::print(str);
        }

        void print(element_type c) const
        {
            Outputter::print(c);
        }


        static TLogger *instance_;
    };

    template<typename char_traits, typename Outputter>
    TLogger<char_traits, Outputter>* TLogger<char_traits, Outputter>::instance_ = NULL;

    template<typename char_traits, typename Outputter>
    typename TLogger<char_traits, Outputter>::const_pointer_type TLogger<char_traits, Outputter>::InfoPrefix = "[Info]";

    template<typename char_traits, typename Outputter>
    typename TLogger<char_traits, Outputter>::const_pointer_type TLogger<char_traits, Outputter>::WarnPrefix = "[Warn]";

    template<typename char_traits, typename Outputter>
    typename TLogger<char_traits, Outputter>::const_pointer_type TLogger<char_traits, Outputter>::ErrorPrefix = "[Error]";
}

#endif //INC_LCORE_LOGGER_H__
