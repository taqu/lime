#ifndef INC_LGRAPHICS_ES2_GRAPHICSDEVICEREF_H__
#define INC_LGRAPHICS_ES2_GRAPHICSDEVICEREF_H__
/**
@file GraphicsDeviceRef.h
@author t-sakai
@date 2010/05/29 create
@date 2010/12/12 modify for gles2
*/
#include "../../lgraphicsAPIInclude.h"
#include "../../lgraphicscore.h"
#include "Enumerations.h"
#include <lcore/Stack.h>

namespace lmath
{
    class Vector3;
    class Vector4;
    class Matrix44;
}

namespace lgraphics
{
    struct InitParam;

    struct Descriptor
    {
        void addRef()
        {
            ++counter_;
        }

        void release();
        u32 counter_;
        u32 id_;
    };


    class GraphicsDeviceRef
    {
    public:
        GraphicsDeviceRef();
        ~GraphicsDeviceRef();

        bool initialize(const InitParam& initParam);
        void terminate();

        inline void beginScene();
        inline void endScene();

        inline void clear(GLbitfield target);
        void setClearColor(u32 color);
        void setClearDepth(f32 depth);
        void setClearStencil(u32 stencil);

        inline void present();

        Descriptor* allocate()
        {
            Descriptor *ret= descAllocator_.pop();
            ret->counter_ = 0;
            ret->id_ = 0;
            return ret;
        }
        void free(Descriptor* ptr){ descAllocator_.push(ptr);}

        // ビューポート設定
        inline void setViewport(s32 left, s32 top, s32 width, s32 height);

        // Draw系
        //-----------------------------------------------------------------------------
        inline void draw(PrimitiveType type, u32 numVertices, u32 offsetVertex) const;
        inline void drawIndexed(PrimitiveType type, u32 numIndices, u32 offsetIndex) const;

        //inline void drawUP(PrimitiveType type, u32 primCount, const void* vertices, u32 stride);
        //inline void drawIndexedUP(
        //    PrimitiveType type,
        //    u32 minIndex,
        //    u32 vertexNum,
        //    u32 primCount,
        //    const void* indices,
        //    BufferFormat indexFormat,
        //    const void* vertices,
        //    u32 stride);



        void checkError();

        inline void setAlphaTest(bool enable);
        inline bool getAlphaTest() const;

        inline void setAlphaTestRef(s32 ref);
        inline s32 getAlphaTestRef() const;

        inline void setAlphaTestFunc(CmpFunc func);
        inline CmpFunc getAlphaTestFunc() const;
    private:
        friend struct Descriptor;

        typedef lcore::Stack<Descriptor> DescriptorAllocator;

        void enableState();

        //他のGLバージョンと合わせるためのステート
        //----------------------------------------------------
        /// ステートフラグ
        enum StateFlag
        {
            Flag_AlphaTest = 0x01U << 0,
        };

        static const s32 DefaultAlphaTestRef = 128;
        static const CmpFunc DefaultAlphaTestFunc = Cmp_Less;


        u32 state_;

        s32 alphaTestRef_; //アルファテスト参照値
        CmpFunc alphaTestFunc_; //アルファテスト比較関数

#if defined(ANDROID)
#else
        EGLDisplay display_;
        EGLSurface surface_;
        EGLContext context_;
        NativeWindowType window_;
#endif

        DescriptorAllocator descAllocator_;

        //u32 maxMultiTextures_; /// マルチテクスチャ最大枚数
    };



    inline void GraphicsDeviceRef::beginScene()
    {
    }

    inline void GraphicsDeviceRef::endScene()
    {
    }


    inline void GraphicsDeviceRef::clear(GLbitfield target)
    {
        glClear(target);
    }

    inline void GraphicsDeviceRef::present()
    {
#if defined(ANDROID)
        glFlush();
#else
        glFlush();
        eglSwapBuffers(display_, surface_);
#endif
    }

    // ビューポート設定
    inline void GraphicsDeviceRef::setViewport(s32 left, s32 top, s32 width, s32 height)
    {
        glViewport(left, top, width, height);
    }

    // Draw系
    //------------------------------------------------------------------------------------------
    inline void GraphicsDeviceRef::draw(PrimitiveType type, u32 numVertices, u32 offsetVertex) const
    {
        glDrawArrays(type, offsetVertex, numVertices);
    }

    inline void GraphicsDeviceRef::drawIndexed(PrimitiveType type, u32 numIndices, u32 offsetIndex) const
    {
        LASSERT(glDrawElements != NULL);
        offsetIndex <<= 1; //バイト単位のオフセットに修正
        glDrawElements(type, numIndices, GL_UNSIGNED_SHORT, LBUFFER_OFFSET(offsetIndex));
    }


    inline void GraphicsDeviceRef::setAlphaTest(bool enable)
    {
        if(enable){
            state_ |= Flag_AlphaTest;
        }else{
            state_ &= ~Flag_AlphaTest;
        }
    }

    inline bool GraphicsDeviceRef::getAlphaTest() const
    {
        return ((state_ & Flag_AlphaTest) != 0);
    }

    inline void GraphicsDeviceRef::setAlphaTestRef(s32 ref)
    {
        alphaTestRef_ = ref;
    }

    inline s32 GraphicsDeviceRef::getAlphaTestRef() const
    {
        return alphaTestRef_;
    }

    inline void GraphicsDeviceRef::setAlphaTestFunc(CmpFunc func)
    {
        alphaTestFunc_ = func;
    }

    inline CmpFunc GraphicsDeviceRef::getAlphaTestFunc() const
    {
        return alphaTestFunc_;
    }
}
#endif //INC_LGRAPHICS_ES2_GRAPHICSDEVICEREF_H__
