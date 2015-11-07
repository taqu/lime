/**
@file Node.cpp
@author t-sakai
@date 2009/01/29 create
@data 2009/05/19 lcoreライブラリ用に変更
@data 2015/11/07 STL排除
*/
#include "Node.h"
#include <algorithm>

#ifdef _MSC_VER
#include <iterator>
#endif

namespace xml
{
    //----------------------------------------------------------------------
    //---
    //--- Node
    //---
    //----------------------------------------------------------------------
    Node::Node()
        :attributes_(0),
        children_(0)
    {
    }

    //----------------------------------------------------------------------
    Node::~Node()
    {
        for(AttrList::iterator itr = attributes_.begin(); itr != attributes_.end(); ++itr){
            XML_DELETE(*itr);
        }

        for(NodeList::iterator itr = children_.begin(); itr != children_.end(); ++itr){
            XML_DELETE(*itr);
        }
    }

    //----------------------------------------------------------------------
    // 属性をキー名で検索
    const Node::string_type* Node::getAttributeByName(const string_type& key) const
    {
        for(AttrList::const_iterator itr = attributes_.begin();
            itr != attributes_.end();
            ++itr)
        {
            const string_type &registeredKey = (*itr)->first_;
            if(registeredKey == key){
                return &( (*itr)->second_ );
            }
        }
        return NULL;
    }

    //----------------------------------------------------------------------
    // 子を追加
    void Node::addChild(xml::Node *child)
    {
        NodeList tmpList(children_.size() + 1);

        for(s32 i=0; i<children_.size(); ++i){
            tmpList[i] = children_[i];
        }
        tmpList[ children_.size() ] = child;

        children_.swap(tmpList);
    }

    //----------------------------------------------------------------------
    // 属性を追加
    void Node::addAttribute(const string_type& key, const string_type& value)
    {
        AttrList tmpList(attributes_.size() + 1);
        for(s32 i=0; i<attributes_.size(); ++i){
            tmpList[i] = attributes_[i];
        }

        tmpList[ attributes_.size() ] = XML_NEW string_pair(key, value);

        attributes_.swap(tmpList);
    }
}
