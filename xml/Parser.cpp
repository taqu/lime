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

        static void setName(Node* node, const Char* nameBegin, const Char* nameEnd)
        {
            node->setName(nameBegin, nameEnd);
        }

        static void addAttribute(Node* node, const Node::string_type& key, const Node::string_type& value)
        {
            node->addAttribute(key, value);
        }

        static void appendText(Node* node, const Char* beginText, const Char* endText)
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
        inline s32 strcompare(const Char* str1, const Char* str2, s32 count)
        {
            return strncmp(str1, str2, count);
        }

        /// 文字列の一致判定
        inline bool isEqual(const Char* str1, const Char* str2, s32 count)
        {
            return (strcompare(str1, str2, count) == 0);
        }

        /// isspace
        bool isSpace(Char c)
        {
            return (isspace(c) != 0);
        }

        /// 空白読み飛ばし
        const Char* skipSpace(const Char* str)
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
        const Char* skipNotSpace(const Char* str)
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
        inline const Char* skipCount(const Char* str, s32 count)
        {
            return (str + count);
        }

        /// 海胆か。正しくは海胆っぽいか
        inline bool isTextUnicode(const Char* text, s32 count)
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
        inline s32 wideCharToMultiByte(const wchar_t* wide, Char* multi, s32 multiByte)
        {
            return ::WideCharToMultiByte(0, 0, wide, -1, multi, multiByte, NULL, NULL);
        }
    }

    Node* Parser::parse(const Char* memory)
    {
        const Char* text = memory;
        text = BOM(text);

        text = str::skipSpace(text); //空白読み飛ばし

        CharEncode encode = CharEncode_Num;
        text = xmlHead(text, encode);
        //if(encode == CharEncode_Num){
        //    return NULL;
        //}

        if(encode == CharEncode_UTF8){
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

        return (nodeStack_.count() > 0)? nodeStack_.top() : NULL;
    }

    const Char* Parser::BOM(const Char* text)
    {
        if(*text == 0xEF){
            for(s32 i=0; i<3; ++i){
                if(*text == '\0'){
                    break;
                }
                ++text;
            }
        }
        return text;
    }

    const Char* Parser::xmlHead(const Char* text, CharEncode &encode)
    {
        static const Char XML_TAG[] = "<?xml";// version=\"1.0\"";// encoding=";
        static const s32 XML_TAG_COUNT = sizeof(XML_TAG) - 1;
        if(!str::isEqual(text, XML_TAG, XML_TAG_COUNT)){
            return text;
        }
        text = str::skipCount(text, XML_TAG_COUNT);
        text = str::skipSpace(text);

        static const Char XML_VERSION[] = "version=\"1.0\"";
        static const s32 XML_VERSION_COUNT = sizeof(XML_VERSION) - 1;
        if(!str::isEqual(text, XML_VERSION, XML_VERSION_COUNT)){
            return text;
        }
        text = str::skipCount(text, XML_VERSION_COUNT);
        text = str::skipSpace(text);

        static const Char XML_ENCODING[] = "encoding=";
        static const Char XML_ENCODING_COUNT = sizeof(XML_ENCODING) - 1;
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

    const Char* Parser::beginElementTag(const Char* text)
    {
        // 新しいノードエレメントをスタックに積む
        Node *node = XML_NEW Node;
        
        // スタックにノードがあればスタック先頭の子にする
        if(nodeStack_.count() > 0){
            NodeAccess::addChild(nodeStack_.top(), node);
        }

        nodeStack_.push(node);

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

    const Char* Parser::endElementTag(const Char* text)
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

    const Char* Parser::tagName(const Char* text)
    {
        const Char *nameBegin = text;
        while(*text != '\0'){
            if(str::isSpace(*text)
                || (*text == '>'))
            {
                //タグ名をスタックの先頭ノードに割り当て
                NodeAccess::setName( nodeStack_.top(), nameBegin, text);
                break;
            }
            ++text;
        }
        return text;
    }

    const Char* Parser::attribute(const Char* text)
    {
        lcore::DynamicString attrKey, tmpAttrValue;

        const Char* strBegin = text;
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

        lcore::DynamicStringBuilder stringBuilder(tmpAttrValue.length()+1);
        for(lcore::DynamicString::iterator itr = tmpAttrValue.begin();
            itr != tmpAttrValue.end();
            ++itr)
        {
            if(*itr == '\\'){
                continue;
            }
            stringBuilder.push_back(*itr);
        }

        lcore::DynamicString attrValue;
        stringBuilder.toString(attrValue);

        //アトリビュートをスタック先頭に追加
        NodeAccess::addAttribute( nodeStack_.top(), attrKey, attrValue);
        return text;
    }

    const Char* Parser::content(const Char* text)
    {
        const Char* beginContent = text;
        while(*text != '\0'){
            if(*text == '<'){
                NodeAccess::appendText(nodeStack_.top(), beginContent, text);
                break;
            }
            ++text;
        }
        return text;
    }
}
