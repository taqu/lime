#ifndef INC_XML_PARSER_H__
#define INC_XML_PARSER_H__
/**
@file Parser.h
@author t-sakai
@date 2009/01/29 create
@data 2009/05/19 lcoreライブラリ用に変更
*/
#include "xml_core.h"
#include "NodePtrStack.h"

namespace xml
{
    class Node;

    /// XMLパーサ
    class Parser
    {
    public:
        /// コンストラクタ
        Parser()
        {
        }

        /// デストラクタ
        ~Parser()
        {
        }

        /**
        @brief XMLテキストのパース
        @return ルートノード
        @param memory ... XMLテキスト
        */
        Node* parse(const Char* memory);

    private:
        /// 文字コード
        enum CharEncode
        {
            CharEncode_SJIS,
            CharEncode_UTF8,
            CharEncode_Num,
        };

        const Char* xmlHead(const Char* text, CharEncode &encode);
        const Char* beginElementTag(const Char* text);
        const Char* endElementTag(const Char* text);
        const Char* attribute(const Char* text);
        const Char* content(const Char* text);

        const Char* tagName(const Char* text);

        const Char* BOM(const Char* text);

        /// スタックポップ
        void popStack()
        {
            //スタックにひとつしかなければポップしない
            if(nodeStack_.count() > 1){
                nodeStack_.pop();
            }
        }

        /// ノード階層のロールバック用スタック
        NodePtrStack nodeStack_;
    };
}

#endif //INC_XML_PARSER_H__
