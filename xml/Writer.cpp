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

        if(0 != fopen_s(&file_, filepath, "wb")){
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
        fwrite(name.c_str(), name.length(), 1, file_);

        for(s32 i=0; i<node->attributeNum(); ++i){
            const xml::Node::string_pair& attr = *( node->getAttribute(i) );
            fputc(' ', file_);
            fwrite(attr.first_.c_str(), attr.first_.length(), 1, file_);
            fwrite("=\"", 2, 1, file_);
            fwrite(attr.second_.c_str(), attr.second_.length(), 1, file_);
            fputc('\"', file_);
        }
        fputc('>', file_);

        if(node->getText().length()>0){
            fwrite(node->getText().c_str(), node->getText().length(), 1, file_);
        }

        if(node->getNumChildren()>0){
            fputc('\n', file_);
        }

        for(s32 i=0; i<node->getNumChildren(); ++i){
            writeNode(node->getChild(i));
        }

        fwrite("</", 2, 1, file_);
        fwrite(name.c_str(), name.length(), 1, file_);
        fwrite(">\n", 2, 1, file_);
    }
}
