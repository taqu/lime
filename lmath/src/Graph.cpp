/**
@file Graph.cpp
@author t-sakai
@date 2016/06/03 create
*/
#include "Graph.h"
#include <lcore/Random.h>
#include <lcore/Sort.h>

namespace lmath
{
    UnionFind::UnionFind()
        :nodes_(NULL)
    {
    }

    UnionFind::~UnionFind()
    {
        LFREE(nodes_);
    }


    s32 UnionFind::krusal(s32 numVertices, s32 numEdges, Edge* edges)
    {
        LASSERT(0<=numVertices);
        LFREE(nodes_);
        nodes_ = (Node*)LMALLOC(sizeof(Node)*numVertices);

        //コストが小さい順にソート
        lcore::introsort(numEdges, edges);
        //ランダムにシャッフルすると迷路が作れる
        for(s32 i=0; i<numVertices; ++i){
            nodes_[i] = makeSet(i);
        }
        s32 minimum_cost = 0;
        for(s32 i=0; i<numEdges; ++i){
            Edge& edge = edges[i];
            if(!isSame(edge.v0_, edge.v1_)){
                minimum_cost += edge.cost_;
                unionSet(edge.v0_, edge.v1_);
            }
        }
        return minimum_cost;
    }

    s32 UnionFind::find(s32 x)
    {
        if(x==nodes_[x].parent_){
            return x;
        } else{
            nodes_[x].parent_ = find(nodes_[x].parent_);
            return nodes_[x].parent_;
        }
    }

    void UnionFind::unionSet(s32 x, s32 y)
    {
        x = find(x);
        y = find(y);

        if(x==y){
            return;
        }
        if(nodes_[x].rank_ < nodes_[y].rank_){
            lcore::swap(x,y);
        }
        nodes_[y].parent_ = x;
        nodes_[x].rank_ += nodes_[y].rank_;
        nodes_[y].rank_ = 0;
    }

    inline bool UnionFind::isSame(s32 x, s32 y)
    {
        return find(x) == find(y);
    }

    inline s32 UnionFind::getRank(s32 x)
    {
        return nodes_[find(x)].rank_;
    }
}
