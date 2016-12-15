#include <iostream>
#include <fstream>
#include <string>
#include "Font.h"

using namespace font;

namespace
{
    struct U32F32Union
    {
        union
        {
            u32 u_;
            f32 f_;
        };
    };

    u16 toBinary16Float(f32 f)
    {
        U32F32Union t;
        t.f_ = f;

        u16 sign = (t.u_>>16) & 0x8000U;
        s32 exponent = (t.u_>>23) & 0x00FFU;
        u32 fraction = t.u_ & 0x007FFFFFU;

        if(exponent == 0){
            return sign; //符号付き0

        }else if(exponent == 0xFFU){
            if(fraction == 0){
                return sign | 0x7C00U; //符号付きInf
            }else{
                return (fraction>>13) | 0x7C00U; //NaN
            }
        }else {
            exponent += (-127 + 15);
            if(exponent>=0x1F){ //オーバーフロー
                return sign | 0x7C00U;
            }else if(exponent<=0){ //アンダーフロー
                s32 shift = 14 - exponent;
                if(shift>24){ //表現できないほど小さい
                    return sign;
                }else{
                    fraction |= 0x800000U; //隠れた整数ビット足す
                    u16 frac = fraction >> shift;
                    if((fraction>>(shift-1)) & 0x01U){ //１ビット下位を丸める
                        frac += 1;
                    }
                    return sign | frac;
                }
            }
        }

        u16 ret = sign | ((exponent<<10) & 0x7C00U) | (fraction>>13);
        if((fraction>>12) & 0x01U){ //１ビット下位を丸める
            ret += 1;
        }
        return ret;
    }

    void getOutFilename(std::string& outfile, const std::string& infile)
    {
        std::string::size_type index = infile.rfind('.');
        
        if(index<infile.size()){
            outfile = infile.substr(0, index);
        }else{
            outfile = infile;
        }
        outfile += ".fpak";
    }

    void skipSpace(std::istream& input)
    {
        if(input.eof()){
            return;
        }

        int next = input.peek();
        while(next == ' ' || next == '\t'){
            input.get();
            if(input.eof()){
                break;
            }
            next = input.peek();
        }
    }

    void skipLine(std::istream& input)
    {
        for(;;){
            if(input.eof()){
                break;
            }

            int next = input.peek();
            if(next == '\n' || next == '\r'){
                do{
                    input.get();
                    if(input.eof()){
                        break;
                    }

                    next = input.peek();
                }while(next == '\n' || next == '\r');
                break;
            }
            input.get();
        }
    }

    void getWord(std::string& word, std::istream& input)
    {
        word.clear();

        if(input.eof()){
            return;
        }

        int next = input.peek();
        while(next != ' ' && next != '\t' && next != '\n' && next != '\r'){
            word.append(1, input.get());
            if(input.eof()){
                break;
            }
            next = input.peek();
        }
    }

    int parseParamInt(const std::string& word)
    {
        for(std::string::size_type i=0; i<word.length(); ++i){
            if(word[i] == '='){
                ++i;
                if(word.length()<=i){
                    break;
                }

                return atoi(word.c_str()+i);
            }
        }
        return 0;
    }

    unsigned int parseParamUInt(const std::string& word)
    {
        for(std::string::size_type i=0; i<word.length(); ++i){
            if(word[i] == '='){
                ++i;
                if(word.length()<=i){
                    break;
                }

                return (unsigned int)atol(word.c_str()+i);
            }
        }
        return 0;
    }

    void parseParamString(std::string& str, const std::string& word)
    {
        for(std::string::size_type i=0; i<word.length(); ++i){
            if(word[i] == '='){
                ++i;
                str.clear();
                int c = 0;
                for(std::string::size_type j=i; j<word.length(); ++j){
                    if('"' == word[j]){
                        if(2<=++c){
                            return;
                        }
                        continue;
                    }
                    str.append(1, word[j]);
                }
                break;
            }
        }
    }


    bool parseCommon(font::Header& header, std::istream& input)
    {
        skipSpace(input);

        std::string word;
        getWord(word, input);
        if(word != "common"){
            return false;
        }

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 10, "lineHeight")){
            return false;
        }
        header.lineHeight_ = parseParamInt(word);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 4, "base")){
            return false;
        }
        header.baseLine_ = parseParamInt(word);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 6, "scaleW")){
            return false;
        }
        header.width_ = parseParamInt(word);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 6, "scaleH")){
            return false;
        }
        header.height_ = parseParamInt(word);

        skipLine(input);
    }

    int parseChars(std::istream& input)
    {
        skipSpace(input);

        std::string word;
        getWord(word, input);
        if(word != "chars"){
            return 0;
        }

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 5, "count")){
            return 0;
        }
        int ret = parseParamInt(word);
        skipLine(input);
        return ret;
    }

    void parsePage(std::string& image, std::istream& input)
    {
        skipSpace(input);

        std::string word;
        getWord(word, input);
        if(word != "page"){
            return;
        }

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 2, "id")){
            return;
        }
        parseParamInt(word);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 4, "file")){
            return;
        }
        parseParamString(image, word);
        skipLine(input);
    }

    bool parseGlyph(font::Glyph& glyph, f32 invW, f32 invH, f32 halfPixelW, f32 halfPixelH, std::istream& input)
    {
        skipSpace(input);

        std::string word;
        getWord(word, input);
        if(word != "char"){
            return false;
        }

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 2, "id")){
            return false;
        }
        glyph.code_ = parseParamUInt(word);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 1, "x")){
            return false;
        }
        int x = parseParamInt(word);
        glyph.u0_ = toBinary16Float(invW * x + halfPixelW);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 1, "y")){
            return false;
        }
        int y = parseParamInt(word);
        glyph.v0_ = toBinary16Float(invH * y + halfPixelH);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 5, "width")){
            return false;
        }
        glyph.width_ = parseParamInt(word);
        glyph.u1_ = toBinary16Float(invW * (x + glyph.width_-1) + halfPixelW);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 6, "height")){
            return false;
        }
        glyph.height_ = parseParamInt(word);
        glyph.v1_ = toBinary16Float(invH * (y + glyph.height_-1) + halfPixelH);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 7, "xoffset")){
            return false;
        }
        glyph.xoffset_ = parseParamInt(word);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 7, "yoffset")){
            return false;
        }
        glyph.yoffset_ = parseParamInt(word);

        skipSpace(input);
        getWord(word, input);
        if(0 != word.compare(0, 8, "xadvance")){
            return false;
        }
        glyph.xadvance_ = parseParamInt(word);

        skipLine(input);
    }
}

int main(int argc, char** argv)
{
    if(argc<=1){
        std::cerr << "[Usage] " << argv[0] << " <input file>" << std::endl;
        return 0;
    }

    std::string infilename(argv[1]);
    std::ifstream infile(infilename.c_str(), std::ios::binary);
    if(!infile.is_open()){
        return 0;
    }

    std::string outfilename;
    getOutFilename(outfilename, infilename);
    std::string image;

    font::Header header;
    skipLine(infile);
    parseCommon(header, infile);
    parsePage(image, infile);
    header.numGlyphs_ = parseChars(infile);

    f32 invW = (1.0f/(header.width_ - 1));
    f32 invH = (1.0f/(header.height_ - 1));
    //f32 invW = (1.0f/(header.width_));
    //f32 invH = (1.0f/(header.height_));
    f32 halfPixelW = 0.5f * invW;
    f32 halfPixelH = 0.5f * invH;
    font::Glyph* glyphs = new font::Glyph[header.numGlyphs_];
    for(int i=0; i<header.numGlyphs_; ++i){
        memset(&glyphs[i], 0, sizeof(font::Glyph));
        parseGlyph(glyphs[i], invW, invH, halfPixelW, halfPixelH, infile);
    }
    infile.close();

    std::ifstream imageFile(image.c_str(), std::ios::binary);
    if(!imageFile.is_open()){
        delete[] glyphs;
        return 0;
    }

    imageFile.seekg(0, std::ios::end);
    unsigned int size = imageFile.tellg();
    imageFile.seekg(0, std::ios::beg);

    unsigned char* bytes = new unsigned char[size];
    imageFile.read((char*)bytes, sizeof(unsigned char)*size);
    imageFile.close();

    std::ofstream outFile(outfilename.c_str(), std::ios::binary);
    if(!outFile.is_open()){
        delete[] bytes;
        delete[] glyphs;
        return 0;
    }

    outFile.write((char*)&header, sizeof(font::Header));
    outFile.write((char*)glyphs, sizeof(font::Glyph)*header.numGlyphs_);
    outFile.write((char*)bytes, size);
    outFile.close();

    delete[] bytes;
    delete[] glyphs;
    return 0;
}
