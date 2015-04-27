#ifndef INC_LGRAPHICS_DX11_INPUTLAYOUTREF_H__
#define INC_LGRAPHICS_DX11_INPUTLAYOUTREF_H__
/**
@file InputLayoutRef.h
@author t-sakai
@date 2012/07/15 create
*/
#include "../../lgraphicscore.h"

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11InputLayout;

namespace lgraphics
{
    class ContextRef;

    class InputLayoutRef
    {
    public:
        typedef ID3D11InputLayout element_type;
        typedef ID3D11InputLayout* pointer_type;

        InputLayoutRef()
            :layout_(NULL)
        {}

        InputLayoutRef(const InputLayoutRef& rhs);

        ~InputLayoutRef()
        {
            destroy();
        }

        pointer_type get(){ return layout_;}

        void destroy();

        bool valid() const{ return (NULL != layout_);}

        InputLayoutRef& operator=(const InputLayoutRef& rhs)
        {
            InputLayoutRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        void swap(InputLayoutRef& rhs)
        {
            lcore::swap(layout_, rhs.layout_);
        }

        void attach(ContextRef& context);
    private:
        friend class InputLayout;

        explicit InputLayoutRef(pointer_type layout)
            :layout_(layout)
        {}

        pointer_type layout_;
    };

    class InputLayout
    {
    public:
        static InputLayoutRef create(
            D3D11_INPUT_ELEMENT_DESC* elements,
            u32 numElements,
            const void* data,
            u32 size);
    };
}
#endif //INC_LGRAPHICS_DX11_INPUTLAYOUTREF_H__
