#include "types.h"
#include "resource.h"
#include <stdio.h>

#include <xml/Parser.h>
#include <xml/Node.h>
#include <xml/Writer.h>

#include "Config.h"
#include "Dialog.h"
#include "Display.h"

using namespace config;


static HINSTANCE g_hInstance = NULL;

BOOL APIENTRY DllMain( HINSTANCE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        g_hInstance = hModule;
        break;

	case DLL_THREAD_ATTACH:
        break;

	case DLL_THREAD_DETACH:
        break;

	case DLL_PROCESS_DETACH:
        g_hInstance = NULL;
        break;
	}
	return TRUE;
}


namespace
{
    static const Char TagRoot[] = "config";
    static const Char TagWidth[] = "width";
    static const Char TagHeight[] = "height";
    static const Char TagRate[] = "rate";
    static const Char TagWindowed[] = "windowed";
    static const Char TagFormat[] = "format";

    static const Char DefaultFilePath[] = "conf.xml";

    void readU32(u32& value, const xml::Node::string_type& str)
    {
        value = abs(atol(str.c_str()));
    }

    void readU32NoneZero(u32& value, const xml::Node::string_type& str)
    {
        u32 tmp = abs(atol(str.c_str()));
        if(tmp > 0){
            value = tmp;
        }
    }

    void readS32(s32& value, const xml::Node::string_type& str)
    {
        value = abs(atol(str.c_str()));
    }

    void writeU32(xml::Node::string_type& str, u32 value)
    {
        Char buff[32] = {0};
        ltoa(value, buff, 10);
        str = buff;
    }

    void writeS32(xml::Node::string_type& str, s32 value)
    {
        Char tmp = (value == 0)? '0' : '1';
        str.push_back(tmp);
    }

    void readConfig(config::Config& conf, const Char* filepath)
    {
        if(filepath == NULL){
            filepath = DefaultFilePath;
        }
        FILE* file = fopen(filepath, "rb");
        if(file != NULL){
            //ファイルロード
            u32 size = 0;
            fseek(file, 0, SEEK_END);
            size = static_cast<config::u32>( ftell(file) );
            fseek(file, 0, SEEK_SET);

            config::Char* buff = CONF_NEW config::Char[size+1];
            fread(buff, size, 1, file);
            buff[size] = '\0';
            fclose(file);

            xml::Parser parser;
            xml::Node* root = parser.parse(buff);

            if(root != NULL){

                for(u32 i=0; i<root->childNum(); ++i){
                    const xml::Node* node = root->getChild(i);
                    if(node->getName() == TagWidth){
                        readU32NoneZero(conf.width_, node->getText());

                    }else if(node->getName() == TagHeight){
                        readU32NoneZero(conf.height_, node->getText());

                    }else if(node->getName() == TagRate){
                        readU32NoneZero(conf.rate_, node->getText());

                    }else if(node->getName() == TagWindowed){
                        readS32(conf.windowed_, node->getText());

                    }else if(node->getName() == TagFormat){
                        u32 format;
                        readU32(format, node->getText());
                        if(format>=Buffer_Num){
                            conf.format_ = Buffer_X8R8G8B8;
                        }else{
                            conf.format_ = static_cast<BufferFormat>(format);
                        }
                    }
                } //for(u32 i=0;
            }//if(root != NULL)

            CONF_DELETE(root);
            CONF_DELETE_ARRAY(buff);
        }//if(file != NULL)
    }


    void writeConfig(const config::Config& conf, const Char* filepath)
    {

        xml::Node* root = CONF_NEW xml::Node;
        root->getName() = TagRoot;
        {
            xml::Node* nodeWidth = CONF_NEW xml::Node;
            nodeWidth->getName() = TagWidth;

            xml::Node* nodeHeight = CONF_NEW xml::Node;
            nodeHeight->getName() = TagHeight;

            xml::Node* nodeRate = CONF_NEW xml::Node;
            nodeRate->getName() = TagRate;

            xml::Node* nodeWindowed = CONF_NEW xml::Node;
            nodeWindowed->getName() = TagWindowed;

            xml::Node* nodeFormat = CONF_NEW xml::Node;
            nodeFormat->getName() = TagFormat;

            writeU32(nodeWidth->getText(), conf.width_);
            writeU32(nodeHeight->getText(), conf.height_);
            writeU32(nodeRate->getText(), conf.rate_);
            writeS32(nodeWindowed->getText(), conf.windowed_);
            writeU32(nodeFormat->getText(), conf.format_);


            root->addChild( nodeWidth );
            root->addChild( nodeHeight );
            root->addChild( nodeRate );
            root->addChild( nodeWindowed );
            root->addChild( nodeFormat );

        }

        if(filepath == NULL){
            filepath = DefaultFilePath;
        }
        xml::Writer writer;
        writer.write(root, filepath);
        CONF_DELETE(root);
    }
}

#ifdef __cplusplus
extern "C"
{
#endif
long getConfig(config::Config& conf, const config::Char* filepath, bool popup)
{
    config::Config initConf;
    initConf.width_ = 640;
    initConf.height_ = 480;
    initConf.rate_ = 60;
    initConf.windowed_ = true;
    initConf.format_ = config::Buffer_X8R8G8B8;

    readConfig(initConf, filepath);

    if(!popup){
        conf = initConf;
        return OK;
    }

    if(false == config::ConfigDialog::initialize(initConf)){
        MessageBox(NULL, "Cannot create Direct3D9", "Error", MB_OK);
            config::ConfigDialog::terminate();
            return Fail;
    }

    INT_PTR ret = config::ConfigDialog::create(g_hInstance, MAKEINTRESOURCE(IDD_CONFIG));

    if(IDOK == ret){
        conf = config::ConfigDialog::getInstance().getConfig();

        if(conf.width_ != initConf.width_
            || conf.height_ != initConf.height_
            || conf.rate_ != initConf.rate_
            || conf.windowed_ != initConf.windowed_
            || conf.format_ != initConf.format_)
        {
            writeConfig(conf, filepath);
        }
    }

    config::ConfigDialog::terminate();

    conf.format_ = toD3DFormat[conf.format_];
    return (ret == IDOK)? OK : Cancel;
}

#ifdef __cplusplus
}
#endif
