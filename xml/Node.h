#ifndef INC_XML_NODE_H__
#define INC_XML_NODE_H__
/**
@file Node.h
@author t-sakai
@date 2009/02/05 create
@data 2009/05/19 lcoreライブラリ用に変更
@data 2015/11/07 STL排除
*/
#include "xml_core.h"
#include <lcore/String.h>
#include <lcore/FixedArray.h>

namespace xml
{
    template<class FirstType, class SecondType>
    struct Pair
    {
        typedef FirstType first_type;
        typedef SecondType second_type;

        Pair()
            :first_(first_type())
            ,second_(second_type())
        {
        }

        Pair(const first_type& first, const second_type& second)
            :first_(first)
            ,second_(second)
        {
        }

        Pair(const Pair<first_type, second_type>& pair)
            :first_(pair.first_),
            second_(pair.second_)
        {

        }

        first_type first_;
        second_type second_;
    };

    // Nodeクラスのプライベートにアクセスするためのクラス
    class NodeAccess;

    /// XMLノードクラス
    class Node
    {
    public:
        typedef lcore::DynamicString string_type;
        typedef Pair<string_type, string_type> string_pair;
        typedef lcore::FixedArray<string_pair*> AttrList;
        typedef lcore::FixedArray<Node*> NodeList;

        /// コンストラクタ
        Node();

        /// デストラクタ
        ~Node();

        /**
        @return ノード名
        */
        const string_type& getName() const
        {
            return name_;
        }

        /**
        @return ノード名
        */
        string_type& getName()
        {
            return name_;
        }

        /**
        @return ノードの内容の文字列
        */
        const string_type& getText() const
        {
            return text_;
        }

        /**
        @return ノードの内容の文字列
        */
        string_type& getText()
        {
            return text_;
        }

        /**
        @return 属性数
        */
        s32 attributeNum() const
        {
            return attributes_.size();
        }

        /**
        @brief 属性をキー名で検索
        @return 属性の文字列。なければNULL
        @param key ... キー文字列
        */
        const string_type* getAttributeByName(const string_type& key) const;

        /**
        @brief 属性をインデックスで検索
        @return 属性の文字列
        @param index ... インデックス
        @notice インデックスは0以上属性数未満でなければならない
        */
        const string_pair* getAttribute(size_t index) const
        {
            return attributes_[index];
        }

        /**
        @return 子の数
        */
        s32 getNumChildren() const
        {
            return children_.size();
        }

        /**
        @brief 子をインデックスで検索
        @return 子ノード
        @param index ... インデックス
        @notice インデックスは0以上子の数未満でなければならない
        */
        const Node* getChild(size_t index) const
        {
            return children_[index];
        }

        /**
        @brief 子を追加
        @param child ... 子ノード
        */
        void addChild(Node* child);

        /**
        @brief 属性を追加
        @param key ... キー文字列
        @param value ... 値文字列
        */
        void addAttribute(const string_type& key, const string_type& value);
    private:
        // このクラスを通じてだけアクセスさせる
        friend class NodeAccess;

        /**
        @brief ノードの名前を設定
        @param nameBegin ... 名前文字列の先頭
        @param nameEnd ... 名前文字列の末尾のひとつ後
        */
        void setName(const Char* nameBegin, const Char* nameEnd)
        {
            name_.assign(nameBegin, nameEnd);
        }

        /**
        @brief ノードの内容文字列に、文字列を追加
        @param beginText ... 追加文字列の先頭
        @param endText ... 追加文字列の末尾のひとつ後
        */
        void appendText(const Char* beginText, const Char* endText)
        {
            text_.append(beginText, endText);
        }

        /// ノード名
        string_type name_;

        /// 属性キー名と属性文字列のペアのリスト
        AttrList attributes_;

        /// ノードの内容文字列
        string_type text_;

        /// 子ノードのリスト
        NodeList children_;
    };
}
#endif //INC_XML_NODE_H__
