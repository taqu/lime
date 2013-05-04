#ifndef INC_LCORE_STACK_H__
#define INC_LCORE_STACK_H__
/**
@file Stack.h
@author t-sakai
@date 2010/11/08 create
*/
#include "lcore.h"

namespace lcore
{
    /**
    @brief スタック

    足りなくなれば、固定長で拡張するスタック
    */
    template<class T>
    class Stack
    {
    public:
        Stack();

        /**
        @param size ... バッファブロック１個のエントリ数。0より大きい

        initializeを行う
        */
        explicit Stack(u32 size);

        /// デストラクタ
        ~Stack();

        /**
        @brief 初期化
        @param size ... バッファブロック１個のエントリ数。0より大きい

        何度呼んでもよいが、その都度新しく確保するブロックエントリ数が変わる
        */
        void initialize(u32 size);

        /**
        @brief 終了

        これ以降initializeを再度呼ぶまで、popとpushを呼んではいけない。
        すでにpopされているエントリのポインタは無効になる。
        */
        void terminate();

        /// エントリ取得
        T* pop();

        /// エントリをスタックに戻す
        void push(T* ptr);

        /// スワップ
        void swap(Stack<T>& rhs)
        {
            lcore::swap(size_, rhs.size_);
            lcore::swap(count_, rhs.count_);
            lcore::swap(numBuffers_, rhs.numBuffers_);
            lcore::swap(top_, rhs.top_);
            lcore::swap(buffers_, rhs.buffers_);
        }

        /// pop数-push数
        u32 getCount() const{ return count_;}
    private:
        /// 単方向リストのエントリ。ポインタと内容のメモリ共有
        union Entry
        {
            T element_;
            Entry *next_;
        };

        /// バッファブロック内に、新規エントリリスト作成
        Entry* initialize(Entry* buffer);

        /// バッファブロック追加
        void incBuffers();

        /// ポインタがスタック管理内かチェック
        bool check(T* ptr);

        u32 size_; /// バッファブロック内エントリ数
        u32 count_; /// pop数-push数
        u32 numBuffers_; /// バッファブロック数
        Entry *top_; /// エントリ先頭
        Entry **buffers_; /// バッファブロック配列
    };


    //----------------------------------------------------
    template<class T>
    Stack<T>::Stack()
        :size_(0)
        ,count_(0)
        ,numBuffers_(0)
        ,top_(NULL)
        ,buffers_(NULL)
    {
    }

    //----------------------------------------------------
    template<class T>
    Stack<T>::Stack(u32 size)
        :size_(0)
        ,count_(0)
        ,numBuffers_(0)
        ,top_(NULL)
        ,buffers_(NULL)
    {
        initialize(size);
    }

    //----------------------------------------------------
    template<class T>
    Stack<T>::~Stack()
    {
        terminate();
    }

    //-----------------------------------------------------
    // 初期化
    template<class T>
    void Stack<T>::initialize(u32 size)
    {
        LASSERT(size > 0);
        LASSERT(NULL == buffers_);

        size_ = size;

        count_ = 0;
        numBuffers_ = 1;
        buffers_ = LIME_NEW Entry*[numBuffers_];
        buffers_[0] = LIME_NEW Entry[size_];
        top_ = initialize(buffers_[0]);
    }

    //-----------------------------------------------------
    // 終了
    template<class T>
    void Stack<T>::terminate()
    {
        for(u32 i=0; i<numBuffers_; ++i){
            LIME_DELETE_ARRAY(buffers_[i]);
        }
        LIME_DELETE_ARRAY(buffers_);

        numBuffers_ = 0;
        count_ = 0;
        top_ = NULL;
    }

    //-----------------------------------------------------
    // エントリ取得
    template<class T>
    T* Stack<T>::pop()
    {
        LASSERT(top_ != NULL);
        ++count_;

        Entry *next = top_->next_;
        if(next == NULL){
            incBuffers();
        }
        Entry *ret = top_;
        top_ = top_->next_;
        return (T*)ret;
    }

    //-----------------------------------------------------
    // エントリをスタックに戻す
    template<class T>
    void Stack<T>::push(T* ptr)
    {
        if(ptr == NULL){
            return;
        }

#if 0
        LASSERT(check(ptr));
#else
        if(check(ptr) == false){
            lcore::Log("Stack::push wrong adress");
            return;
        }
#endif
        --count_;
        Entry *entry = (Entry*)ptr;
        entry->next_ = top_;
        top_ = entry;
    }

    //-----------------------------------------------------
    // バッファブロック内に、新規エントリリスト作成
    template<class T>
    typename Stack<T>::Entry* Stack<T>::initialize(Entry* buffer)
    {
        u32 end = size_-1;
        for(u32 i=0; i<end; ++i){
            buffer[i].next_ = &(buffer[i+1]);
        }
        buffer[size_-1].next_ = NULL;
        return &(buffer[0]);
    }

    //-----------------------------------------------------
    // バッファブロック追加
    template<class T>
    void Stack<T>::incBuffers()
    {
        Entry **newBuffers = LIME_NEW Entry*[numBuffers_+1];
        for(u32 i=0; i<numBuffers_; ++i){
            newBuffers[i] = buffers_[i];
        }
        newBuffers[numBuffers_] = LIME_NEW Entry[size_];
        top_->next_ = initialize(newBuffers[numBuffers_]);
        ++numBuffers_;

        LIME_DELETE_ARRAY(buffers_);
        buffers_ = newBuffers;
    }

    //-----------------------------------------------------
    // ポインタチェック
    template<class T>
    bool Stack<T>::check(T* ptr)
    {
        bool ret = false;
        for(u32 i=0; i<numBuffers_; ++i){
            T *begin = &(buffers_[i]->element_);
            T *end = &((buffers_[i]+size_-1)->element_);

            ret |= (begin <= ptr && ptr<=end);
        }
        return ret;
    }
}
#endif //INC_LCORE_STACK_H__
