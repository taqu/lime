#ifndef INC_LMATH_DEPTHCLUSTER_H__
#define INC_LMATH_DEPTHCLUSTER_H__
/**
@file DepthCluster.h
@author t-sakai
@date 2015/06/01 create
*/
#include "../lmath.h"
#include "../Vector4.h"

namespace lmath
{
    class DepthCluster
    {
    public:
        static const u32 Align = 16;
        static const s32 MaxNumElements = 16;
        static const s32 MaxLevels = 8;
        static const u16 Invalid = 0xFFFFU;
        static const u16 MaxNum = Invalid - 1;

        struct Vertex
        {
            f32 z_;
            u16 id_;
            u16 material_;
        };

        struct Node
        {
            u16 left_;
            u16 right_;
        };

        class Iterator
        {
        public:
            void operator++();
            inline bool next() const;

            inline u16 start() const;
            inline u16 end() const;
            inline const Vertex& get(u16 index) const;
        private:
            friend class DepthCluster;

            u16 start_;
            u16 end_;
            u16 size_;
            u16 material_;
            Vertex* vertices_;
        };

        DepthCluster();
        explicit DepthCluster(s32 capacity);
        ~DepthCluster();

        void add(f32 z, u16 id, u16 material);
        void construct();
        void clear();

        Iterator begin();
    private:
        friend class Iterator;

        DepthCluster(const DepthCluster&);
        DepthCluster& operator=(const DepthCluster&);

        static bool compare(const Vertex& v0, const Vertex& v1);

        void resize();

        s32 capacity_;
        s32 size_;
        Vertex* vertices_;
    };

    inline bool DepthCluster::Iterator::next() const
    {
        return start_<size_;
    }

    inline u16 DepthCluster::Iterator::start() const
    {
        return start_;
    }

    inline u16 DepthCluster::Iterator::end() const
    {
        return end_;
    }

    inline const DepthCluster::Vertex& DepthCluster::Iterator::get(u16 index) const
    {
        return vertices_[index];
    }
}

#endif //INC_LMATH_DEPTHCLUSTER_H__
