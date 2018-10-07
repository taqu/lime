#ifndef INC_LCORE_RECTPACK_H_
#define INC_LCORE_RECTPACK_H_
/**
@file RectPack.h
@author t-sakai
@date 2017/01/14 create
*/
#include "lcore.h"
#include "Sort.h"

namespace lcore
{
    struct RectPackRect
    {
        s32 id_;
        u16 x_;
        u16 y_;
        u16 w_;
        u16 h_;

        const s32& ID() const{ return id_;}
        s32& ID(){ return id_;}
        const u16& X() const{ return x_;}
        u16& X(){ return x_;}
        const u16& Y() const{ return y_;}
        u16& Y(){ return y_;}
        const u16& W() const{ return w_;}
        u16& W(){ return w_;}
        const u16& H() const{ return h_;}
        u16& H(){ return h_;}
    };

    class RectPack
    {
    public:
        struct Node
        {
            s32 left_;
            u16 width_;
            u16 height_;
            s32 id_;

            void initialize(u16 width, u16 height)
            {
                width_ = width;
                height_ = height;
            }

            void clear()
            {
                left_ = -1;
                id_ = -1;
            }

            bool isLeaf() const
            {
                return left_<0;
            }

            s32 left() const
            {
                return left_;
            }

            /**
            @return -1:¬‚³‚·‚¬‚éA0:fitA1:•ïŠÜ
            */
            s32 fit(u16 width, u16 height) const;
        };

        struct Context
        {
            s32 free_;
            Node root_;
            Node* nodes_;
            u16 width_;
            u16 height_;
            s32 pop();
        };

        static void initialize(Context& context, u16 width, u16 height, s32 numNodes, Node* nodes);
        template<class T>
        static bool pack(Context& context, s32 numRects, T* rects);
    private:
        RectPack();
        RectPack(const RectPack&);
        RectPack& operator=(const RectPack&);

        template<class T>
        static bool insert(
            Context& context,
            Node& node,
            u16 x,
            u16 y,
            u16 width,
            u16 height,
            s32 id,
            T* rects);

        template<class T>
        static bool less_rect(const T& r0, const T& r1);

    };

    template<class T>
    bool RectPack::less_rect(const T& r0, const T& r1)
    {
        s32 s0 = r0.W()*r0.H();
        s32 s1 = r1.W()*r1.H();
        if(s0==s1){
            return (r0.W()==r1.W())? (r1.H()<r0.H()) : (r1.W()<r0.W());
        }
        return (s1<s0);
    }

    template<class T>
    bool RectPack::pack(Context& context, s32 numRects, T* rects)
    {
        if(numRects<=0){
            return true;
        }
        introsort(numRects, rects, RectPack::less_rect<T>);
        for(s32 i=0; i<numRects; ++i){
            rects[i].x_ = static_cast<u16>(-1);
            rects[i].y_ = static_cast<u16>(-1);
        }
        context.root_.clear();
        context.root_.initialize(context.width_, context.height_);
        bool result = true;
        for(s32 i=0; i<numRects; ++i){
            result &= insert(context, context.root_, 0, 0, context.width_, context.height_, i, rects);
        }
        return result;
    }

    template<class T>
    bool RectPack::insert(
        Context& context,
        Node& node,
        u16 x,
        u16 y,
        u16 width,
        u16 height,
        s32 id,
        T* rects)
    {
        T& rect = rects[id];

        if(node.isLeaf()){
            //‚·‚Å‚É–„‚Ü‚Á‚Ä‚¢‚é‚©
            if(0<=node.id_){
                return false;
            }

            switch(node.fit(rect.W(), rect.H()))
            {
            case 0:
                node.id_ = id;
                rect.X() = x;
                rect.Y() = y;
                return true;

            case 1:
                break;

            default:
                return false;
            }
            node.left_ = context.pop();
            s32 left = node.left();
            Node& leftNode = context.nodes_[left];
            Node& rightNode = context.nodes_[left+1];
            leftNode.clear();
            rightNode.clear();

            //‘å‚«‚¢Ž²‚ð•ªŠ„
            s32 dw = static_cast<s32>(width) - rect.W();
            s32 dh = static_cast<s32>(height) - rect.H();
            if(dh<dw){
                leftNode.initialize(rect.W(), height);
                rightNode.initialize(width-rect.W(), height);
            }else{
                leftNode.initialize(width, rect.H());
                rightNode.initialize(width, height-rect.H());
            }
        }
        s32 left = node.left();
        Node& leftNode = context.nodes_[left];
        Node& rightNode = context.nodes_[left+1];

        if(leftNode.width_ != rightNode.width_){
            if(insert(context, leftNode, x, y, leftNode.width_, leftNode.height_, id, rects)){
                return true;
            }
            return insert(context, rightNode, x+leftNode.width_, y, rightNode.width_, rightNode.height_, id, rects);

        } else{
            if(insert(context, leftNode, x, y, leftNode.width_, leftNode.height_, id, rects)){
                return true;
            }
            return insert(context, rightNode, x, y+leftNode.height_, rightNode.width_, rightNode.height_, id, rects);
        }
    }
}
#endif //INC_LCORE_RECTPACK_H_
