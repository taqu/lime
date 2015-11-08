#ifndef INC_LRENDER_SPECTRUM_H__
#define INC_LRENDER_SPECTRUM_H__
/**
@file Spectrum.h
@author t-sakai
@date 2015/09/23 create
*/
#include "../lrender.h"

namespace lrender
{
    //---------------------------------------------------------------------
    //---
    //--- SpectrumBase
    //---
    //---------------------------------------------------------------------
    template<class T, s32 Dimensions, class Derived>
    class SpectrumBase
    {
    public:
        static const s32 Dim = Dimensions;

        typedef SpectrumBase<T, Dimensions, Derived> this_type;
        typedef T value_type;
        typedef Derived derived_type;

        inline ~SpectrumBase();

        inline value_type operator[](s32 index) const;
        inline value_type& operator[](s32 index);

        this_type operator+(const this_type& rhs) const;
        this_type& operator+=(const this_type& rhs);

        this_type operator-(const this_type& rhs) const;
        this_type& operator-=(const this_type& rhs);

        this_type operator*(value_type x) const;

        template<class T, s32 Dimensions, class Derived>
        friend SpectrumBase<T, Dimensions, Derived> operator*(T x, const SpectrumBase<T, Dimensions, Derived>& rhs);

        this_type& operator*=(value_type x);

        this_type operator*(const this_type& rhs) const;
        this_type& operator*=(const this_type& rhs);

        this_type operator/(value_type x) const;

        template<class T, s32 Dimensions, class Derived>
        friend SpectrumBase<T, Dimensions, Derived> operator/(T x, const SpectrumBase<T, Dimensions, Derived>& rhs);

        this_type& operator/=(value_type x);

        this_type operator/(const this_type& rhs) const;
        this_type& operator/=(const this_type& rhs);

        bool operator==(const this_type& rhs) const;
        bool operator!=(const this_type& rhs) const;

        this_type& operator=(const this_type& rhs);
        void swap(this_type& rhs);

        void muladd(value_type x, const this_type& rhs);
        this_type absolute() const;

        f32 minimum() const;
        f32 maximum() const;
        inline bool isZero() const;
    protected:
        inline SpectrumBase();
        explicit SpectrumBase(value_type x);
        explicit SpectrumBase(value_type v[Dim]);
        SpectrumBase(const this_type& rhs);

        value_type values_[Dim];
    };

    template<class T, s32 Dimensions, class Derived>
    inline SpectrumBase<T, Dimensions, Derived>::SpectrumBase()
    {
    }

    template<class T, s32 Dimensions, class Derived>
    SpectrumBase<T, Dimensions, Derived>::SpectrumBase(value_type x)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] = x;
        }
    }

    template<class T, s32 Dimensions, class Derived>
    SpectrumBase<T, Dimensions, Derived>::SpectrumBase(value_type v[Dim])
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] = v[i];
        }
    }

    template<class T, s32 Dimensions, class Derived>
    SpectrumBase<T, Dimensions, Derived>::SpectrumBase(const this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] = rhs.values_[i];
        }
    }

    template<class T, s32 Dimensions, class Derived>
    inline SpectrumBase<T, Dimensions, Derived>::~SpectrumBase()
    {
    }

    template<class T, s32 Dimensions, class Derived>
    inline typename SpectrumBase<T, Dimensions, Derived>::value_type
        SpectrumBase<T, Dimensions, Derived>::operator[](s32 index) const
    {
        return values_[index];
    }

    template<class T, s32 Dimensions, class Derived>
    inline typename SpectrumBase<T, Dimensions, Derived>::value_type&
        SpectrumBase<T, Dimensions, Derived>::operator[](s32 index)
    {
        return values_[index];
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type
        SpectrumBase<T, Dimensions, Derived>::operator+(const this_type& rhs) const
    {
        this_type ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = values_[i] + rhs.values_[i];
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type&
        SpectrumBase<T, Dimensions, Derived>::operator+=(const this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] += rhs.values_[i];
        }
        return *this;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type
        SpectrumBase<T, Dimensions, Derived>::operator-(const this_type& rhs) const
    {
        this_type ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = values_[i] - rhs.values_[i];
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type&
        SpectrumBase<T, Dimensions, Derived>::operator-=(const this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] -= rhs.values_[i];
        }
        return *this;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type
        SpectrumBase<T, Dimensions, Derived>::operator*(value_type x) const
    {
        this_type ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = values_[i] * x;
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    SpectrumBase<T, Dimensions, Derived>
        operator*(T x, const SpectrumBase<T, Dimensions, Derived>& rhs)
    {
        SpectrumBase<T, Dimensions, Derived> ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = x * rhs.values_[i];
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type&
        SpectrumBase<T, Dimensions, Derived>::operator*=(value_type x)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] *= x;
        }
        return *this;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type
        SpectrumBase<T, Dimensions, Derived>::operator*(const this_type& rhs) const
    {
        this_type ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = values_[i] * rhs.values_[i];
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type&
        SpectrumBase<T, Dimensions, Derived>::operator*=(const this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] *= rhs.values_[i];
        }
        return *this;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type
        SpectrumBase<T, Dimensions, Derived>::operator/(value_type x) const
    {
        value_type inv = 1.0f/x;
        this_type ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = values_[i] * inv;
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    SpectrumBase<T, Dimensions, Derived>
        operator/(T x, const SpectrumBase<T, Dimensions, Derived>& rhs)
    {
        SpectrumBase<T, Dimensions, Derived> ret(x);
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] /= rhs.values_[i];
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type&
        SpectrumBase<T, Dimensions, Derived>::operator/=(value_type x)
    {
        value_type inv = 1.0f/x;
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] *= inv;
        }
        return *this;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type
        SpectrumBase<T, Dimensions, Derived>::operator/(const this_type& rhs) const
    {
        this_type ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = values_[i] / rhs.values_[i];
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type&
        SpectrumBase<T, Dimensions, Derived>::operator/=(const this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] /= rhs.values_[i];
        }
        return *this;
    }

    template<class T, s32 Dimensions, class Derived>
    bool SpectrumBase<T, Dimensions, Derived>::operator==(const this_type& rhs) const
    {
        for(s32 i=0; i<Dimensions; ++i){
            if(values_[i] != rhs.values_[i]){
                return false;
            }
        }
        return true;
    }

    template<class T, s32 Dimensions, class Derived>
    bool SpectrumBase<T, Dimensions, Derived>::operator!=(const this_type& rhs) const
    {
        for(s32 i=0; i<Dimensions; ++i){
            if(values_[i] != rhs.values_[i]){
                return true;
            }
        }
        return false;
    }

    template<class T, s32 Dimensions, class Derived>
    inline typename SpectrumBase<T, Dimensions, Derived>::this_type&
        SpectrumBase<T, Dimensions, Derived>::operator=(const this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] = rhs.values_[i];
        }
        return *this;
    }

    template<class T, s32 Dimensions, class Derived>
    void SpectrumBase<T, Dimensions, Derived>::swap(this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            lcore::swap(values_[i], rhs.values_[i]);
        }
    }

    template<class T, s32 Dimensions, class Derived>
    void SpectrumBase<T, Dimensions, Derived>::muladd(value_type x, const this_type& rhs)
    {
        for(s32 i=0; i<Dimensions; ++i){
            values_[i] += x * rhs.values_[i];
        }
    }

    template<class T, s32 Dimensions, class Derived>
    typename SpectrumBase<T, Dimensions, Derived>::this_type
        SpectrumBase<T, Dimensions, Derived>::absolute() const
    {
        this_type ret;
        for(s32 i=0; i<Dimensions; ++i){
            ret.values_[i] = lcore::absolute(values_[i]);
        }
        return ret;
    }

    template<class T, s32 Dimensions, class Derived>
    f32 SpectrumBase<T, Dimensions, Derived>::minimum() const
    {
        f32 v = values_[0];
        for(s32 i=1; i<Dimensions; ++i){
            if(values_[i]<v){
                v = values_[i];
            }
        }
        return v;
    }

    template<class T, s32 Dimensions, class Derived>
    f32 SpectrumBase<T, Dimensions, Derived>::maximum() const
    {
        f32 v = values_[0];
        for(s32 i=1; i<Dimensions; ++i){
            if(v<values_[i]){
                v = values_[i];
            }
        }
        return v;
    }

    template<class T, s32 Dimensions, class Derived>
    inline bool SpectrumBase<T, Dimensions, Derived>::isZero() const
    {
        for(s32 i=0; i<Dimensions; ++i){
            if(!lmath::isZero(values_[i])){
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------------------------
    //---
    //--- Color3
    //---
    //---------------------------------------------------------------------
    class Color3 : public SpectrumBase<f32, 3, Color3>
    {
    public:
        typedef SpectrumBase<f32, 3, Color3> parent_type;

        inline Color3();
        inline explicit Color3(f32 x);
        inline explicit Color3(f32 rgb[3]);
        inline Color3(f32 r, f32 g, f32 b);
        inline Color3(const Color3& rhs);
        inline Color3(const parent_type& rhs);
        inline ~Color3();

        f32 getLuminance() const;
        void getRGB(u8& r, u8& g, u8& b) const;
        void setRGB(u8 r, u8 g, u8 b);

        Color3 linearToStandard() const;
        Color3 standardToLinear() const;

        Color3 gamma(f32 x) const;

        inline Color3& operator=(const parent_type& rhs);

        static inline Color3 clear();
        static inline Color3 black();
        static inline Color3 white();
    };

    inline Color3::Color3()
    {
    }

    inline Color3::Color3(f32 x)
        :parent_type(x)
    {
    }

    inline Color3::Color3(f32 rgb[3])
        :parent_type(rgb)
    {
    }

    inline Color3::Color3(f32 r, f32 g, f32 b)
    {
        values_[0] = r;
        values_[1] = g;
        values_[2] = b;
    }

    inline Color3::Color3(const Color3& rhs)
        :parent_type(rhs)
    {
    }

    inline Color3::Color3(const parent_type& rhs)
        :parent_type(rhs)
    {
    }

    inline Color3::~Color3()
    {
    }

    inline Color3 Color3::clear()
    {
        return Color3(0.0f);
    }

    inline Color3 Color3::black()
    {
        return Color3(0.0f);
    }

    inline Color3 Color3::white()
    {
        return Color3(1.0f);
    }

    inline Color3& Color3::operator=(const parent_type& rhs)
    {
        parent_type::operator=(rhs);
        return *this;
    }

    //---------------------------------------------------------------------
    //---
    //--- Color4
    //---
    //---------------------------------------------------------------------
    class Color4 : public SpectrumBase<f32, 4, Color4>
    {
    public:
        typedef SpectrumBase<f32, 4, Color4> parent_type;

        inline Color4();
        inline explicit Color4(f32 x);
        inline explicit Color4(f32 rgba[4]);
        inline Color4(f32 r, f32 g, f32 b, f32 a);
        inline Color4(const Color4& rhs);
        inline Color4(const parent_type& rhs);
        inline ~Color4();

        f32 getLuminance() const;
        void getRGBA(u8& r, u8& g, u8& b, u8& a) const;
        void setRGBA(u8 r, u8 g, u8 b, u8 a);

        Color4 linearToStandard() const;
        Color4 standardToLinear() const;

        Color4 gamma(f32 x) const;

        inline Color4& operator=(const parent_type& rhs);

        static inline Color4 clear();
        static inline Color4 black();
        static inline Color4 white();
    };

    inline Color4::Color4()
    {
    }

    inline Color4::Color4(f32 x)
        :parent_type(x)
    {
    }

    inline Color4::Color4(f32 rgba[4])
        :parent_type(rgba)
    {
    }

    inline Color4::Color4(f32 r, f32 g, f32 b, f32 a)
    {
        values_[0] = r;
        values_[1] = g;
        values_[2] = b;
        values_[3] = a;
    }

    inline Color4::Color4(const Color4& rhs)
        :parent_type(rhs)
    {
    }

    inline Color4::Color4(const parent_type& rhs)
        :parent_type(rhs)
    {
    }

    inline Color4::~Color4()
    {
    }

    inline Color4 Color4::clear()
    {
        return Color4(0.0f);
    }

    inline Color4 Color4::black()
    {
        f32 rgba[4]={0.0f, 0.0f, 0.0f, 1.0f};
        return Color4(rgba);
    }

    inline Color4 Color4::white()
    {
        return Color4(1.0f);
    }

    inline Color4& Color4::operator=(const parent_type& rhs)
    {
        parent_type::operator=(rhs);
        return *this;
    }
}
#endif //INC_LRENDER_SPECTRUM_H__
