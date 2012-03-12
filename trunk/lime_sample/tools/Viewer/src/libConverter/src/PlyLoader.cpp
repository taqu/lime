/**
@file PlyLoader.cpp
@author t-sakai
@date 2009/01/29 create
*/
#include <memory>
#include <cstdlib>
#include <string>
#include <tools/container/Array.h>

#include <lcore/lcore.h>
#include <lcore/Variant.h>
#include <lmath/Vector3.h>

#include "PlyLoader.h"

using namespace lcore;

namespace lloader
{
    //-----------------------------------------
    //---
    //--- PLY の予約語
    //---
    //-----------------------------------------
    static const char FORMAT_MAGIC[] = "ply";

    static const char FORMAT_ASCII[] = "ascii";
    static const char FORMAT_BINARY_LITTLE[] = "binary_little_endian";
    static const char FORMAT_BINARY_BIG[] = "binary_big_endian";
    static const char VERSION[] = "1.0";

    static const char TAG_FORMAT[] = "format";
    static const char TAG_COMMENT[] = "comment";
    static const char TAG_ELEMENT[] = "element";
    static const char TAG_PROPERTY[] = "property";
    static const char TAG_LIST[] = "list";
    static const char TAG_END_HEADER[] = "end_header";

    static const char TAG_VERTEX[] = "vertex";
    static const char TAG_FACE[] = "face";
    static const char TAG_CONFIDENCE[] = "confidence";
    static const char TAG_INTENSITY[] = "intensity";
    static const char TAG_VERTEX_INDICES[] = "vertex_indices";

    static const char TYPE_STR_CHAR[] = "char";
    static const char TYPE_STR_UCHAR[] = "uchar";
    static const char TYPE_STR_SHORT[] = "short";
    static const char TYPE_STR_USHORT[] = "ushort";
    static const char TYPE_STR_INT[] = "int";
    static const char TYPE_STR_UINT[] = "uint";
    static const char TYPE_STR_FLOAT[] = "float";
    static const char TYPE_STR_DOUBLE[] = "double";

    static const int FORMAT_MAGIC_SIZE = sizeof(FORMAT_MAGIC) - 1;
    static const int FORMAT_ASCII_SIZE = sizeof(FORMAT_ASCII) - 1;
    static const int FORMAT_BINARY_LITTLE_SIZE = sizeof(FORMAT_BINARY_LITTLE) - 1;
    static const int VERSION_SIZE = sizeof(VERSION) - 1;

    static const int TAG_FORMAT_SIZE = sizeof(TAG_FORMAT) - 1;
    static const int TAG_COMMENT_SIZE = sizeof(TAG_COMMENT) - 1;
    static const int TAG_ELEMENT_SIZE = sizeof(TAG_ELEMENT) - 1;
    static const int TAG_PROPERTY_SIZE = sizeof(TAG_PROPERTY) - 1;
    static const int TAG_LIST_SIZE = sizeof(TAG_LIST) - 1;
    static const int TAG_END_HEADER_SIZE = sizeof(TAG_END_HEADER) - 1;

    static const int TAG_VERTEX_SIZE = sizeof(TAG_VERTEX) - 1;
    static const int TAG_FACE_SIZE = sizeof(TAG_FACE) - 1;
    static const int TAG_CONFIDENCE_SIZE = sizeof(TAG_CONFIDENCE) - 1;
    static const int TAG_INTENSITY_SIZE = sizeof(TAG_INTENSITY) - 1;
    static const int TAG_VERTEX_INDICES_SIZE = sizeof(TAG_VERTEX_INDICES) - 1;

    static const int TYPE_STR_CHAR_SIZE = sizeof(TYPE_STR_CHAR) - 1;
    static const int TYPE_STR_UCHAR_SIZE = sizeof(TYPE_STR_UCHAR) - 1;
    static const int TYPE_STR_SHORT_SIZE = sizeof(TYPE_STR_SHORT) - 1;
    static const int TYPE_STR_USHORT_SIZE = sizeof(TYPE_STR_USHORT) - 1;
    static const int TYPE_STR_INT_SIZE = sizeof(TYPE_STR_INT) - 1;
    static const int TYPE_STR_UINT_SIZE = sizeof(TYPE_STR_UINT) - 1;
    static const int TYPE_STR_FLOAT_SIZE = sizeof(TYPE_STR_FLOAT) - 1;
    static const int TYPE_STR_DOUBLE_SIZE = sizeof(TYPE_STR_DOUBLE) - 1;


    //-----------------------------------------
    //---
    //--- Helper Functions
    //---
    //-----------------------------------------
    inline const char* textProcess(const char* str, int maxCount)
    {
        str += maxCount;
        return str;
    }

    inline const char* textEqualProcess(const char* str1, const char* str2, int maxCount)
    {
        if(0 == lcore::strncmp(str1, str2, maxCount)){
            str1 += maxCount;
            return str1;
        }
        return NULL;
    }

    inline const char* skipSpace(const char* str)
    {
        while(lcore::isSpace(*str)){
            ++str;
        }
        return str;
    }

    inline const char* skipNotSpace(const char* str)
    {
        while(!lcore::isSpace(*str)){
            ++str;
        }
        return str;
    }

    inline const char* skipLine(const char* str)
    {
        int EOLSize = 0;
        while(0 == (EOLSize = lcore::isEOL(str))
            && false == lcore::isNullChar(*str))
        {
            ++str;
        }
        return str + EOLSize;
    }

    //-----------------------------------------
    //---
    //--- PlyProperty
    //---
    //-----------------------------------------
    struct PlyProperty
    {
        PlyProperty(const char* nameBegin, const char* nameEnd, const lcore::Variant& value)
            :_name(nameBegin, nameEnd),
            _value(value)
        {
        }

        ~PlyProperty()
        {
        }

        static LCore_Type stringToType(const char* str, int& strCount);

        const char* load(const char* text);

        std::string _name;
        Variant _value;
    };

    LCore_Type PlyProperty::stringToType(const char* str, int& strCount)
    {
        strCount = 0;

        if(0 == lcore::strncmp(str, TYPE_STR_CHAR, TYPE_STR_CHAR_SIZE)){
            strCount = TYPE_STR_CHAR_SIZE;
            return LCore_Type_S8;

        }else if(0 == lcore::strncmp(str, TYPE_STR_UCHAR, TYPE_STR_UCHAR_SIZE)){
            strCount = TYPE_STR_UCHAR_SIZE;
            return LCore_Type_U8;

        }else if(0 == lcore::strncmp(str, TYPE_STR_INT, TYPE_STR_INT_SIZE)){
            strCount = TYPE_STR_INT_SIZE;
            return LCore_Type_S32;

        }else if(0 == lcore::strncmp(str, TYPE_STR_UINT, TYPE_STR_UINT_SIZE)){
            strCount = TYPE_STR_UINT_SIZE;
            return LCore_Type_U32;

        }else if(0 == lcore::strncmp(str, TYPE_STR_FLOAT, TYPE_STR_FLOAT_SIZE)){
            strCount = TYPE_STR_FLOAT_SIZE;
            return LCore_Type_F32;

        }else if(0 == lcore::strncmp(str, TYPE_STR_DOUBLE, TYPE_STR_DOUBLE_SIZE)){
            strCount = TYPE_STR_DOUBLE_SIZE;
            return LCore_Type_F64;

        }else if(0 == lcore::strncmp(str, TAG_LIST, TAG_LIST_SIZE)){
            strCount = TAG_LIST_SIZE;
            return LCore_Type_Array;
        }
        return LCore_Type_Num;
    }

    const char* PlyProperty::load(const char* text)
    {
        const char* end = skipNotSpace(text);
        std::string str(text, end);
        switch(_value.getType())
        {
        case LCore_Type_S8:
            {
                s8 val = std::atoi(str.c_str());
                _value.setS8(val);
            }
            break;
        case LCore_Type_U8:
            {
                u8 val = std::atoi(str.c_str());
                _value.setU8(val);
            }
            break;
        case LCore_Type_S32:
            {
                s32 val = std::atoi(str.c_str());
                _value.setS32(val);
            }
            break;
        case LCore_Type_U32:
            {
                u32 val = std::strtoul(str.c_str(), NULL, 10);
                _value.setU32(val);
            }
            break;
        case LCore_Type_F32:
            {
                f32 val = static_cast<f32>( std::atof(str.c_str()) );
                _value.setF32(val);
            }
            break;
        case LCore_Type_F64:
            {
                f64 val = std::strtod(str.c_str(), NULL);
                _value.setF64(val);
            }
            break;
        default:
            break;
        };
        return end;
    }


    //-----------------------------------------
    //---
    //--- PlyElement
    //---
    //-----------------------------------------
    class PlyElement
    {
    public:
        PlyElement();
        ~PlyElement();

        const char* loadDefinition(const char* text);

        const std::string& getName() const
        {
            return _name;
        }

        u32 num() const
        {
            return _num;
        }

        u32 properyNum() const
        {
            return static_cast<u32>( _properties.size() );
        }

        const PlyProperty& getProperty(s32 index) const
        {
            return *(_properties[index]);
        }
    private:
        const char* loadElementDefinition(const char* text);
        const char* loadPropertyDefinition(const char* text);
        const char* loadListPropertyDefinition(const char* text);

        typedef container::Array<PlyProperty*> PropertyArray;

        std::string _name;
        u32 _num;

        PropertyArray _properties;
    };

    PlyElement::PlyElement()
    {
    }

    PlyElement::~PlyElement()
    {
        PropertyArray::iterator end = _properties.end();
        for(PropertyArray::iterator itr = _properties.begin();
            itr != end;
            ++itr)
        {
            LIME_DELETE (*itr);
            (*itr) = NULL;
        }
    }

    const char* PlyElement::loadDefinition(const char* text)
    {
        text = loadElementDefinition(text);
        if(text == NULL){
            return NULL;
        }
        text = skipSpace(text);
        while(0 == lcore::strncmp(text, TAG_PROPERTY, TAG_PROPERTY_SIZE)){
            text = loadPropertyDefinition(text);
            if(text == NULL){
                break;
            }
            text = skipSpace(text);
        }
        return text;
    }

    const char* PlyElement::loadElementDefinition(const char* text)
    {
        text = textProcess(text, TAG_ELEMENT_SIZE);
        if(text == NULL){
            return NULL;
        }

        text = skipSpace(text);
        const char* end = skipNotSpace(text);
        std::string name(text, end);

        text = skipSpace(end);
        end = skipNotSpace(text);
        std::string buff(text, end);

        lcore::u32 num = std::strtoul(buff.c_str(), (char**)NULL, 10);
        text = end;
        text = skipSpace(text);

        _name.swap(name);
        _num = num;
        return text;
    }

    const char* PlyElement::loadPropertyDefinition(const char* text)
    {
        text = textProcess(text, TAG_PROPERTY_SIZE);
        if(text == NULL){
            return NULL;
        }

        text = skipSpace(text);

        int count = 0;
        LCore_Type type = PlyProperty::stringToType(text, count);
        if(type == LCore_Type_Num){
            return NULL;
        }else if(type == LCore_Type_Array){
            return loadListPropertyDefinition(text);
        }

        text += count;
        text = skipSpace(text);
        const char* end = skipNotSpace(text);
        lcore::Variant variant(type);

        // 失敗した場合の解放を行うためのstd::auto_ptr
        std::auto_ptr<PlyProperty> prop( LIME_NEW PlyProperty(text, end, variant) );
        if(prop.get() == NULL){
            return NULL;
        }

        _properties.push_back(prop.get());

        // 無事渡せたので、参照を外す
        prop.release();

        return end;
    }

    const char* PlyElement::loadListPropertyDefinition(const char* text)
    {
        text = textProcess(text, TAG_LIST_SIZE);
        text = skipSpace(text);

        text = textEqualProcess(text, TYPE_STR_UCHAR, TYPE_STR_UCHAR_SIZE);
        if(text == NULL){
            return NULL;
        }

        text = skipSpace(text);

        lcore::s32 count = 0;
        LCore_Type type = PlyProperty::stringToType(text, count);
        if(type == LCore_Type_Num || type == LCore_Type_Array){
            return NULL;
        }
        text += count;

        Variant value(type);

        // 失敗した場合の解放を行うためのstd::auto_ptr
        std::auto_ptr<Variant::VariantArray> arrayImpl( LIME_NEW Variant::VariantArray );
        if(arrayImpl.get() == NULL){
            return NULL;
        }
        arrayImpl->push_back(value);

        Variant array(arrayImpl.get());
        // 無事渡せたので、参照を外す
        arrayImpl.release();

        text = skipSpace(text);
        const char* end = skipNotSpace(text);

        std::auto_ptr<PlyProperty> prop( LIME_NEW PlyProperty(text, end, array) );
        if(prop.get() == NULL){
            return NULL;
        }
        // 無事渡せたので、配列への参照を外す
        array.unref();

        _properties.push_back(prop.get());
        prop.release();

        return end;
    }

    //-----------------------------------------
    //---
    //--- PlyLoaderImpl
    //---
    //-----------------------------------------
    class PlyLoader::PlyLoaderImpl
    {
    public:
        PlyLoaderImpl()
            :_inputText(NULL)
        {
        }

        ~PlyLoaderImpl()
        {
            PlyElementList::iterator end = _elements.end();
            for(PlyElementList::iterator itr = _elements.begin();
                itr != end;
                ++itr)
            {
                LIME_DELETE (*itr);
                (*itr) = NULL;
            }
        }


        bool loadHeader(std::istream& istrm);

        u32 getElementNum() const
        {
            return static_cast<u32>( _elements.size() );
        }

        const PlyElement& getElement(s32 index) const
        {
            return *( _elements[index] );
        }

    private:
        bool magic(std::istream& istrm);
        bool format(std::istream& istrm);
        bool version(std::istream& istrm);
        bool comment(cstd::istream& istrm);
        bool element(std::istream& istrm);
        bool endHeader(std::istream& istrm);

        const char* vertexList(const char* text, lmath::Vector3* vertices, u32 count);
        const char* indexList(const char* text, s16* indices, s32 count);

        typedef container::Array<PlyElement*> PlyElementList;
        PlyElementList _elements;

        const char* _inputText;
    };


    bool PlyLoader::PlyLoaderImpl::magic(std::istream& istrm)
    {
        return textEqualProcess(text, FORMAT_MAGIC, FORMAT_MAGIC_SIZE);
    }

    bool PlyLoader::PlyLoaderImpl::format(std::istream& istrm)
    {
        text = textEqualProcess(text, TAG_FORMAT, TAG_FORMAT_SIZE);
        text = skipSpace(text);
        return textEqualProcess(text, FORMAT_ASCII, FORMAT_ASCII_SIZE);
    }

    bool PlyLoader::PlyLoaderImpl::version(std::istream& istrm)
    {
        return textEqualProcess(text, VERSION, VERSION_SIZE);
    }

    bool PlyLoader::PlyLoaderImpl::comment(std::istream& istrm)
    {
        return istrm.getline(NULL, 0);
    }

    bool PlyLoader::PlyLoaderImpl::element(std::istream& istrm)
    {
        std::auto_ptr<PlyElement> elem( LIME_NEW PlyElement );
        if(elem.get() == NULL){
            return NULL;
        }

        text = elem->loadDefinition(text);
        if(text == NULL){
            return NULL;
        }
        _elements.push_back(elem.get());
        elem.release();
        return text;
    }

    bool PlyLoader::PlyLoaderImpl::endHeader(std::istream& istrm)
    {
        return textProcess(text, TAG_END_HEADER_SIZE);
    }

    bool PlyLoader::PlyLoaderImpl::loadHeader(std::istream& istrm)
    {
        _inputText = text;

        _inputText = magic(_inputText);
        _inputText = skipSpace(_inputText);

        _inputText = format(_inputText);
        _inputText = skipSpace(_inputText);

        _inputText = version(_inputText);
        _inputText = skipSpace(_inputText);

        while(_inputText != NULL){
            if( lcore::strncmp(_inputText, TAG_COMMENT, TAG_COMMENT_SIZE) == 0){
                _inputText = comment(_inputText);

            }else if( lcore::strncmp(_inputText, TAG_ELEMENT, TAG_ELEMENT_SIZE) == 0){
                _inputText = element(_inputText);

            }else {
                _inputText = endHeader(_inputText);
                _inputText = skipSpace(_inputText);
                break;
            }
            _inputText = skipSpace(_inputText);
        }
    }

    const char* PlyLoader::PlyLoaderImpl::vertexList(const char* text, lmath::Vector3* vertices, u32 count)
    {
        //if(_elements.size() < 1){
        //    return NULL;
        //}

        //s32 count;

        //PlyElement *elem = _elements[0];
        //elem->properyNum();
        return NULL;
    }

    const char* PlyLoader::PlyLoaderImpl::indexList(const char* text, s16* indices, s32 count)
    {
        return NULL;
    }

    //-----------------------------------------------------------------------------
    //---
    //---
    //---
    //-----------------------------------------------------------------------------
    PlyLoader::PlyLoader()
        :_buffer(NULL)
    {
        _impl = LIME_NEW PlyLoaderImpl;
    }

    PlyLoader::~PlyLoader()
    {
        LIME_DELETE _impl;
        _impl = NULL;
    }

    bool PlyLoader::loadHeader(std::istream& istrm)
    {
        return _impl->loadHeader(istrm);
    }
}
