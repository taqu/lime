/**
@file Node.cpp
@author t-sakai
@date 2009/01/29 create
@data 2009/05/19 lcoreライブラリ用に変更
*/
#include "Node.h"
#include <algorithm>

namespace xml
{
    //-------------------------------------------------
    //---
    //--- Helper Function
    //---
    //-------------------------------------------------
    void deleteStringPair(Node::string_pair*& strPair)
    {
        XML_DELETE(strPair);
        strPair = NULL;
    }


    void deleteNode(Node*& node)
    {
        XML_DELETE(node);
        node = NULL;
    }


    //----------------------------------------------------------------------
    //---
    //--- Node
    //---
    //----------------------------------------------------------------------
    Node::Node()
        :_attributes(0),
        _childs(0)
    {
    }

    //----------------------------------------------------------------------
    Node::~Node()
    {
        std::for_each(_attributes.begin(), _attributes.end(), deleteStringPair);
        std::for_each(_childs.begin(), _childs.end(), deleteNode);
    }

    //----------------------------------------------------------------------
    // 属性をキー名で検索
    const Node::string_type* Node::getAttributeByName(const string_type& key) const
    {
        AttrList::const_iterator end = _attributes.end();
        for(AttrList::const_iterator itr = _attributes.begin();
            itr != end;
            ++itr)
        {
            const string_type &registeredKey = (*itr)->_first;
            if(registeredKey == key){
                return &( (*itr)->_second );
            }
        }
        return NULL;
    }

    //----------------------------------------------------------------------
    // 子を追加
    void Node::addChild(xml::Node *child)
    {
        //効率悪いけれども
        NodeList tmpList(_childs.size() + 1);

        std::copy(_childs.begin(), _childs.end(), tmpList.begin());

        tmpList[ _childs.size() ] = child;

        _childs.swap(tmpList);
    }

    //----------------------------------------------------------------------
    // 属性を追加
    void Node::addAttribute(const string_type& key, const string_type& value)
    {
        //効率悪いけれども
        AttrList tmpList(_attributes.size() + 1);

        std::copy(_attributes.begin(), _attributes.end(), tmpList.begin());

        tmpList[ _attributes.size() ] = XML_NEW string_pair(key, value);

        _attributes.swap(tmpList);
    }
}
