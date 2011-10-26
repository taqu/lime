/**
@file Writer.cpp
@author t-sakai
@date 2011/08/18
*/
#include "Writer.h"
#include "Node.h"

namespace xml
{
    bool Writer::write(const Node* root, const Char* filepath)
    {
        if(root == NULL){
            return true;
        }

        file_ = fopen(filepath, "wb");
        if(file_ == NULL){
            return false;
        }

        writeNode(root);

        fclose(file_);
        return true;
    }

    void Writer::writeNode(const Node* node)
    {
        const xml::Node::string_type& name = node->getName();
        fputc('<', file_);
        fwrite(name.c_str(), name.size(), 1, file_);

        for(u32 i=0; i<node->attributeNum(); ++i){
            const xml::Node::string_pair& attr = *( node->getAttribute(i) );
            fputc(' ', file_);
            fwrite(attr._first.c_str(), attr._first.size(), 1, file_);
            fwrite("=\"", 2, 1, file_);
            fwrite(attr._second.c_str(), attr._second.size(), 1, file_);
            fputc('\"', file_);
        }
        fputc('>', file_);

        if(node->getText().size()>0){
            fwrite(node->getText().c_str(), node->getText().size(), 1, file_);
        }

        if(node->childNum()>0){
            fputc('\n', file_);
        }

        for(u32 i=0; i<node->childNum(); ++i){
            writeNode(node->getChild(i));
        }

        fwrite("</", 2, 1, file_);
        fwrite(name.c_str(), name.size(), 1, file_);
        fwrite(">\n", 2, 1, file_);
    }
}
