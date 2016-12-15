#ifndef INC_LGRAPHICS_TRANSIENTBUFFER_H__
#define INC_LGRAPHICS_TRANSIENTBUFFER_H__
/**
@file TransientBuffer.h
@author t-sakai
@date 2014/10/16 create
*/
#include <lcore/ObjectPool.h>
#include <lcore/RedBlackTree.h>
#include "BufferCreator.h"

namespace lgfx
{
    class ContextRef;

    template<class Buffer, class Creator>
    class TransientBuffer
    {
    public:
        typedef TransientBuffer<Buffer, Creator> this_type;
        typedef Buffer buffer_type;
        typedef Creator creator_type;

        typedef lcore::uintptr_t uintptr_t;
        typedef u32 size_type;
        typedef u32 flag_type;
        typedef u8* pointer_type;
        typedef u32 index_type;

        static const u32 MallocAlign = (1U<<7);
        static const u32 ChunkAlignMask = (MallocAlign - 1U);

        static const u32 NumSmallBins = 15U;
        static const u32 NumTreeBins = 32U;
        static const u32 SmallBinShift = 7U;
        static const u32 LargeBinShift = 10U;

        static const u32 MinLargeBinSize = 0x01U<<LargeBinShift;

        static const u32 PageSize = 16*1024;

        struct Chunk
        {
            static const flag_type Flag_InUse = 0x01U;
            static const flag_type Flag_InGPUUse = 0x02U;
            static const flag_type Flag_All = (Flag_InUse|Flag_InGPUUse);

            inline Chunk()
                :prev_(NULL)
                ,next_(NULL)
                ,offset_(0)
                ,size_(0)
                ,event_(0)
                ,refCount_(0)
            {
                reset();
            }

            inline ~Chunk()
            {
            }

            inline size_type getSize() const
            {
                return size_ & ~Flag_All;
            }

            inline void setSize(size_type size)
            {
                size_ = size;
            }

            inline void setSizeWithFlag(size_type size)
            {
                size_ = size | (size_&Flag_All);
            }

            inline void clearFlags()
            {
                size_ &= ~Flag_All;
            }

            inline bool isInUse() const
            {
                return 0 != (size_ & Flag_InUse);
            }

            inline void setInUse()
            {
                size_ |= Flag_InUse;
            }

            inline void resetInUse()
            {
                size_ &= ~Flag_InUse;
            }

            inline bool isInGPUUse() const
            {
                return 0 != (size_ & Flag_InGPUUse);
            }

            inline void setInGPUUse()
            {
                size_ |= Flag_InGPUUse;
            }

            inline void resetInGPUUse()
            {
                size_ &= ~Flag_InGPUUse;
            }

            inline bool checkFlags() const
            {
                return 0 != (size_ & Flag_All);
            }

            inline void reset()
            {
                binPrev_ = binNext_ = this;
            }

            inline void unlink()
            {
                binNext_->binPrev_ = binPrev_;
                binPrev_->binNext_ = binNext_;
                binPrev_ = binNext_ = this;
            }

            inline void link(Chunk* bin)
            {
                binPrev_ = bin->binPrev_;
                binNext_ = bin;
                binPrev_->binNext_ = bin->binPrev_ = this;
            }

            inline void push_front(Chunk* bin)
            {
                bin->link(binNext_);
            }

            inline bool empty() const
            {
                return (this == binNext_);
            }

            inline void addRef()
            {
                ++refCount_;
            }

            inline void release()
            {
                --refCount_;
            }

            inline bool valid() const
            {
                return buffer_.valid();
            }

            Chunk* prev_;
            Chunk* next_;
            Chunk* binPrev_;
            Chunk* binNext_;

            size_type offset_;
            size_type size_;
            s32 event_;
            s32 refCount_;
            buffer_type buffer_;
        };
        typedef Chunk chunk_type;

        class AllocatedChunk
        {
        public:
            AllocatedChunk()
                :parent_(NULL)
                ,chunk_(NULL)
            {}

            AllocatedChunk(const AllocatedChunk& rhs);
            ~AllocatedChunk();

            inline u32 getSize() const;

            inline bool valid() const;

            bool map(lgfx::ContextRef& context, u32 subresource, lgfx::MappedSubresourceTransientBuffer& mapped);
            void unmap(lgfx::ContextRef& context, u32 subresource);

            void attach(lgfx::ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes);
            void attach(lgfx::ContextRef& context, lgfx::DataFormat format, u32 offsetInBytes);

            AllocatedChunk& operator=(const AllocatedChunk& rhs)
            {
                AllocatedChunk(rhs).swap(*this);
                return *this;
            }

            void swap(AllocatedChunk& rhs);
        private:
            friend class TransientBuffer;
            AllocatedChunk(this_type* parent, chunk_type* chunk);

            this_type* parent_;
            chunk_type* chunk_;
        };
        typedef AllocatedChunk allocated_chunk_type;

        TransientBuffer();
        ~TransientBuffer();

        void initialize(FrameSyncQuery* frameSyncQuery);
        void terminate();

        void begin();

        AllocatedChunk allocate(size_type size);
        void deallocate(AllocatedChunk& allocated);

    private:
        TransientBuffer(const TransientBuffer&);
        TransientBuffer& operator=(const TransientBuffer&);

        inline static index_type getSmallIndex(size_type size)
        {
            return (size>>SmallBinShift);
        }

        inline static size_type getSmallSize(index_type index)
        {
            return (index << SmallBinShift);
        }

        inline static size_type paddingRequest(size_type size)
        {
            return (size + ChunkAlignMask) & ~ChunkAlignMask;
        }

        typedef lcore::ObjectPool<Chunk, lcore::DefaultAllocator> ChunkPool;

        struct RedBlackTreeAllocator
        {
            typedef lcore::RedBlackTreeNodeBase<Chunk*> node_type;
            typedef lcore::ObjectPool<node_type, lcore::DefaultAllocator> NodePool;

            RedBlackTreeAllocator(node_type* ptr)
                :nullptr_(ptr)
            {
            }

            node_type* nullptr_;

            inline node_type* create(Chunk*const value)
            {
                node_type* node = nodePool_.pop();
                node->left_ = nullptr_;
                node->right_ = nullptr_;
                node->setRed();
                node->value_ = value;
                return node;
            }

            inline void destroy(node_type*& node)
            {
                nodePool_.push(node);
                node = nullptr_;
            }

            void initialize()
            {
                nodePool_.initialize(4*1024, NodePool::initializer_type());
            }

            void terminate()
            {
                nodePool_.terminate();
            }

            NodePool nodePool_;
        };

        struct RedBlackTreeComparator
        {
            /**
            v0<v1 : <0
            v0==v1 : 0
            v0>v1 : >0
            */
            s32 operator()(Chunk*& v0, Chunk* const v1) const
            {
                u32 s0 = v0->getSize();
                u32 s1 = v1->getSize();
                return (s0==s1)? 0 : ((s0<s1)? -1 : 1);
            }
        };

        friend class RedBlackTreeTraversal;
        struct RedBlackTreeTraversal
        {
            RedBlackTreeTraversal(TransientBuffer* parent)
                :parent_(parent)
            {}

            void operator()(Chunk*& v0);

            TransientBuffer* parent_;
        };

        typedef lcore::RedBlackTree<Chunk*, RedBlackTreeAllocator, RedBlackTreeComparator, RedBlackTreeTraversal> ChunkRedBlackTree;

        /**
        @brief Œã”ösizeƒoƒCƒg‚ð•ªŠ„
        @return •ªŠ„‚µ‚½Chunk
        */
        Chunk* splitBack(Chunk* chunk, size_type size);

        Chunk* allocateSmall(size_type size);
        Chunk* allocateLarge(size_type size);
        Chunk* allocatePage(size_type size);
        void storeFreeChunk(Chunk* chunk);

        void combine(Chunk* chunk);
        void unlink(Chunk* chunk);
        void unlinkFromTree(Chunk* chunk);

        void pushChunk(Chunk* chunk);

        FrameSyncQuery* frameSyncQuery_;
        Chunk smallBins_[NumSmallBins+1];
        ChunkPool chunkPool_;
        ChunkRedBlackTree chunkTree_;

        Chunk allocated_;
        Chunk inGPUUse_;
    };

    template<class Buffer, class Creator>
    TransientBuffer<Buffer, Creator>::AllocatedChunk::AllocatedChunk(const AllocatedChunk& rhs)
        :parent_(rhs.parent_)
        ,chunk_(rhs.chunk_)
    {
        if(chunk_){
            chunk_->addRef();
        }
    }

    template<class Buffer, class Creator>
    TransientBuffer<Buffer, Creator>::AllocatedChunk::AllocatedChunk(this_type* parent, chunk_type* chunk)
        :parent_(parent)
        ,chunk_(chunk)
    {
        if(chunk_){
            chunk_->addRef();
        }
    }

    template<class Buffer, class Creator>
    TransientBuffer<Buffer, Creator>::AllocatedChunk::~AllocatedChunk()
    {
        if(chunk_){
            chunk_->release();
            if(0 == chunk_->refCount_){
                parent_->deallocate(*this);
            }
        }
    }

    template<class Buffer, class Creator>
    inline u32 TransientBuffer<Buffer, Creator>::AllocatedChunk::getSize() const
    {
        return chunk_->getSize();
    }

    template<class Buffer, class Creator>
    inline bool TransientBuffer<Buffer, Creator>::AllocatedChunk::valid() const
    {
        return chunk_->valid();
    }

    template<class Buffer, class Creator>
    bool TransientBuffer<Buffer, Creator>::AllocatedChunk::map(lgfx::ContextRef& context, u32 subresource, lgfx::MappedSubresourceTransientBuffer& mapped)
    {
        LASSERT(NULL != chunk_);
        if(chunk_->size_ <= 0){
            return false;
        }
        lgfx::MappedSubresource m;
        if(chunk_->buffer_.map(context, subresource, lgfx::MapType_WriteNoOverwrite, m)){
            mapped.offsetData_ = ((u8*)m.data_ + chunk_->offset_);
            mapped.offsetInBytes_ = chunk_->offset_;
            mapped.sizeInBytes_ = chunk_->size_;
            return true;
        }
        return false;
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::AllocatedChunk::unmap(lgfx::ContextRef& context, u32 subresource)
    {
        LASSERT(NULL != chunk_);
        chunk_->buffer_.unmap(context, subresource);
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::AllocatedChunk::attach(lgfx::ContextRef& context, u32 startSlot, u32 stride, u32 offsetInBytes)
    {
        LASSERT(NULL != chunk_);
        context.setVertexBuffers(startSlot, 1, (chunk_->buffer_), &stride, &offsetInBytes);
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::AllocatedChunk::attach(lgfx::ContextRef& context, lgfx::DataFormat format, u32 offsetInBytes)
    {
        LASSERT(NULL != chunk_);
        context.setIndexBuffer(chunk_->buffer_.get(), format, offsetInBytes);
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::AllocatedChunk::swap(AllocatedChunk& rhs)
    {
        lcore::swap(parent_, rhs.parent_);
        lcore::swap(chunk_, rhs.chunk_);
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::RedBlackTreeTraversal::operator()(Chunk*& v0)
    {
        Chunk* node = v0;
        Chunk* end = v0;
        do{
            Chunk* next = node->binNext_;
            parent_->pushChunk(node);
            node = next;
        }while(end != node);
    }

    template<class Buffer, class Creator>
    TransientBuffer<Buffer, Creator>::TransientBuffer()
        :frameSyncQuery_(NULL)
    {
    }

    template<class Buffer, class Creator>
    TransientBuffer<Buffer, Creator>::~TransientBuffer()
    {
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::initialize(FrameSyncQuery* frameSyncQuery)
    {
        LASSERT(NULL != frameSyncQuery);
        frameSyncQuery_ = frameSyncQuery;
        chunkPool_.initialize(16*1024, ChunkPool::initializer_type());
        chunkTree_.initAllocator();
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::terminate()
    {
        //u32 size = 0;
        for(s32 i=0; i<=NumSmallBins; ++i){
            Chunk* chunk = smallBins_[i].binNext_;
            while(&smallBins_[i] != chunk){
                //size += chunk->getSize();
                Chunk* next = chunk->binNext_;
                pushChunk(chunk);
                chunk = next;
            }
            smallBins_[i].reset();
        }

        {
            RedBlackTreeTraversal traversal(this);
            chunkTree_.traverse(traversal);
            chunkTree_.clear();
        }

        {
            Chunk* chunk = inGPUUse_.binNext_;
            while(&inGPUUse_ != chunk){
                //size += chunk->getSize();
                Chunk* next = chunk->binNext_;
                pushChunk(chunk);
                chunk = next;
            }
        }

        {
            Chunk* chunk = allocated_.binNext_;
            while(&allocated_ != chunk){
                //size += chunk->getSize();
                Chunk* next = chunk->binNext_;
                pushChunk(chunk);
                chunk = next;
            }
        }

        chunkTree_.termAllocator();
        chunkPool_.terminate();
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::begin()
    {
        Chunk* chunk = inGPUUse_.binNext_;
        while(&inGPUUse_ != chunk){
            Chunk* next = chunk->binNext_;

            if(false == frameSyncQuery_->checkInGPUUse(chunk->event_)){
                chunk->resetInGPUUse();
                chunk->unlink();
                combine(chunk);
                storeFreeChunk(chunk);
            }

            chunk = next;
        }
    }

    template<class Buffer, class Creator>
    typename TransientBuffer<Buffer, Creator>::allocated_chunk_type
        TransientBuffer<Buffer, Creator>::allocate(size_type size)
    {
        size = paddingRequest(size);

        Chunk* chunk;
        if(size<MinLargeBinSize){
            chunk = allocateSmall(size);
        }else{
            chunk = allocateLarge(size);
        }
        chunk->setInUse();

        lscene::FrameSyncQueryType& frameSync = SystemInstance::getInstance().getFrameSync();
        chunk->event_ = frameSync.getCurrentFrame();
        chunk->refCount_ = 0;

        allocated_.push_front(chunk);
        return AllocatedChunk(this, chunk);
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::deallocate(allocated_chunk_type& allocated)
    {
        LASSERT(NULL != allocated.chunk_);
        Chunk* chunk = allocated.chunk_;
        chunk->resetInUse();
        chunk->setInGPUUse();
        chunk->unlink();
        allocated.chunk_ = NULL;
        inGPUUse_.push_front(chunk);

        //combine(chunk);
        //storeFreeChunk(chunk);
    }

    template<class Buffer, class Creator>
    typename TransientBuffer<Buffer, Creator>::chunk_type*
        TransientBuffer<Buffer, Creator>::splitBack(Chunk* chunk, size_type size)
    {
        LASSERT(NULL != chunk);
        LASSERT(size<=chunk->getSize());

        Chunk* newChunk = chunkPool_.pop();
        newChunk->prev_ = chunk;
        newChunk->next_ = chunk->next_;
        if(newChunk->next_){
            newChunk->next_->prev_ = newChunk;
        }
        newChunk->setSize(size);
        newChunk->buffer_ = chunk->buffer_;
        newChunk->reset();

        size_type chunkSize = chunk->getSize() - size;

        chunk->next_ = newChunk;
        chunk->setSize(chunkSize);

        newChunk->offset_ = chunk->offset_ + chunkSize;

        LASSERT((chunk->getSize() + chunk->offset_) == newChunk->offset_);
        return newChunk;
    }

    template<class Buffer, class Creator>
    typename TransientBuffer<Buffer, Creator>::chunk_type*
        TransientBuffer<Buffer, Creator>::allocateSmall(size_type size)
    {
        index_type index = getSmallIndex(size);
        for(;index<=NumSmallBins; ++index){
            if(smallBins_[index].empty()){
                continue;
            }
            Chunk* chunk = smallBins_[index].binNext_;
            chunk->unlink();
            size_type chunkSize = chunk->getSize();
            if(size == chunkSize){
                return chunk;
            }
            size_type restSize = chunkSize - size;
            Chunk* rest = splitBack(chunk, restSize);
            index = getSmallIndex(restSize);
            smallBins_[index].push_front(rest);
            return chunk;
        }

        return allocateLarge(size);
    }

    template<class Buffer, class Creator>
    typename TransientBuffer<Buffer, Creator>::chunk_type*
        TransientBuffer<Buffer, Creator>::allocateLarge(size_type size)
    {
        Chunk query;
        query.setSize(size);

        Chunk** ppChunk = chunkTree_.findGreaterEqual(&query);
        if(NULL != ppChunk){

            Chunk* chunk = *ppChunk;
            unlinkFromTree(chunk);

            size_type chunkSize = chunk->getSize();
            if(chunkSize==size){
                return chunk;
            }else{
                Chunk* newChunk = splitBack(chunk, size);
                storeFreeChunk(chunk);
                return newChunk;
            }

        }else{
            if(size<PageSize){
                Chunk* chunk = allocatePage(PageSize);
                Chunk* newChunk = splitBack(chunk, size);
                storeFreeChunk(chunk);
                return newChunk;
            }else{
                Chunk* chunk = allocatePage(size);
                return chunk;
            }
        }
    }

    template<class Buffer, class Creator>
    typename TransientBuffer<Buffer, Creator>::chunk_type*
        TransientBuffer<Buffer, Creator>::allocatePage(size_type size)
    {
        Chunk* chunk = chunkPool_.pop();
        chunk->prev_ = chunk->next_ = NULL;
        chunk->offset_ = 0;
        chunk->setSize(size);
        chunk->reset();
        chunk->buffer_ = creator_type::create(size);

        return chunk;
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::storeFreeChunk(Chunk* chunk)
    {
        size_type chunkSize = chunk->getSize();
        if(chunkSize<MinLargeBinSize){
            index_type index = getSmallIndex(chunkSize);
            smallBins_[index].push_front(chunk);
        }else{
            Chunk** ppChunk = chunkTree_.find(chunk);
            if(NULL == ppChunk){
                chunkTree_.insert(chunk);
            }else{
                (*ppChunk)->push_front(chunk);
            }
        }
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::combine(Chunk* chunk)
    {
        LASSERT(NULL != chunk);

        if(NULL != chunk->prev_){
            Chunk* target = chunk->prev_;
            if(!target->checkFlags()){
                unlink(target);
                LASSERT((target->getSize() + target->offset_) == chunk->offset_);

                chunk->prev_ = target->prev_;
                chunk->offset_ = target->offset_;
                chunk->setSize(chunk->getSize() + target->getSize());
                if(chunk->prev_){
                    chunk->prev_->next_ = chunk;
                }
                pushChunk(target);
            }
        }

        if(NULL != chunk->next_){
            Chunk* target = chunk->next_;
            if(!target->checkFlags()){
                unlink(target);
                LASSERT((chunk->getSize() + chunk->offset_) == target->offset_);
                chunk->next_ = target->next_;
                chunk->setSize(chunk->getSize() + target->getSize());
                if(chunk->next_){
                    chunk->next_->prev_ = chunk;
                }
                pushChunk(target);
            }
        }
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::unlink(Chunk* chunk)
    {
        LASSERT(NULL != chunk);

        size_type chunkSize = chunk->getSize();
        if(chunkSize<MinLargeBinSize){
            chunk->unlink();
        }else{
            unlinkFromTree(chunk);
        }
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::unlinkFromTree(Chunk* chunk)
    {
        if(chunk->empty()){
            chunkTree_.remove(chunk);
            chunk->unlink();
        }else{
            chunk->unlink();
        }
    }

    template<class Buffer, class Creator>
    void TransientBuffer<Buffer, Creator>::pushChunk(Chunk* chunk)
    {
        chunk->buffer_.destroy();
        chunkPool_.push(chunk);
    }
}

#endif //INC_LGRAPHICS_TRANSIENTBUFFER_H__
