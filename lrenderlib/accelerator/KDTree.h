#ifndef INC_LRENDER_KDTREE_H__
#define INC_LRENDER_KDTREE_H__
/**
@file KDTree.h
@author t-sakai
@date 2013/06/07 create
*/
#include "../lrender.h"
#include <lcore/Vector.h>

namespace lrender
{
    struct Photon;

    class KDTree
    {
    public:
        static const u32 MinLeafElements = 16;

        KDTree();
        ~KDTree();

        void add(const Photon& photon);

        void construct();
    private:
        struct Node
        {
            static const u8 AxisX = 0x00U;
            static const u8 AxisY = 0x01U;
            static const u8 AxisZ = 0x02U;
            static const u8 LeafFlag = 0x04U;

            bool isLeaf() const
            {
                return (axis_&LeafFlag) != 0;
            }

            void setInner(f32 split, u8 axis)
            {
                split_ = split;
                axis_ = axis;
            }

            void setLeaf(u16 index, u8 num)
            {
                index_ = index;
                num_ = num;
                axis_ |= LeafFlag;
            }

            f32 split_;
            u16 index_;
            u8 axis_;
            u8 num_;
        };

        s32 KDTree::getParentIndex(s32 index);
        s32 KDTree::getChildStartIndex(s32 index);
        Node& createNode(s32 index);

        void recursiveConstruct(u32 start, u32 end, Photon* photons, s32 nodeIndex);

        lcore::vector_arena<Node> nodes_;
        lcore::vector_arena<Photon> photons_;
    };
}
#endif //INC_LRENDER_KDTREE_H__
