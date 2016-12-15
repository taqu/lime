#ifndef INC_LCORE_CONFIGURATION_H__
#define INC_LCORE_CONFIGURATION_H__
/**
@file Configuration.h
@author t-sakai
@date 2016/09/21 create
*/
#include "lcore.h"
#include "Any.h"
#include "Array.h"
#include "HashMap.h"
#include "HandleTable.h"

namespace lcore
{
    class ifstream;

    class Configuration
    {
    public:
        struct StringPair
        {
            const Char* str_;
            s32 length_;
        };

        Configuration();
        ~Configuration();

        bool load(const Char* path);
        bool save(const Char* path);

        bool exist(const Char* key) const;

        s32 get(const Char* key, s32 defaultValue) const;
        f32 get(const Char* key, f32 defaultValue) const;
        StringPair get(const Char* key, const Char* defaultValue) const;

        void remove(const Char* key);
        void set(const Char* key, s32 value);
        void set(const Char* key, f32 value);
        void set(const Char* key, const Char* value);
    private:
        Configuration(const Configuration&);
        Configuration& operator=(const Configuration&);

        typedef HandleTable<> handletable_type;
        typedef handletable_type::handle_type handle_type;
        typedef HopscotchHashMap<StringWrapper, handle_type> hashmap_type;

        struct Entry
        {
            Char* key_;
            Any value_;
        };

        void clear();
        handle_type setKey(const Char* key);
        void setKey(s32 index, const Char* key);
        void destroy(s32 index);
        void setKeyValue(const Char* key, Char* value);

        static void skipBOM(lcore::ifstream& is);
        static void getLine(s32& length, s32& capacity, Char*& line, lcore::ifstream& is);
        static Char* skipSpace(Char* str);
        static s16 guessType(const Char* str);
        static Char* extractString(Char* str);

        handletable_type handleTable_;
        hashmap_type keyToHandle_;

        s32 capacity_;
        Entry* entries_;
    };
}
#endif //INC_LCORE_CONFIGURATION_H__
