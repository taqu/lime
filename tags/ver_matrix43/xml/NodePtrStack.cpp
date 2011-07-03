/**
@file NodePtrStack.cpp
@author t-sakai
@date 2009/02/02 create
@data 2009/05/19 lcoreライブラリ用に変更
*/
#include <algorithm>
#include "NodePtrStack.h"

namespace xml
{
    //-------------------------------------------------
    // プッシュ
    void NodePtrStack::push(xml::Node *node)
    {
        // スタック個数インクリメント
        ++_count;

        // 配列サイズを超えたら、配列を２倍に拡張
        if(_count >= _nodes.size()){
            expand(_nodes.size() * 2);
        }

        _nodes[_count - 1] = node;
    }

    //-------------------------------------------------
    // スタック領域を拡張
    void NodePtrStack::expand(size_t newSize)
    {
        XML_ASSERT(_nodes.size() <= newSize);

        NodeArray newStack(newSize);

        std::copy(_nodes.begin(), _nodes.end(), newStack.begin());

        _nodes.swap(newStack);
    }
}
