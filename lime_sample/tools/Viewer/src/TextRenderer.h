#ifndef INC_VIEWER_TEXTRENDERER_H__
#define INC_VIEWER_TEXTRENDERER_H__
/**
@file TextRenderer.h
@author t-sakai
@date 2011/04/01 create
*/
namespace viewer
{
    class TextRenderer
    {
    public:
        class PrimitiveBatch;

        TextRenderer();
        ~TextRenderer();

        void initialize(u32 numMaxCharacters, u32 charW, u32 charH, u32 numRows, u32 numCols);
        void terminate();

        void setViewSize(u32 numRows, u32 numCols);

        void draw();
        void clear();

        void print(s32 row, s32 col, const char* str);

        bool setTextureFromFile(const char* path);
        bool setTextureFromMemory(const s8* memory, u32 size);
    private:
        PrimitiveBatch* batch_;
    };
}
#endif //INC_VIEWER_TEXTRENDERER_H__
