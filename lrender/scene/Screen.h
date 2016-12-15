#ifndef INC_LRENDER_SCREEN_H__
#define INC_LRENDER_SCREEN_H__
/**
@file Screen.h
@author t-sakai
@date 2013/05/09 create
*/
#include "../lrender.h"

namespace lrender
{
    class Color3;
    class Filter;

    class Screen
    {
    public:
        Screen();
        Screen(s32 width, s32 height, Filter* filter);
        ~Screen();

        inline bool valid() const;
        inline s32 getWidth() const;
        inline s32 getHeight() const;
        inline f32 getInvWidth() const;
        inline f32 getInvHeight() const;

        void clear(const Color3& rgb);
        void clearWeights();
        const Color3& get(s32 x, s32 y) const;
        void set(s32 x, s32 y, const Color3& rgb);
        void setFiltered(f32 x, f32 y, const Color3& rgb);
        void setFilter(Filter* filter);
        void divWeights();

        void linearToStandardRGB();
        void toneMapping(f32 scale);

        void swap(Screen& rhs);

        bool savePPM(const char* filename) const;
    private:
        Screen(const Screen&);
        Screen& operator=(const Screen&);

        Filter* filter_;
        f32* filterWeightsX_;
        f32* filterWeightsY_;
        s32 width_;
        s32 height_;
        f32 invWidth_;
        f32 invHeight_;
        Color3* pixels_;
        f32* weights_;
    };

    inline bool Screen::valid() const
    {
        return NULL != pixels_;
    }

    inline s32 Screen::getWidth() const
    {
        return width_;
    }

    inline s32 Screen::getHeight() const
    {
        return height_;
    }

    inline f32 Screen::getInvWidth() const
    {
        return invWidth_;
    }

    inline f32 Screen::getInvHeight() const
    {
        return invHeight_;
    }
}
#endif //INC_LRENDER_SCREEN_H__
