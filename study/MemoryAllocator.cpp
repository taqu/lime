/**
@file MemoryAllocator.cpp
@author t-sakai
@date 2014/03/24 create
*/
#include "MemoryAllocator.h"

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace lcore
{
    //-------------------------------------------------
    //---
    //--- MemoryAllocator::TreeChunk
    //---
    //-------------------------------------------------
    MemoryAllocator::TreeChunk::TreeChunk()
        :prev_(this)
        ,next_(this)
        ,parent_(NULL)
    {
        child_[0] = child_[1] = NULL;
    }

    inline void MemoryAllocator::TreeChunk::resetAllLink()
    {
        prev_ = next_ = this;
        parent_ = NULL;
        child_[0] = child_[1] = NULL;
    }


    void MemoryAllocator::TreeChunk::reset()
    {
        prev_ = next_ = this;
    }

    void MemoryAllocator::TreeChunk::unlink()
    {
        next_->prev_ = prev_;
        prev_->next_ = next_;
        prev_ = next_ = this;
    }

    void MemoryAllocator::TreeChunk::link(TreeChunk* treeChunk)
    {
        prev_ = treeChunk->prev_;
        next_ = treeChunk;
        prev_->next_ = treeChunk->prev_ = this;
    }

    void MemoryAllocator::TreeChunk::unlinkFromParent()
    {
        LASSERT(NULL != parent_);
        if(parent_->child_[0] == this){
            parent_->child_[0] = NULL;
        }else if(parent_->child_[1] == this){
            parent_->child_[1] = NULL;
        }
    }

    void MemoryAllocator::TreeChunk::copyBlood(TreeChunk* from)
    {
        parent_ = from->parent_;
        child_[0] = from->child_[0];
        child_[1] = from->child_[1];

        if(NULL != child_[0]){
            child_[0]->parent_ = this;
        }
        if(NULL != child_[1]){
            child_[1]->parent_ = this;
        }
    }

    MemoryAllocator::TreeChunk* MemoryAllocator::TreeChunk::getNextChild()
    {
        LASSERT(NULL != child_[1]);
        TreeChunk* child = child_[1];
        TreeChunk* chunk;
        do{
            chunk = child;
            child = (NULL != chunk->child_[0])? chunk->child_[0] : chunk->child_[1];
        }while(NULL != child);

        LASSERT(NULL == chunk->child_[0] && NULL == chunk->child_[1]);
        return chunk;
    }

    MemoryAllocator::TreeChunk* MemoryAllocator::TreeChunk::getPrevChild()
    {
        LASSERT(NULL != child_[0]);
        TreeChunk* child = child_[0];
        TreeChunk* chunk;
        do{
            chunk = child;
            child = (NULL != chunk->child_[1])? chunk->child_[1] : chunk->child_[0];
        }while(NULL != child);

        LASSERT(NULL == chunk->child_[0] && NULL == chunk->child_[1]);
        return chunk;
    }

    //-------------------------------------------------
    //---
    //--- MemoryAllocator::BitwiseTrie
    //---
    //-------------------------------------------------
    inline MemoryAllocator::BitwiseTrie::BitwiseTrie()
        :treeMap_(0)
    {
        for(u32 i=0; i<NumTreeBins; ++i){
            treeBins_[i].setSize(0x01U<<i);
            treeBins_[i].setBinIndex(i);
            LASSERT(lcore::mostSignificantBit(treeBins_[i].getSize()) == i);
        }
    }

    inline MemoryAllocator::BitwiseTrie::~BitwiseTrie()
    {
    }

    inline MemoryAllocator::index_type MemoryAllocator::BitwiseTrie::getBinIndex(size_type size)
    {
        LASSERT(0==(size & LargeSizeMask));
        return lcore::mostSignificantBit(size);
    }

    inline MemoryAllocator::size_type MemoryAllocator::BitwiseTrie::getChildIndex(size_type size, size_type keybit)
    {
        return !!(size&keybit);
    }

    inline MemoryAllocator::size_type MemoryAllocator::BitwiseTrie::getTreeKeyBit(index_type index) const
    {
        return (0x01U << index);
    }

    inline void MemoryAllocator::BitwiseTrie::setTreeMap(size_type keybit)
    {
        treeMap_ |= keybit;
    }

    inline void MemoryAllocator::BitwiseTrie::resetTreeMap(size_type keybit)
    {
        treeMap_ &= ~keybit;
    }

    inline bool MemoryAllocator::BitwiseTrie::isSetTreeMap(size_type keybit) const
    {
        return 0 != (treeMap_ & keybit);
    }

    void MemoryAllocator::BitwiseTrie::insert(TreeChunk* newChunk)
    {
        LASSERT(NULL != newChunk);
        LASSERT(NULL == newChunk->child_[0] && NULL == newChunk->child_[1]);
        LASSERT(NULL == newChunk->parent_);

        size_type size = newChunk->getSize();

        index_type binIndex = getBinIndex(size);
        LASSERT(binIndex<NumTreeBins);

        size_type keybit = getTreeKeyBit(binIndex);
        if(!isSetTreeMap(keybit)){
            setTreeMap(keybit);

            keybit >>= 1;
            size_type childIndex = getChildIndex(size, keybit);

            TreeChunk& chunk = treeBins_[binIndex];
            LASSERT(NULL == chunk.child_[0]);
            LASSERT(NULL == chunk.child_[1]);
            LASSERT(NULL == newChunk->parent_);

            chunk.child_[childIndex] = newChunk;
            newChunk->parent_ = &chunk;

            LASSERT(NULL == newChunk->child_[0] && NULL == newChunk->child_[1]);
            return;
        }

        TreeChunk* chunk = &treeBins_[binIndex];
        for(;;){
            keybit >>= 1;
            size_type childIndex = getChildIndex(size, keybit);

            TreeChunk* child = chunk->child_[childIndex];
            if(NULL == child){
                newChunk->parent_ = chunk;
                chunk->child_[childIndex] = newChunk;
                break;
            }

            chunk = child;

            if(size == chunk->getSize()){
                newChunk->link(chunk->next_);
                break;
            }
        }
    }

    MemoryAllocator::TreeChunk* MemoryAllocator::BitwiseTrie::find(size_type size)
    {
        index_type binIndex = getBinIndex(size);
        LASSERT(binIndex<NumTreeBins);

        size_type keybit = getTreeKeyBit(binIndex);
        if(!isSetTreeMap(keybit)){
            return NULL;
        }

        TreeChunk* chunk = &treeBins_[binIndex];
        for(;;){
            keybit >>= 1;
            size_type childIndex = getChildIndex(size, keybit);

            TreeChunk* child = chunk->child_[childIndex];
            if(NULL == child){
                break;
            }
            chunk = child;

            if(size == chunk->getSize()){
                return chunk;
            }
        }
        return NULL;
    }

    MemoryAllocator::TreeChunk* MemoryAllocator::BitwiseTrie::findLarger(size_type size)
    {
        index_type binIndex = getBinIndex(size);
        size_type keybit = getTreeKeyBit(binIndex);

        TreeChunk* chunk = NULL;
        size_type retSize = (size_type)-1;
        TreeChunk* ret = NULL;

        if(isSetTreeMap(keybit)){
            chunk = &treeBins_[binIndex];
            for(;;){
                if(NULL != chunk->child_[0]){
                    size_type ts = chunk->child_[0]->getSize();
                    size_type d = ts - size;
                    if(size<=ts && d<retSize){
                        ret = chunk->child_[0];
                        retSize = d;
                        if(0 == retSize){
                            break;
                        }
                    }
                }

                if(NULL != chunk->child_[1]){
                    size_type ts = chunk->child_[1]->getSize();
                    size_type d = ts - size;
                    if(size<=ts && d<retSize){
                        ret = chunk->child_[1];
                        retSize = d;
                        if(0 == retSize){
                            break;
                        }
                    }
                }
                keybit >>= 1;
                index_type childIndex = getChildIndex(size, keybit);
                TreeChunk* child = chunk->child_[childIndex];
                if(NULL == child && 0 == childIndex){
                    child = chunk->child_[1];
                }
                if(NULL == child){
                    break;
                }
                chunk = child;
            };

        }else{
            LASSERT(getBinIndex(keybit) == binIndex);
            size_type nextTreeMap = treeMap_ & ~((keybit<<1) - 1);
            index_type nextIndex = lcore::leastSignificantBit(nextTreeMap);
            //size_type nextTreeMap = treeMap_ & ~keybit;
            //index_type nextIndex = lcore::mostSiginificantBit(nextTreeMap);
            if(binIndex < nextIndex){
                chunk = &treeBins_[nextIndex];
                LASSERT(size<chunk->getSize());

                for(;;){
                    TreeChunk* child = NULL;
                    if(NULL != chunk->child_[0]){
                        size_type ts = chunk->child_[0]->getSize();
                        LASSERT(size<ts);
                        if(ts<retSize){
                            ret = chunk->child_[0];
                            retSize = ts;
                        }
                        child = chunk->child_[0];
                    }

                    if(NULL != chunk->child_[1]){
                        size_type ts = chunk->child_[1]->getSize();
                        LASSERT(size<ts);
                        if(ts<retSize){
                            ret = chunk->child_[1];
                            retSize = ts;
                        }
                        if(NULL == child){
                            child = chunk->child_[1];
                        }
                    }
                    if(NULL == child){
                        break;
                    }
                    chunk = child;
                }
            }else{
                return NULL;
            }
        }

        return ret;
    }

    bool MemoryAllocator::BitwiseTrie::contains(const TreeChunk* target) const
    {
        LASSERT(NULL != target);
        size_type size = target->getSize();

        index_type binIndex = getBinIndex(size);
        LASSERT(binIndex<NumTreeBins);
        size_type keybit = getTreeKeyBit(binIndex);
        if(!isSetTreeMap(keybit)){
            return false;
        }

        const TreeChunk* chunk = &treeBins_[binIndex];
        for(;;){
            keybit >>= 1;
            size_type childIndex = getChildIndex(size, keybit);

            const TreeChunk* child = chunk->child_[childIndex];
            if(NULL == child){
                break;
            }
            chunk = child;

            if(size == chunk->getSize()){
                TreeChunk* next = chunk->next_;
                while(next != chunk){
                    if(target == next){
                        return true;
                    }
                    next = next->next_;
                }
                return (target == chunk);
            }
        }
        return false;
    }

    void MemoryAllocator::BitwiseTrie::remove(TreeChunk* chunk)
    {
        LASSERT(NULL != chunk);
        LASSERT( isSetTreeMap( getTreeKeyBit( getBinIndex( chunk->getSize() ) ) ) );

        if(NULL == chunk->parent_){
            LASSERT(NULL == chunk->child_[0]);
            LASSERT(NULL == chunk->child_[1]);
            chunk->unlink();
            return;
        }

        TreeChunk* chunkNext = chunk->next_;
        if(chunkNext == chunk){
            
            //if(NULL == chunkNext->child_[0] && NULL == chunkNext->child_[1]){
            //    chunkNext = NULL;
            //}else{
            //    TreeChunk* next = (NULL != chunkNext->child_[1])? chunkNext->child_[1] : chunkNext->child_[0];
            //    while(NULL != next){
            //        chunkNext = next;
            //        next = (NULL != chunkNext->child_[1])? chunkNext->child_[1] : chunkNext->child_[0];
            //    }
            //    LASSERT(NULL == chunkNext->child_[0] && NULL == chunkNext->child_[1]);
            //    chunkNext->unlinkFromParent();
            //    chunkNext->copyBlood(chunk);
            //}
            if(NULL != chunk->child_[0]){
                chunkNext = chunk->getPrevChild();
                chunkNext->unlinkFromParent();
                chunkNext->copyBlood(chunk);

            }else if(NULL != chunk->child_[1]){
                chunkNext = chunk->getNextChild();
                chunkNext->unlinkFromParent();
                chunkNext->copyBlood(chunk);

            }else{
                chunkNext = NULL;
            }
        }else{
            chunkNext->copyBlood(chunk);
            chunk->unlink();
        }

        TreeChunk* parent = chunk->parent_;
        if(parent->child_[0] == chunk){
            parent->child_[0] = chunkNext;
        }else{
            LASSERT(parent->child_[1] == chunk);
            parent->child_[1] = chunkNext;
        }
        if(parent->haveBinIndex()){
            LASSERT(isSetTreeMap(getTreeKeyBit(parent->getBinIndex())));

            if(NULL == parent->child_[0] && NULL == parent->child_[1]){
                index_type binIndex = parent->getBinIndex();
                LASSERT(binIndex == getBinIndex(chunk->getSize()));

                resetTreeMap(getTreeKeyBit(binIndex));
            }
        }
    }


    //-------------------------------------------------
    //---
    //--- MemoryAllocator::ChunkBase
    //---
    //-------------------------------------------------
    // Œã”ösizeƒoƒCƒg‚ð•ªŠ„
    MemoryAllocator::ChunkBase* MemoryAllocator::ChunkBase::splitBack(size_type size)
    {
        LASSERT(!isInUse());
        LASSERT(size<size_);

        size_ -= size;
        ChunkBase* splitChunk = getNext();
        splitChunk->setSize(size);
        splitChunk->setPrevSize(size_);

        ChunkBase* nextChunk = splitChunk->getNext();
        nextChunk->setPrevSize(size);
        return splitChunk;
    }


    //-------------------------------------------------
    //---
    //--- MemoryAllocator
    //---
    //-------------------------------------------------
    u32 MemoryAllocator::PageBankSize = 4*1024;

    MemoryAllocator::MemoryAllocator()
        :pageBank_(NULL)
        ,allocatedPage_(NULL)
        ,binMap_(0)
    {
        {
            SYSTEM_INFO systemInfo = {0};
            GetSystemInfo(&systemInfo);
            if(PageBankSize<systemInfo.dwPageSize){
                PageBankSize = systemInfo.dwPageSize;
            }
        }

        for(u32 i=0; i<=NumSmallBins; ++i){
            smallBins_[i].size_ = MallocAlign * i;
            smallBins_[i].prevSize_ = 0;
            smallBins_[i].reset();
        }
        pageBank_ = createPageBank();


        page_.prev_ = page_.next_ = &page_;
        page_.size_ = 0;
        page_.memory_ = NULL;

        largePage_.prev_ = largePage_.next_ = &largePage_;
        largePage_.size_ = 0;
        largePage_.memory_ = NULL;
    }

    //------------------------------------------------------
    MemoryAllocator::~MemoryAllocator()
    {
        while(NULL != allocatedPage_){
            freemmap(allocatedPage_->memory_);
            allocatedPage_ = allocatedPage_->next_;
        }

        {
            Page* page = page_.next_;
            while(&page_ != page){
                freemmap(page->getRawMemory());
                page = page->next_;
            }
        }

        {
            Page* page = largePage_.next_;
            while(&largePage_ != page){
                freemmap(page->getRawMemory());
                page = page->next_;
            }
        }

        while(NULL != pageBank_){
            PageBank* next = pageBank_->next_;
            LFREE(pageBank_);
            pageBank_ = next;
        }
    }

    //------------------------------------------------------
    void* MemoryAllocator::allocate(size_type requestSize)
    {
#ifdef LCORE_MEMORYALLOCATOR_CHECK
        requestSize += sizeof(uintptr_t);
#endif
        if(requestSize<=MaxSmallRequest){
            size_type size = (requestSize<MinRequestSize)? MinChunkSize : paddingRequest(requestSize);

            Chunk* chunk = NULL;
            index_type index = getSmallIndex(size);
            LASSERT(0<=index && index<=NumSmallBins);
            size_type bitset = 0x01U<<(index-1);
            if(0 != (binMap_ & bitset)){
                LASSERT(!smallBins_[index].empty());
                chunk = smallBins_[index].next_;
                chunk->unlink();
                if(smallBins_[index].empty()){
                    resetBinMap(index);
                }
            }else{
                size_type nextMap = binMap_ & ~((bitset<<1) - 1);
                index_type nextIndex = lcore::leastSignificantBit(nextMap) + 1;
                if(index<nextIndex){
                    LASSERT(!smallBins_[nextIndex].empty());
                    chunk = smallBins_[nextIndex].next_;
                    chunk->unlink();
                    if(smallBins_[nextIndex].empty()){
                        resetBinMap(nextIndex);
                    }
                }
            }
#if 0
            index_type maxBins = NumSmallBins;//lcore::minimum(index+1, NumSmallBins);

            for(index_type i=index; i<=maxBins; ++i){
                Chunk& rootChunk = smallBins_[i];
                LASSERT(getSmallSize(i) == rootChunk.getSize());
                LASSERT(size<=rootChunk.getSize());
                if(!rootChunk.empty()){
                    chunk = rootChunk.next_;
                    chunk->unlink();
                    if(rootChunk.empty()){
                        resetBinMap(i);
                    }
                    break;
                }
            }
#endif

            if(NULL == chunk){
                chunk = allocateChunk(size);

            }else if(size<chunk->getSize()){
                Chunk* splitChunk = (Chunk*)chunk->splitBack(size);
                LASSERT(MinChunkSize<=chunk->getSize());
                LASSERT(splitChunk->getPrevSize() == chunk->getSize());
                LASSERT(!chunk->isInUse());
                LASSERT(!splitChunk->isInUse());
                LASSERT(splitChunk->getSize() == size);

                index_type restIndex = getSmallIndex(chunk->getSize());
                LASSERT(0<=restIndex && restIndex<=NumSmallBins);
                Chunk& rootChunk = smallBins_[restIndex];
                LASSERT(getSmallSize(restIndex) == rootChunk.getSize());
                chunk->link(rootChunk.next_);
                setBinMap(restIndex);
                chunk = splitChunk;

            }
            LASSERT(MinChunkSize<=chunk->getPrevSize());
            LASSERT(chunk->getSize() == size);
            chunk->setInUse();

#ifdef LCORE_MEMORYALLOCATOR_CHECK
            setMagic(chunk, chunk->getSize());
#endif
            return chunk->getMemory();

        }else if(requestSize<=MaxLargeRequest){
            size_type size = (requestSize<MinLargeSize)? paddingLargeRequest(MinLargeSize) : paddingLargeRequest(requestSize);
            TreeChunk* chunk = largeTree_.findLarger(size);
            if(NULL != chunk){
                largeTree_.remove(chunk);

                if(size<chunk->getSize()){
                    TreeChunk* splitChunk = (TreeChunk*)chunk->splitBack(size);
                    LASSERT(MinLargeSize<=chunk->getSize());
                    LASSERT(splitChunk->getPrevSize() == chunk->getSize());
                    LASSERT(!chunk->isInUse());
                    LASSERT(!splitChunk->isInUse());
                    LASSERT(splitChunk->getSize() == size);
                    chunk->resetAllLink();
                    largeTree_.insert(chunk);

                    chunk = splitChunk;
                }

            }else{
                chunk = allocateTreeChunk(size);
            }
            LASSERT(MinChunkSize<=chunk->getPrevSize());
            chunk->setInUse();

#ifdef LCORE_MEMORYALLOCATOR_CHECK
            setMagic(chunk, size);
#endif
            return chunk->getMemory();

        }else{
            size_type size = (requestSize<MinExternalSize)? paddingExternalRequest(MinExternalSize) : paddingExternalRequest(requestSize);
            void* mem = LMALLOC(size);
            Chunk* chunk = (Chunk*)mem;
            chunk->prevSize_ = 0;
            chunk->size_ = size;
#ifdef LCORE_MEMORYALLOCATOR_CHECK
            setMagic(chunk, size);
#endif
            return chunk->getMemory();
        }
    }

    //------------------------------------------------------
    void MemoryAllocator::deallocate(void* ptr)
    {
        if(NULL == ptr){
            return;
        }
        ChunkBase* chunkBase = ChunkBase::getChunk(ptr);
        size_type size = chunkBase->getSize();

#ifdef LCORE_MEMORYALLOCATOR_CHECK
        LASSERT(checkMagic(chunkBase, size));
#endif
        if(size<=MinLargeSize){
            Chunk* chunk = (Chunk*)chunkBase;
            chunk->resetInUse();
            chunk->reset();

            //chunk = combinePrevious(chunk);
            chunk = combineAround(chunk);
            LASSERT(MinChunkSize<=chunk->getPrevSize());
            size = chunk->getSize();

            index_type index = getSmallIndex(size);
            LASSERT(0<=index && index<=NumSmallBins);
            Chunk& rootChunk = smallBins_[index];

            LASSERT(getSmallSize(index) == rootChunk.getSize());
            LASSERT(size == rootChunk.getSize());
            chunk->link(rootChunk.next_);
            setBinMap(index);
            LASSERT(!chunk->empty());

        }else if(size<=MinExternalSize){
            TreeChunk* treeChunk = (TreeChunk*)chunkBase;
            treeChunk->resetInUse();
            //treeChunk = combinePrevious(treeChunk);
            treeChunk = combineAround(treeChunk);

            treeChunk->resetAllLink();

            LASSERT(size<=treeChunk->getSize());
            largeTree_.insert(treeChunk);

        }else{
            LFREE(chunkBase);
        }
    }

    //------------------------------------------------------
    inline void* MemoryAllocator::allocmmap(size_type size)
    {
        return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    }

    //------------------------------------------------------
    inline void MemoryAllocator::freemmap(void* ptr)
    {
        LASSERT(NULL != ptr);
        VirtualFree(ptr, 0, MEM_RELEASE);
    }

    //------------------------------------------------------
    inline MemoryAllocator::Chunk* MemoryAllocator::combinePrevious(Chunk* chunk)
    {
        LASSERT(NULL != chunk);
        LASSERT(!chunk->isInUse());
        LASSERT((chunk->prev_ == chunk) && (chunk->next_ == chunk));

        size_type size = chunk->getSize();
        Chunk* prev = (Chunk*)chunk->getPrev();
        if(prev->isInUse()){
            return chunk;
        }
        size_type s = size + prev->getSize();
        if(MinLargeSize<s){
            return chunk;
        }
        size = s;
        LASSERT(!prev->empty());
        LASSERT((NULL!=prev->prev_) && (NULL != prev->next_));

        index_type binIndex = getSmallIndex(prev->getSize());
        LASSERT(smallBins_[binIndex].getSize() == prev->getSize());

        prev->unlink();
        if(smallBins_[binIndex].empty()){
            resetBinMap(binIndex);
        }

        prev->setSize(size);

        ChunkBase* nextChunk = prev->getNext();
        nextChunk->setPrevSize(size);
        return prev;
    }

    //------------------------------------------------------
    inline MemoryAllocator::Chunk* MemoryAllocator::combineAround(Chunk* chunk)
    {
        LASSERT(NULL != chunk);
        LASSERT(!chunk->isInUse());
        LASSERT((chunk->prev_ == chunk) && (chunk->next_ == chunk));

        size_type size = chunk->getSize();
        Chunk* prev;
        Chunk* next;
        size_type total;
        index_type binIndex;

        prev = (Chunk*)chunk->getPrev();
        if(prev->isInUse()){
            next = (Chunk*)chunk->getNext();
            goto COMBINE_NEXT;
        }
        total = size + prev->getSize();
        if(MinLargeSize<total){
            next = (Chunk*)chunk->getNext();
            goto COMBINE_NEXT;
        }
        size = total;
        LASSERT(!prev->empty());
        LASSERT((NULL!=prev->prev_) && (NULL != prev->next_));

        binIndex = getSmallIndex(prev->getSize());
        LASSERT(smallBins_[binIndex].getSize() == prev->getSize());

        prev->unlink();
        if(smallBins_[binIndex].empty()){
            resetBinMap(binIndex);
        }
        chunk = prev;
        chunk->setSize(size);

        next = (Chunk*)chunk->getNext();
        next->setPrevSize(size);

COMBINE_NEXT:
        LASSERT(0<next->getSize());
        LASSERT(next->getPrevSize() == chunk->getSize());
        if(next->isInUse()){
            return chunk;
        }

        total = size + next->getSize();
        if(MinLargeSize<total){
            return chunk;
        }
        size = total;

        LASSERT(!next->empty());
        LASSERT((NULL!=next->prev_) && (NULL != next->next_));
        binIndex = getSmallIndex(next->getSize());
        LASSERT(smallBins_[binIndex].getSize() == next->getSize());

        next->unlink();
        if(smallBins_[binIndex].empty()){
            resetBinMap(binIndex);
        }

        chunk->setSize(size);
        next = (Chunk*)chunk->getNext();
        next->setPrevSize(size);

        return chunk;
    }

    //------------------------------------------------------
    inline MemoryAllocator::TreeChunk* MemoryAllocator::combinePrevious(TreeChunk* chunk)
    {
        LASSERT(NULL != chunk);
        LASSERT(!chunk->isInUse());

        size_type size = chunk->getSize();

        TreeChunk* prev = (TreeChunk*)chunk->getPrev();
        LASSERT(0<prev->getSize());
        LASSERT(chunk->getPrevSize() == prev->getSize());
        LASSERT(chunk->getNext()->getPrevSize() == chunk->getSize());

        if(prev->isInUse()){
            return chunk;
        }
        size_type s = size + prev->getSize();
        if(MaxLargeSize<s){
            return chunk;
        }
        LASSERT(!largeTree_.contains(chunk));
        LASSERT(largeTree_.contains(prev));
        largeTree_.remove(prev);

        size = s;
        chunk = prev;

        LASSERT(0<chunk->getPrevSize());

        chunk->setSize(size);
        ChunkBase* nextChunk = chunk->getNext();
        nextChunk->setPrevSize(size);

        return chunk;
    }

    //------------------------------------------------------
    inline MemoryAllocator::TreeChunk* MemoryAllocator::combineAround(TreeChunk* chunk)
    {
        LASSERT(NULL != chunk);
        LASSERT(!chunk->isInUse());
        LASSERT(!largeTree_.contains(chunk));

        TreeChunk* prev;
        TreeChunk* next;
        size_type size = chunk->getSize();

        prev = (TreeChunk*)chunk->getPrev();
        LASSERT(0<prev->getSize());
        LASSERT(chunk->getPrevSize() == prev->getSize());
        LASSERT(chunk->getNext()->getPrevSize() == chunk->getSize());
        if(prev->isInUse()){
            next = (TreeChunk*)chunk->getNext();
            goto COMBINE_NEXT;
        }

        size += prev->getSize();

        LASSERT(largeTree_.contains(prev));
        largeTree_.remove(prev);

        chunk = prev;
        LASSERT(0<chunk->getPrevSize());

        chunk->setSize(size);
        next = (TreeChunk*)chunk->getNext();
        next->setPrevSize(size);

COMBINE_NEXT:
        LASSERT(0<next->getSize());
        LASSERT(next->getPrevSize() == chunk->getSize());
        if(next->isInUse()){
            return chunk;
        }

        size += next->getSize();

        LASSERT(largeTree_.contains(next));
        largeTree_.remove(next);

        chunk->setSize(size);
        next = (TreeChunk*)chunk->getNext();
        next->setPrevSize(size);

        return chunk;
    }

    //------------------------------------------------------
    MemoryAllocator::Chunk* MemoryAllocator::allocateChunk(size_type size)
    {
        Page* page = page_.next_;
        while(&page_ != page){
            if(size<=page->size_){
                break;
            }
            page = page->next_;
        }

        if(&page_ == page){
            page = allocatePage(PageSize);
            page->link(page_.next_);
        }

        Chunk* chunk = page->splitMemory<Chunk>(size);
        LASSERT(MinChunkSize<=chunk->getPrevSize());
        if(page->size_ < MinChunkSize){
            page->unlink();
            page->next_ = allocatedPage_;
            allocatedPage_ = page;
        }
        return chunk;
    }

    //------------------------------------------------------
    MemoryAllocator::TreeChunk* MemoryAllocator::allocateTreeChunk(size_type size)
    {
        Page* page = largePage_.next_;
        while(&largePage_ != page){
            if(size<=page->size_){
                break;
            }
            page = page->next_;
        }

        if(&largePage_ == page){
            page = allocatePage(LargePageSize);
            page->link(largePage_.next_);
        }

        TreeChunk* chunk = page->splitMemory<TreeChunk>(size);
        LASSERT(chunk->getPrev()->isInUse());
        if(page->size_ < MinLargeSize){
            page->unlink();
            page->next_ = allocatedPage_;
            allocatedPage_ = page;
        }
        return chunk;
    }

    //------------------------------------------------------
    MemoryAllocator::Page* MemoryAllocator::allocatePage(size_type size)
    {
        LASSERT(NULL != pageBank_);

        //PageBank‚ª–³‚­‚È‚ê‚Îì¬
        if(pageBank_->maxPages_<=pageBank_->numPages_){
            PageBank* pageBank = createPageBank();
            pageBank->next_ = pageBank_;
            pageBank_ = pageBank;
        }

        ++pageBank_->numPages_;
        Page* page = (Page*)((pointer_type)pageBank_ + sizeof(Page)*(pageBank_->numPages_));
        void* mem = allocmmap(size);
        page->reset(size, mem);
        LASSERT(page->getRawMemory() == mem);
        return page;
    }

    //------------------------------------------------------
    MemoryAllocator::PageBank* MemoryAllocator::createPageBank()
    {
        LASSERT(sizeof(PageBank)<=sizeof(Page));

        PageBank* pageBank = (PageBank*)LMALLOC(PageBankSize);
        pageBank->next_ = NULL;
        pageBank->maxPages_ = PageBankSize/sizeof(Page) - 1;
        pageBank->numPages_ = 0;
        return pageBank;
    }

    //------------------------------------------------------
    void MemoryAllocator::setMagic(void* ptr, size_type size)
    {
        lcore::memset((pointer_type)ptr+ChunkOverhead, 0, size-ChunkOverhead);
        uintptr_t* magic = (uintptr_t*)((pointer_type)ptr + size - sizeof(uintptr_t));
        *magic = (uintptr_t)ptr;
    }

    //------------------------------------------------------
    bool MemoryAllocator::checkMagic(void* ptr, size_type size)
    {
        uintptr_t* magic = (uintptr_t*)((pointer_type)ptr + size - sizeof(uintptr_t));
        bool ret = (*magic == (uintptr_t)ptr);
        lcore::memset((pointer_type)ptr+ChunkOverhead, 0, size-ChunkOverhead);
        return ret;
    }
}
