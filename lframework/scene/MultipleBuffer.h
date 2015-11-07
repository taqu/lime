#ifndef INC_LSCENE_MULTIPLEBUFFER_H__
#define INC_LSCENE_MULTIPLEBUFFER_H__
/**
@file MultipleBuffer.h
@author t-sakai
@date 2015/05/27 create
*/
namespace lscene
{
    //--------------------------------------------------
    //---
    //--- MultipleBuffer
    //---
    //--------------------------------------------------
    template<class Buffer, class View, class Creator>
    class MultipleBuffer
    {
    public:
        static const s32 NumMaxBuffers = 4;
        typedef Buffer buffer_type;
        typedef View view_type;
        typedef Creator creator_type;

        MultipleBuffer();
        ~MultipleBuffer();

        void initialize(s32 numMaxBuffers, s32 totalSize);
        void terminate();

        void begin();
        void end();

        inline buffer_type& get();
        inline view_type& getView();
    private:
        s32 numMaxBuffers_;
        s32 current_;
        view_type view_[NumMaxBuffers];
        buffer_type buffer_[NumMaxBuffers];
    };

    template<class Buffer, class View, class Creator>
    MultipleBuffer<Buffer, View, Creator>::MultipleBuffer()
        :numMaxBuffers_(0)
        ,current_(0)
    {
    }

    template<class Buffer, class View, class Creator>
    MultipleBuffer<Buffer, View, Creator>::~MultipleBuffer()
    {
    }

    template<class Buffer, class View, class Creator>
    void MultipleBuffer<Buffer, View, Creator>::initialize(s32 numMaxBuffers, s32 totalSize)
    {
        LASSERT(0<numMaxBuffers && numMaxBuffers<=NumMaxBuffers);
        numMaxBuffers_ = numMaxBuffers;
        for(s32 i=0; i<numMaxBuffers_; ++i){
            buffer_[i] = creator_type::create(totalSize);
            view_[i] = creator_type::createView(buffer_[i], totalSize);
        }
    }

    template<class Buffer, class View, class Creator>
    void MultipleBuffer<Buffer, View, Creator>::terminate()
    {
        for(s32 i=numMaxBuffers_-1; 0<=i; --i){
            view_[i].destroy();
            buffer_[i].destroy();
        }
    }

    template<class Buffer, class View, class Creator>
    void MultipleBuffer<Buffer, View, Creator>::begin()
    {
        if(numMaxBuffers_<=++current_){
            current_ = 0;
        }
    }

    template<class Buffer, class View, class Creator>
    void MultipleBuffer<Buffer, View, Creator>::end()
    {
    }

    template<class Buffer, class View, class Creator>
    inline typename MultipleBuffer<Buffer, View, Creator>::buffer_type& MultipleBuffer<Buffer, View, Creator>::get()
    {
        return buffer_[current_];
    }

    template<class Buffer, class View, class Creator>
    inline typename MultipleBuffer<Buffer, View, Creator>::view_type& MultipleBuffer<Buffer, View, Creator>::getView()
    {
        return view_[current_];
    }
}
#endif //INC_LSCENE_MULTIPLEBUFFER_H__
