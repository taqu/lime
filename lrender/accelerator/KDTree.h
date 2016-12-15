#ifndef INC_LRENDER_KDTREE_H__
#define INC_LRENDER_KDTREE_H__
/**
@file KDTree.h
@author t-sakai
@date 2013/06/07 create
*/
#include "../lrender.h"
#include <lcore/Array.h>
#include <lcore/Sort.h>

namespace lrender
{
    struct Point
    {
        inline const Vector3& getPosition() const
        {
            return position_;
        }

        Vector3 position_;
    };

    struct Photon
    {
        inline const Vector3& getPosition() const
        {
            return position_;
        }

        static void estimateIrradiance(
            Vector3& irradiance,
            const Vector3& position,
            const Vector3& normal,
            s32 num,
            s32 minNum,
            const f32* sqrDistances,
            const Photon** photons);

        Vector3 position_;
        u16 theta_;
        u16 phi_;
        Vector3 power_;
    };


    template<class T, s32 NumMaxLeaves=16>
    class KDTree
    {
    public:
        typedef KDTree<T> this_type;
        typedef T value_type;
        static const s32 MaxLeafElements = NumMaxLeaves;

        struct Node
        {
            static const u16 AxisX = 0x00U;
            static const u16 AxisY = 0x01U;
            static const u16 AxisZ = 0x02U;
            static const u16 Leaf  = 0x04U;

            union Value
            {
                f32 split_;
                s32 index_;
            };

            bool isLeaf() const
            {
                return (child_&Leaf) != 0;
            }

            u16 getAxis() const
            {
                return (child_&0x07U);
            }

            u16 getNum() const
            {
                return (child_>>4);
            }

            s32 getLeftChild() const
            {
                return (child_>>4);
            }

            f32 getSplit() const
            {
                return value_.split_;
            }

            s32 getIndex() const
            {
                return value_.index_;
            }

            void setInner(f32 split, u16 axis, s32 leftChild)
            {
                value_.split_ = split;
                child_ = (leftChild<<4) | axis;
            }

            void setLeaf(s32 index, u32 num)
            {
                value_.index_ = index;
                child_ = (num<<4) | Leaf;
            }

            f32 signedDistance(const Vector3& p) const
            {
                switch(getAxis())
                {
                case AxisX:
                    return p.x_ - value_.split_;
                case AxisY:
                    return p.y_ - value_.split_;
                case AxisZ:
                    return p.z_ - value_.split_;
                default:
                    LASSERT(false);
                    return 0.0f;
                }
            }

            Value value_;
            u32 child_;
        };

        typedef Node node_type;

        struct Nearests
        {
            Nearests();
            Nearests(s32 size, const Vector3& position, f32 maxDistance, f32* sqrDistances, const value_type** points);

            void clear(f32 maxSqrDistance);
            void add(f32 sqrDistance, const value_type* point);
            f32 maxSqrDistance() const
            {
                return (0<found_)? sqrDistances_[found_-1] : sqrDistances_[0];
            }

            s32 size_;
            s32 found_;
            Vector3 position_;
            f32* sqrDistances_;
            const value_type** points_;
        };

        KDTree();
        ~KDTree();

        inline s32 getNumNodes() const;
        inline const node_type& getNode(s32 index) const;

        inline const value_type* getLeafValues(s32 index) const;

        void build(s32 num, const value_type* points);

        inline s32 getNumPoints() const;
        inline value_type* getPoints();
        void resizePoints(s32 num);
        void build();

        inline void gather(Nearests& nearests)
        {
            if(0<nodes_.size()){
                gather(nearests, 0);
            }
        }
    private:
        typedef bool(*CompareFunc)(const value_type& p0, const value_type& p1);
        typedef s32(*SearchFunc)(f32 value, const value_type& p);

        static bool compare_x(const value_type& p0, const value_type& p1)
        {
            return p0.getPosition().x_<p1.getPosition().x_;
        }

        static bool compare_y(const value_type& p0, const value_type& p1)
        {
            return p0.getPosition().y_<p1.getPosition().y_;
        }

        static bool compare_z(const value_type& p0, const value_type& p1)
        {
            return p0.getPosition().z_<p1.getPosition().z_;
        }

        static s32 search_x(f32 value, const value_type& p)
        {
            if(value<p.getPosition().x_){
                return -1;
            }else if(p.getPosition().x_<value){
                return 1;
            }
            return 0;
        }

        static s32 search_y(f32 value, const value_type& p)
        {
            if(value<p.getPosition().y_){
                return -1;
            }else if(p.getPosition().y_<value){
                return 1;
            }
            return 0;
        }

        static s32 search_z(f32 value, const value_type& p)
        {
            if(value<p.getPosition().z_){
                return -1;
            }else if(p.getPosition().z_<value){
                return 1;
            }
            return 0;
        }

        void recursiveConstruct(s32 nodeIndex, s32 offset, s32 n, const Vector3& bmin, const Vector3& bmax);
        void gather(Nearests& nearests, s32 nodeIndex);
        static s32 search(f32 split, s32 num, const value_type* points, SearchFunc comp);

        s32 numPoints_;
        value_type* points_;

        typedef lcore::Array<Node> NodeVector;
        NodeVector nodes_;
    };

    template<class T, s32 NumMaxLeaves>
    KDTree<T, NumMaxLeaves>::Nearests::Nearests()
        :size_(0)
        ,found_(0)
        ,position_(0.0f)
        ,sqrDistances_(NULL)
        ,points_(NULL)
    {
        clear(lcore::numeric_limits<f32>::maximum());
    }

    template<class T, s32 NumMaxLeaves>
    KDTree<T, NumMaxLeaves>::Nearests::Nearests(s32 size, const Vector3& position, f32 maxDistance, f32* sqrDistances, const T** points)
        :size_(size)
        ,found_(0)
        ,position_(position)
        ,sqrDistances_(sqrDistances)
        ,points_(points)
    {
        clear(maxDistance*maxDistance);
    }

    template<class T, s32 NumMaxLeaves>
    void KDTree<T, NumMaxLeaves>::Nearests::clear(f32 maxSqrDistance)
    {
        for(s32 i=0; i<size_; ++i){
            sqrDistances_[i] = maxSqrDistance;
        }
    }

    template<class T, s32 NumMaxLeaves>
    void KDTree<T, NumMaxLeaves>::Nearests::add(f32 sqrDistance, const T* point)
    {
        for(s32 i=0; i<size_; ++i){
            if(sqrDistance <= sqrDistances_[i]){
                for(s32 j=size_-1; i<j; --j){
                    sqrDistances_[j] = sqrDistances_[j-1];
                    points_[j] = points_[j-1];
                }
                sqrDistances_[i] = sqrDistance;
                points_[i] = point;
                found_ = lcore::minimum(size_, found_+1);
                break;
            }
        }
    }

    template<class T, s32 NumMaxLeaves>
    KDTree<T, NumMaxLeaves>::KDTree()
        :numPoints_(0)
        ,points_(NULL)
    {
    }

    template<class T, s32 NumMaxLeaves>
    KDTree<T, NumMaxLeaves>::~KDTree()
    {
        numPoints_ = 0;
        LFREE(points_);
    }

    template<class T, s32 NumMaxLeaves>
    inline s32 KDTree<T, NumMaxLeaves>::getNumNodes() const
    {
        return nodes_.size();
    }

    template<class T, s32 NumMaxLeaves>
    inline const typename KDTree<T, NumMaxLeaves>::node_type& KDTree<T, NumMaxLeaves>::getNode(s32 index) const
    {
        return nodes_[index];
    }

    template<class T, s32 NumMaxLeaves>
    inline const typename KDTree<T, NumMaxLeaves>::value_type* KDTree<T, NumMaxLeaves>::getLeafValues(s32 index) const
    {
        CASSERT(0<=index && index<numNodes_);
        const node_type& node = nodes_[index];
        CASSERT(node.isLeaf());
        CASSERT(0<=node.index_ && node.index_<numPoints_);
        CASSERT((node.num_+node.index_)<=numPoints_);
        return points_ + node.index_;
    }

    template<class T, s32 NumMaxLeaves>
    void KDTree<T, NumMaxLeaves>::build(s32 num, const value_type* points)
    {
        numPoints_ = 0;
        LFREE(points_);
        nodes_.clear();
        if(num<=0){
            return;
        }
        numPoints_ = num;
        points_ = (value_type*)LMALLOC(sizeof(value_type)*numPoints_);
        lcore::memcpy(points_, points, sizeof(value_type)*numPoints_);
        s32 numNodes = 2 * lmath::float2S32((f32)num/MaxLeafElements);
        nodes_.reserve(numNodes);

        Vector3 bmin = points_[0].getPosition();
        Vector3 bmax = points_[0].getPosition();
        for(s32 i=1; i<num; ++i){
            bmin = minimum(bmin, points_[i].getPosition());
            bmax = maximum(bmax, points_[i].getPosition());
        }
        Node root;
        root.setLeaf(0, num);
        nodes_.push_back(root);
        recursiveConstruct(0, 0, num, bmin, bmax);
    }

    template<class T, s32 NumMaxLeaves>
    inline s32 KDTree<T, NumMaxLeaves>::getNumPoints() const
    {
        return numPoints_;
    }

    template<class T, s32 NumMaxLeaves>
    inline typename KDTree<T, NumMaxLeaves>::value_type* KDTree<T, NumMaxLeaves>::getPoints()
    {
        return points_;
    }

    template<class T, s32 NumMaxLeaves>
    void KDTree<T, NumMaxLeaves>::resizePoints(s32 num)
    {
        numPoints_ = num;
        CFREE(points_);
        points_ = (value_type*)CMALLOC(sizeof(value_type)*numPoints_);
    }

    template<class T, s32 NumMaxLeaves>
    void KDTree<T, NumMaxLeaves>::build()
    {
        nodes_.clear();
        if(numPoints_<=0){
            return;
        }
        s32 numNodes = 2 * math::float2S32((f32)numPoints_/MaxLeafElements);
        nodes_.reserve(numNodes);

        Vector3 bmin = points_[0].getPosition();
        Vector3 bmax = points_[0].getPosition();
        for(s32 i=1; i<numPoints_; ++i){
            bmin = minimum(bmin, points_[i].getPosition());
            bmax = maximum(bmax, points_[i].getPosition());
        }
        Node root;
        root.setLeaf(0, numPoints_);
        nodes_.push_back(root);
        recursiveConstruct(0, 0, numPoints_, bmin, bmax);
    }

    template<class T, s32 NumMaxLeaves>
    void KDTree<T, NumMaxLeaves>::recursiveConstruct(s32 nodeIndex, s32 offset, s32 n, const Vector3& bmin, const Vector3& bmax)
    {
        if(n<=MaxLeafElements){
            Node& node = nodes_[nodeIndex];
            node.setLeaf(offset, static_cast<u16>(n));
            return;
        }

        s32 axis = 0;
        f32 maxSize = bmax[0]-bmin[0];
        CompareFunc func_comp = compare_x;
        SearchFunc func_search = search_x;
        for(s32 i=1; i<3; ++i){
            f32 s = bmax[i]-bmin[i];
            if(maxSize<s){
                maxSize = s;
                axis = i;
                func_comp = (1==axis)? compare_y : compare_z;
                func_search = (1==axis)? search_y : search_z;
            }
        }

        value_type* points = points_ + offset;

        //sort
        lcore::introsort(n, points, func_comp);

        //pivot
        LASSERT(0<=axis && axis<3);

        f32 split = (bmax[axis]+bmin[axis])*0.5f;
        s32 mid = search(split, n, points, func_search);
        if(mid<=0 || (n-1)<=mid){
            Node& node = nodes_[nodeIndex];
            node.setLeaf(offset, static_cast<u16>(n));
            return;
        }

        s32 leftChild = nodes_.size();
        s32 rightChild = leftChild+1;
        {
            Node node;
            node.setLeaf(0,0);
            nodes_.push_back(node);
            nodes_.push_back(node);
        }
        nodes_[nodeIndex].setInner(split, static_cast<u16>(axis), leftChild);

        Vector3 tmax = bmax;
        tmax[axis] = split;
        recursiveConstruct(leftChild, offset, mid, bmin, tmax);

        Vector3 tmin = bmin;
        tmin[axis] = split;
        recursiveConstruct(rightChild, offset+mid, n-mid, tmin, bmax);
    }

    template<class T, s32 NumMaxLeaves>
    void KDTree<T, NumMaxLeaves>::gather(Nearests& nearests, s32 nodeIndex)
    {
        Node& node = nodes_[nodeIndex];
        if(node.isLeaf()){
            if(node.getNum()<=0){
                return;
            }
            s32 end = node.getIndex() + node.getNum();
            for(s32 i=node.getIndex(); i<end; ++i){
                value_type& value = points_[i];
                f32 sqrd = lmath::distanceSqr(value.getPosition(), nearests.position_);
                nearests.add(sqrd, &value);
            }
            return;
        }
        f32 sd = node.signedDistance(nearests.position_);
        f32 sd2 = sd*sd;
        s32 leftChild = node.getLeftChild();
        s32 rightChild = leftChild+1;

        if(0.0f<=sd){
            gather(nearests, rightChild); //Right
            if(sd2<nearests.maxSqrDistance()){
                gather(nearests, leftChild); //Left
            }
        }else{
            gather(nearests, leftChild); //Left
            if(sd2<nearests.maxSqrDistance()){
                gather(nearests, rightChild); //Right
            }
        }
    }

    template<class T, s32 NumMaxLeaves>
    s32 KDTree<T, NumMaxLeaves>::search(f32 split, s32 num, const value_type* points, SearchFunc comp)
    {
        s32 mid;
        s32 left = 0;
        s32 right = num-1;
        while(left<=right){
            mid = (left+right)>>1;
            s32 ret = comp(split, points[mid]);
            if(ret<0){
                right = mid-1;
            }else if(0<ret){
                left = mid+1;
            }else{
                while(0<mid){
                    if(0==comp(split, points[mid-1])){
                        --mid;
                    }else{
                        break;
                    }
                }
                return mid;
            }
        }
        return left;
    }
}
#endif //INC_LRENDER_KDTREE_H__
