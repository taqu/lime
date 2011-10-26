/**
@file Parser.cpp
@author t-sakai
@date 2009/01/29 create
@data 2009/05/19 lcoreライブラリ用に変更
*/

#include "Parser.h"
#include "Node.h"
#include <cstring>
#include <cctype>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace xml
{
    //-------------------------------------------------
    //---
    //--- Helper Class
    //---
    //-------------------------------------------------
    /// Nodeクラスのプライベートにアクセスするためのクラス
    class NodeAccess
    {
    public:
        static void addChild(Node* parent, Node* child)
        {
            parent->addChild(child);
        }

        static void setName(Node* node, const char* nameBegin, const char* nameEnd)
        {
            node->setName(nameBegin, nameEnd);
        }

        static void addAttribute(Node* node, const Node::string_type& key, const Node::string_type& value)
        {
            node->addAttribute(key, value);
        }

        static void appendText(Node* node, const char* beginText, const char* endText)
        {
            node->appendText(beginText, endText);
        }
    };


    //-------------------------------------------------
    //---
    //--- Helper Class
    //---
    //-------------------------------------------------
    /// 文字列処理のヘルパー関数
    namespace str
    {
        /// strncmp
        inline s32 strcompare(const char* str1, const char* str2, s32 count)
        {
            return strncmp(str1, str2, count);
        }

        /// 文字列の一致判定
        inline bool isEqual(const char* str1, const char* str2, s32 count)
        {
            return (strcompare(str1, str2, count) == 0);
        }

        /// isspace
        bool isSpace(const char c)
        {
            const unsigned char uc = (const unsigned char)c;
            return (isspace(uc) != 0);
        }

        /// 空白読み飛ばし
        const char* skipSpace(const char* str)
        {
            XML_ASSERT(str != NULL);

            while(*str != '\0'){
                if(!isSpace(*str)){
                    break;
                }
                ++str;
            }
            return str;
        }

        /// 空白以外を読み飛ばし
        const char* skipNotSpace(const char* str)
        {
            XML_ASSERT(str != NULL);

            while(*str != '\0'){
                if(isSpace(*str)){
                    break;
                }
                ++str;
            }
            return str;
        }

        /// 指定数読み飛ばし
        inline const char* skipCount(const char* str, s32 count)
        {
            return (str + count);
        }

        /// 海胆か。正しくは海胆っぽいか
        inline bool isTextUnicode(const char* text, s32 count)
        {
            return (::IsTextUnicode((const void*)text, count, NULL) != 0);
        }

        /**
        @biref ワイド文字列をマルチバイトに変換
        @return 
        @param wide ... 
        @param multi ... 
        @param mutiByte ... 
        */
        inline s32 wideCharToMultiByte(const wchar_t* wide, char* multi, s32 multiByte)
        {
            return ::WideCharToMultiByte(0, 0, wide, -1, multi, multiByte, NULL, NULL);
        }
    }

    Node* Parser::parse(const char* memory)
    {
        const char* text = memory;
        text = BOM(text);

        text = str::skipSpace(text); //空白読み飛ばし
        std::string buffer;

        CharEncode encode = CharEncode_Num;
        text = xmlHead(text, encode);
        //if(encode == CharEncode_Num){
        //    return NULL;
        //}

        if(encode == CharEncode_UTF8){
            //s32 bufferSize = str::wideCharToMultiByte((const wchar_t*)text, NULL, 0);
            //buffer.resize(bufferSize);
            //
            //str::wideCharToMultiByte((const wchar_t*)text, &(buffer[0]), bufferSize);
            //text = &(buffer[0]);
        }

        while(*text != '\0'){
            text = str::skipSpace(text);

            switch(*text){
                case '<':
                    if(*(text+1) == '/'){
                        text = endElementTag(text);
                    }else{
                        text = beginElementTag(text);
                    }
                    break;
                default:
                    text = content(text);
                    break;
            }
        }

        return (_nodeStack.count() > 0)? _nodeStack.top() : NULL;
    }

    const char* Parser::BOM(const char* text)
    {
        if((unsigned char)*text == 0xEF){
            for(s32 i=0; i<3; ++i){
                if(*text == '\0'){
                    break;
                }
                ++text;
            }
        }
        return text;
    }

    const char* Parser::xmlHead(const char* text, CharEncode &encode)
    {
        static const char XML_TAG[] = "<?xml";// version=\"1.0\"";// encoding=";
        static const s32 XML_TAG_COUNT = sizeof(XML_TAG) - 1;
        if(!str::isEqual(text, XML_TAG, XML_TAG_COUNT)){
            return text;
        }
        text = str::skipCount(text, XML_TAG_COUNT);
        text = str::skipSpace(text);

        static const char XML_VERSION[] = "version=\"1.0\"";
        static const s32 XML_VERSION_COUNT = sizeof(XML_VERSION) - 1;
        if(!str::isEqual(text, XML_VERSION, XML_VERSION_COUNT)){
            return text;
        }
        text = str::skipCount(text, XML_VERSION_COUNT);
        text = str::skipSpace(text);

        static const char XML_ENCODING[] = "encoding=";
        static const char XML_ENCODING_COUNT = sizeof(XML_ENCODING) - 1;
        if(str::isEqual(text, XML_ENCODING, XML_ENCODING_COUNT)){
            text = str::skipCount(text, XML_ENCODING_COUNT);

            if(str::isEqual(text, "\"shift-jis\"", 11)){
                encode = CharEncode_SJIS;
                text = str::skipCount(text, 11);

            }else if(str::isEqual(text, "\"utf-8\"", 7)){
                encode = CharEncode_UTF8;
                text = str::skipCount(text, 7);
            }
        }
        text = str::skipCount(text, 2);//"?>"
        return text;
    }

    const char* Parser::beginElementTag(const char* text)
    {
        // 新しいノードエレメントをスタックに積む
        Node *node = XML_NEW Node;
        
        // スタックにノードがあればスタック先頭の子にする
        if(_nodeStack.count() > 0){
            NodeAccess::addChild(_nodeStack.top(), node);
        }

        _nodeStack.push(node);

        ++text; //'<'読み飛ばし
        str::skipSpace(text);

        text = tagName(text);

        while(*text != '\0'){
            text = str::skipSpace(text);
            if(*text == '/'){
                ++text;
                if(*text == '>'){
                    popStack();
                }
            }

            if(*text == '>'){
                return ++text;
            }

            text = attribute(text);
        }
        return text;
    }

    const char* Parser::endElementTag(const char* text)
    {
        while(*text != '\0'){
            if(*text == '>'){
                //スタックの先頭ノードをポップ
                popStack();
                return ++text;
            }
            ++text;
        }
        return text;
    }

    const char* Parser::tagName(const char* text)
    {
        const char *nameBegin = text;
        while(*text != '\0'){
            if(str::isSpace(*text)
                || (*text == '>'))
            {
                //タグ名をスタックの先頭ノードに割り当て
                NodeAccess::setName( _nodeStack.top(), nameBegin, text);
                break;
            }
            ++text;
        }
        return text;
    }

    const char* Parser::attribute(const char* text)
    {
        std::string attrKey, tmpAttrValue;

        const char* strBegin = text;
        while(*text != '\0'){
            if(str::isSpace(*text)){
                attrKey.assign(strBegin, text);
                text = str::skipSpace(text);
                if(*text == '='){
                    ++text;
                    break;
                }
            }

            if(*text == '='){
                attrKey.assign(strBegin, text);
                ++text;
                break;
            }
            ++text;
        }

        text = str::skipSpace(text);
        if(*text != '\"'){
            return text;
        }
        ++text;

        strBegin = text;
        while(*text != '\0'){
            if(*text == '\\'){
                text += 2;
            }

            if(*text == '\"'){
                tmpAttrValue.assign(strBegin, text);
                ++text;
                break;
            }
            ++text;
        }

        std::string attrValue;
        for(std::string::iterator itr = tmpAttrValue.begin();
            itr != tmpAttrValue.end();
            ++itr)
        {
            if(*itr == '\\'){
                continue;
            }
            attrValue.push_back(*itr);
        }

        //アトリビュートをスタック先頭に追加
        NodeAccess::addAttribute( _nodeStack.top(), attrKey, attrValue);
        return text;
    }

    const char* Parser::content(const char* text)
    {
        const char* beginContent = text;
        while(*text != '\0'){
            if(*text == '<'){
                NodeAccess::appendText(_nodeStack.top(), beginContent, text);
                break;
            }
            ++text;
        }
        return text;
    }
}
