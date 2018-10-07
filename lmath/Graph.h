#ifndef INC_LMATH_GRAPH_H__
#define INC_LMATH_GRAPH_H__
/**
@file Graph.h
@author t-sakai
@date 2016/06/03 create
*/
#include "lmath.h"

namespace lmath
{
    class UnionFind
    {
    public:
        struct Edge
        {
            s32 v0_;
            s32 v1_;
            s32 index_;
            s32 cost_;

            inline bool operator<(const Edge& rhs) const
            {
                return cost_<rhs.cost_;
            }
        };

        struct Node
        {
            s32 parent_;
            s32 rank_;
        };

        static Node makeSet(s32 x)
        {
            return {x,0};
        }

        UnionFind();
        ~UnionFind();


        /**
        @brief Kruskal's algorithmで最少全域木のコストを計算
        */
        s32 krusal(s32 numVertices, s32 numEdges, Edge* edges);

    private:
        UnionFind(const UnionFind&) = delete;
        UnionFind& operator=(const UnionFind&) = delete;

        s32 find(s32 x);
        void unionSet(s32 x, s32 y);
        inline bool isSame(s32 x, s32 y);
        inline s32 getRank(s32 x);

        Node* nodes_;
    };
}
#endif //INC_LMATH_GRAPH_H__
