/**
@file Configuration.cpp
@author t-sakai
@date 2016/09/21 create
*/
#include "Configuration.h"
#include "File.h"

namespace lcore
{
    Configuration::Configuration()
        :handleTable_(32, 32)
        ,capacity_(0)
        ,entries_(NULL)
    {
    }

    Configuration::~Configuration()
    {
        clear();
    }

    bool Configuration::load(const Char* path)
    {
        LASSERT(NULL != path);
        File file(path, ios::in);
        if(!file.is_open()){
            return false;
        }
        clear();
        skipBOM(file);

        s32 length = 0;
        s32 capacity = 0;
        Char* line = NULL;
        while(!file.eof()){
            getLine(length, capacity, line, file);
            if(length<=0){
                continue;
            }
            Char* l = skipSpace(line);
            if(*l == '#'){
                continue;
            }

            const Char* key = l;
            while(!lcore::isNullChar(*l)){
                if(lcore::isSpace(*l)){
                    *l = CharNull;

                }else if('=' == *l){
                    break;
                }
                ++l;
            }
            if(lcore::isNullChar(*key) || *l != '='){
                continue;
            }
            *l = CharNull;
            l = skipSpace(l+1);
            Char* value = l;
            for(;;){
                if((lcore::isAscii(*l) && lcore::isSpace(*l)) || lcore::isNullChar(*l)){
                    *l = CharNull;
                    break;
                }
                ++l;
            }
            if(lcore::isNullChar(*value)){
                continue;
            }
            setKeyValue(key, value);
        }
        LFREE(line);
        return true;
    }

    bool Configuration::save(const Char* path)
    {
        LASSERT(NULL != path);
        File file(path, ios::out);
        if(!file.is_open()){
            return false;
        }
        UChar bom[3] = {0xEF, 0xBB, 0xBF};
        file.write(3, bom);

        for(hashmap_type::size_type pos = keyToHandle_.begin(); pos != keyToHandle_.end(); pos = keyToHandle_.next(pos)){
            s32 index = keyToHandle_.getValue(pos).index();
            Entry& entry = entries_[index];
            LASSERT(NULL != entry.key_);
            file.print("%s=", entry.key_);
            switch(entry.value_.getType()){
            case Any::Type_Int:
                file.print("%d\n", (s32)entry.value_);
                break;
            case Any::Type_Float:
                file.print("%f\n", (f32)entry.value_);
                break;
            case Any::Type_String:
                file.print("\"%s\"\n", (const Char*)entry.value_);
                break;
            }
        }

        return true;
    }

    bool Configuration::exist(const Char* key) const
    {
        LASSERT(NULL != key);
        hashmap_type::size_type pos = keyToHandle_.find(StringWrapper(lcore::strlen_s32(key), key));
        return pos != keyToHandle_.end();
    }

    s32 Configuration::get(const Char* key, s32 defaultValue) const
    {
        LASSERT(NULL != key);
        hashmap_type::size_type pos = keyToHandle_.find(StringWrapper(lcore::strlen_s32(key), key));
        if(pos == keyToHandle_.end()){
            return defaultValue;
        }
        s32 index = keyToHandle_.getValue(pos).index();
        LASSERT(0<=index && index<capacity_);
        switch(entries_[index].value_.getType())
        {
        case Any::Type_Int:
            return (s32)entries_[index].value_;
        case Any::Type_Float:
            return static_cast<s32>((f32)entries_[index].value_);
        default:
            return defaultValue;
        }
    }

    f32 Configuration::get(const Char* key, f32 defaultValue) const
    {
        LASSERT(NULL != key);
        hashmap_type::size_type pos = keyToHandle_.find(StringWrapper(lcore::strlen_s32(key), key));
        if(pos == keyToHandle_.end()){
            return defaultValue;
        }
        s32 index = keyToHandle_.getValue(pos).index();
        LASSERT(0<=index && index<capacity_);
        switch(entries_[index].value_.getType())
        {
        case Any::Type_Int:
            return static_cast<f32>((s32)entries_[index].value_);
        case Any::Type_Float:
            return (f32)entries_[index].value_;
        default:
            return defaultValue;
        }
    }

    Configuration::StringPair Configuration::get(const Char* key, const Char* defaultValue) const
    {
        LASSERT(NULL != key);
        hashmap_type::size_type pos = keyToHandle_.find(StringWrapper(lcore::strlen_s32(key), key));
        if(pos == keyToHandle_.end()){
            if(NULL == defaultValue){
                return {NULL, 0};
            }else{
                return {defaultValue, lcore::strlen_s32(defaultValue)};
            }
        }
        s32 index = keyToHandle_.getValue(pos).index();
        LASSERT(0<=index && index<capacity_);
        const Any& value = entries_[index].value_;
        if(value.getType() == Any::Type_String){
            return {(const Char*)value, value.length()};
        }else{
            return {defaultValue, lcore::strlen_s32(defaultValue)};
        }
    }

    void Configuration::remove(const Char* key)
    {
        LASSERT(NULL != key);
        StringWrapper str(lcore::strlen_s32(key), key);
        hashmap_type::size_type pos = keyToHandle_.find(str);
        if(pos == keyToHandle_.end()){
            return;
        }
        handle_type handle = keyToHandle_.getValue(pos);
        keyToHandle_.eraseAt(pos);

        destroy(handle.index());
        handleTable_.destroy(handle);
    }

    void Configuration::set(const Char* key, s32 value)
    {
        LASSERT(NULL != key);

        handle_type handle = setKey(key);
        entries_[handle.index()].value_ = value;
    }

    void Configuration::set(const Char* key, f32 value)
    {
        LASSERT(NULL != key);

        handle_type handle = setKey(key);
        entries_[handle.index()].value_ = value;
    }

    void Configuration::set(const Char* key, const Char* value)
    {
        LASSERT(NULL != key);
        LASSERT(NULL != value);

        handle_type handle = setKey(key);
        entries_[handle.index()].value_ = value;
    }

    void Configuration::clear()
    {
        for(s32 i=0; i<capacity_; ++i){
            destroy(i);
        }
        handleTable_.clear();
        keyToHandle_.clear();
        capacity_ = 0;
        LFREE(entries_);
    }

    Configuration::handle_type Configuration::setKey(const Char* key)
    {
        StringWrapper str(lcore::strlen_s32(key), key);
        hashmap_type::size_type pos = keyToHandle_.find(str);
        handle_type handle;
        if(pos == keyToHandle_.end()){
            handle = handleTable_.create();

            if(capacity_<handleTable_.capacity()){
                s32 capacity = handleTable_.capacity();
                Entry* entries = (Entry*)LMALLOC(sizeof(Entry)*capacity);
                lcore::memcpy(entries, entries_, sizeof(Entry)*capacity_);
                for(s32 i=capacity_; i<capacity; ++i){
                    entries[i].key_ = NULL;
                    LPLACEMENT_NEW(&entries[i].value_) Any;
                }
                LFREE(entries_);
                capacity_ = capacity;
                entries_ = entries;
            }

            setKey(handle.index(), key);
            keyToHandle_.insert(StringWrapper(str.length_, entries_[handle.index()].key_), handle);
        }else{
            handle = keyToHandle_.getValue(pos);
        }
        return handle;
    }

    void Configuration::setKey(s32 index, const Char* key)
    {
        LASSERT(NULL != key);
        LFREE(entries_[index].key_);
        s32 length = lcore::strlen_s32(key);
        s32 size = sizeof(Char)*(length+1);
        entries_[index].key_ = (Char*)LMALLOC(size);
        lcore::memcpy(entries_[index].key_, key, size);
    }

    void Configuration::destroy(s32 index)
    {
        LFREE(entries_[index].key_);
        entries_[index].value_.~Any();
    }

    void Configuration::setKeyValue(const Char* key, Char* value)
    {
        LASSERT(NULL != key);
        LASSERT(NULL != value);

        s16 type = guessType(value);
        switch(type)
        {
        case Any::Type_Int:
            {
                s32 iv = atol(value);
                set(key, iv);
            }
            break;

        case Any::Type_Float:
            {
                f32 fv = static_cast<f32>(atof(value));
                set(key, fv);
            }
            break;

        case Any::Type_String:
            {
                const Char* sv = extractString(value);
                set(key, sv);
            }
            break;
        }
    }

    void Configuration::skipBOM(File& file)
    {
        if(file.size()<3){
            return;
        }
        s32 c;
        c = file.get();
        if(c != 0xEF){
            file.seek(0, ios::beg);
            return;
        }
        c = file.get();
        if(c != 0xBB){
            file.seek(0, ios::beg);
            return;
        }
        c = file.get();
        if(c != 0xBF){
            file.seek(0, ios::beg);
            return;
        }
    }

    void Configuration::getLine(s32& length, s32& capacity, Char*& line, File& file)
    {
        off_t start = file.tell();

        length = 0;
        while(!file.eof()){
            s32 c = file.get();
            if(0x0D == c || 0x0A == c){
                break;
            }
            ++length;
        }
        if(capacity<=length){
            LFREE(line);
            capacity = 2*(length + 1);
            line = (Char*)LMALLOC(sizeof(Char)*capacity);
        }

        if(0<length){
            file.seek(start, lcore::ios::beg);
            length = 0;
            while(!file.eof()){
                s32 c = file.get();
                if(0x0D == c || 0x0A == c){
                    break;
                }
                line[length] = static_cast<Char>(c);
                ++length;
            }
        }
        line[length] = CharNull;
        while(!file.eof()){
            s32 c = file.peek();
            if(0x0D != c && 0x0A != c){
                break;
            }
            file.get();
        }
    }

    Char* Configuration::skipSpace(Char* str)
    {
        LASSERT(NULL != str);
        while(lcore::isAscii(*str) && lcore::isSpace(*str)){
            ++str;
        }
        return str;
    }

    s16 Configuration::guessType(const Char* str)
    {
        LASSERT(NULL != str);
        LASSERT(!lcore::isNullChar(*str));

        if(*str == '\"'
            || !lcore::isAscii(*str)
            || (!lcore::isAsciiNum(*str) && '.' != *str))
        {
            return Any::Type_String;
        }
        while(!lcore::isNullChar(*str)){
            if(*str == '.'){
                if(lcore::isNullChar(str[1])){
                    return Any::Type_Int;
                }else{
                    return Any::Type_Float;
                }
            }
            ++str;
        }
        return Any::Type_Int;
    }

    Char* Configuration::extractString(Char* str)
    {
        while(*str == '\"'){
            ++str;
        }
        Char* s = str;
        while(!lcore::isNullChar(*s)){
            if(*s == '\"'){
                *s = CharNull;
                break;
            }
            ++s;
        }
        return str;
    }
}
