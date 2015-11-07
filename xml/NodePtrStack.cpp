/**
@file NodePtrStack.cpp
@author t-sakai
@date 2009/02/02 create
@data 2009/05/19 lcoreライブラリ用に変更
*/
#include "NodePtrStack.h"

namespace xml
{
    //-------------------------------------------------
    // プッシュ
    void NodePtrStack::push(xml::Node *node)
    {
        // スタック個数インクリメント
        ++count_;

        // 配列サイズを超えたら、配列を２倍に拡張
        if(count_ >= nodes_.size()){
            expand(nodes_.size() * 2);
        }

        nodes_[count_ - 1] = node;
    }

    //-------------------------------------------------
    // スタック領域を拡張
    void NodePtrStack::expand(s32 newSize)
    {
        XML_ASSERT(nodes_.size() <= newSize);

        NodeArray newStack(newSize);

        for(NodeArray::size_type i=0; i<nodes_.size(); ++i){
            newStack[i] = nodes_[i];
        }
        nodes_.swap(newStack);
    }
}
