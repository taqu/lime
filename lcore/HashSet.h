#ifndef INC_LCORE_HASHSET_H__
#define INC_LCORE_HASHSET_H__
/**
@file HashSet.h
@author t-sakai
@date 2010/12/09 create

*/
#include "lcore.h"
#include "Hash.h"

namespace lcore
{
    namespace hash_detail
    {
        template<class Value, class Hasher>
        class HashSetTable
        {
        public:
            typedef Value value_type;

            typedef HashSetTable<Value, Hasher> this_type;

            typedef size_t size_type;
            typedef ptrdiff_t difference_type;

            typedef typename type_traits<Value>::pointer pointer;
            typedef typename type_traits<Value>::const_pointer const_pointer;
            typedef typename type_traits<Value>::reference reference;
            typedef typename type_traits<Value>::const_reference const_reference;
            typedef typename type_traits<Value>::param_tye value_param_type;
            typedef typename type_traits<Value>::const_param_tye const_value_param_type;

            typedef value_type* value_pointer;
            typedef type_traits<Value> value_traits;

            typedef Hasher hasher;

            typedef AllocatorNew<value_type> value_allocator;

            HashSetTable()
                :bucketCount_(0),
                values_(NULL),
                size_(0)
            {
            }

            explicit HashSetTable(size_type n)
                :bucketCount_(next_prime(n)),
                size_(0)
            {
                createBuckets(bucketCount_);
            }

            HashSetTable(size_type n, value_param_type emptyValue)
                :bucketCount_(next_prime(n)),
                empty_(emptyValue),
                size_(0)
            {
                createBuckets(bucketCount_);
            }

            ~HashSetTable()
            {
                destroyBuckets(0, bucketCount_);
            }

            size_type bucket_count() const
            {
                return bucketCount_;
            }

            size_type max_bucket_count(size_type max_size) const
            {
                return prev_prime(max_size);
            }

            size_type calcHash(const_value_param_type value) const
            {
                return hasher::calc(value) % bucketCount_;
            }

            void createBuckets(size_type count)
            {
                AllocatorArrayConstructor<value_allocator>
                    value_constructor(value_alloc_);
                value_constructor.construct(count);

                for(size_type i=0; i<count; ++i){
                    setValue(empty_, value_constructor.get() + i);
                }

                values_ = value_constructor.release();
            }

            void destroyBuckets(size_type first, size_type last)
            {
                while(first != last){
                    value_alloc_.destroy(values_ + first);
                    ++first;
                }
                value_alloc_.deallocate(values_);
                values_ = NULL;
            }

            void setValue(const_value_param_type src, pointer dst)
            {
                value_alloc_.destroy(dst);
                value_alloc_.construct(dst, src);
            }

            bool isEmpty(size_type pos) const
            {
                return (values_[pos] == empty_);
            }

            bool isValid(size_type pos) const
            {
                return (0<=pos && pos<=bucketCount_);
            }

            void setEmptyValue(const_value_param_type value)
            {
                empty_ = value;
            }

            size_type findEmptyPosition(const_value_param_type value) const
            {
                size_type pos = calcHash(value);
                size_type n = pos;

                do{
                    if(isEmpty(n)){
                        return n;
                    }
                    ++n;
                    if(n>=bucketCount_){
                        n-=bucketCount_;
                        LASSERT(n<bucketCount_);
                    }
                }while(n != pos);

                return end();
            }

            size_type find(const_value_param_type value) const
            {
                size_type pos = calcHash(value);
                size_type n = pos;

                do{
                    if(value == values_[n]){
                        return n;
                    }

                    if(isEmpty(n)){
                        return end();
                    }
                    ++n;
                    if(n>=bucketCount_){
                        n-=bucketCount_;
                        LASSERT(n<bucketCount_);
                    }
                }while(n != pos);

                return end();
            }

            bool insert(const_value_param_type value)
            {
                size_type pos = find(value);
                if(pos != end()){
                    return false;
                }
                if((size_<<1)>=bucketCount_){
                    expand();
                }
                
                pos = findEmptyPosition(value);
                if(pos != end()){
                    setValue(value, values_ + pos);
                    ++size_;
                    return true;
                }
                return false;
            }

            void erase(size_type pos)
            {
                assert(isValid(pos));
                if(isEmpty(pos)){
                    return;
                }
                setValue(empty_, values_ + pos);
                --size_;
            }

            void swap(HashSetTable& rhs)
            {
                lcore::swap(values_, rhs.values_);
                lcore::swap(bucketCount_, rhs.bucketCount_);
                lcore::swap(size_, rhs.size_);

                value_traits::swap(empty_, rhs.empty_);
            }

            size_type size() const
            {
                return size_;
            }

            size_type begin() const
            {
                for(size_type i=0; i!=end(); ++i){
                    if(!isEmpty(i)){
                        return i;
                    }
                }
                return end();
            }

            size_type end() const
            {
                return bucketCount_;
            }

            size_type next(size_type pos) const
            {
                for(size_type i=pos+1; i!=end(); ++i){
                    if(!isEmpty(i)){
                        return i;
                    }
                }
                return end();
            }

            void expand()
            {
                size_type n = next_prime(bucketCount_+1);

                this_type tmp(n, empty_);

                size_type itr = begin();
                while(itr != end()){
                    value_type& value = values_[itr];

                    if(false == tmp.insert(value)){
                        LASSERT(false);
                    }

                    itr = next(itr);
                }
                tmp.swap(*this);
            }

            value_type empty_;
            value_pointer values_;
            size_type size_;
            size_type bucketCount_;
            value_allocator value_alloc_;
        };
    }


    template<class Value, class Hasher = hasher<Value> >
    class HashSet
    {
        typedef hash_detail::HashSetTable<Value, Hasher> hash_table;
    public:
        typedef HashSet<Value, Hasher> this_type;
        typedef typename hash_table::size_type size_type;

        typedef Value value_type;

        explicit HashSet(value_type& emptyKey)
            :table_(size, emptyKey)
        {}

        HashSet(size_type size, value_type& emptyKey)
            :table_(size, emptyKey)
        {
        }

        ~HashSet()
        {
        }

        size_type size() const
        {
            return table_.size();
        }

        const value_type& getValue(size_type pos) const
        {
            assert(table_.isValid(pos));
            assert(!table_.isEmpty(pos));
            return table_.values_[pos];
        }

        value_type& getValue(size_type pos)
        {
            LASSERT(table_.isValid(pos));
            LASSERT(!table_.isEmpty(pos));
            return table_.values_[pos];
        }

        bool insert(const value_type& value)
        {
            return table_.insert(value);
        }

        size_type find(const value_type& value) const
        {
            return table_.find(value);
        }

        bool isValid(size_type pos) const
        {
            return table_.isValid();
        }

        bool isEnd(size_type pos) const
        {
            return (pos == table_.end());
        }

        void erase(size_type pos)
        {
            table_.erase(pos);
        }

        void swap(this_type& rhs)
        {
            table_.swap(rhs.table_);
        }

        void setEmptyValue(const value_type& value)
        {
            table_.setEmptyValue(value);
        }

        size_type begin() const
        {
            return table_.begin();
        }

        size_type end() const
        {
            return table_.end();
        }

        size_type next(size_type pos) const
        {
            return table_.next(pos);
        }
    private:

        hash_table table_;
    };

}
#endif //INC_LCORE_HASHSET_H__
