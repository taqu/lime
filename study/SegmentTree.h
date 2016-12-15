#include <vector>

namespace lmath
{
    typedef __int16 s16;
    typedef __int32 s32;

    typedef unsigned __int16 u16;
    typedef unsigned __int32 u32;
    typedef float f32;

    struct Segment
    {
        static const u16 Invalid = 0xFFFFU;

        Segment()
        {}

        Segment(f32 sx, f32 ex)
            :sx_(sx)
            ,ex_(ex)
        {}

        f32 sx_;
        f32 ex_;
    };

    class SegmentTree
    {
    public:
        static const u16 Invalid = 0xFFFFU;

        SegmentTree();
        ~SegmentTree();

        void clear();
        void add(Segment& segment);

        void construct();

        void debugPrint();
        void debugPrint(u16 node, s32 level);
    private:
        struct Node
        {
            typedef std::vector<u16> SegmentVector;

            f32 key_;
            u16 left_;
            u16 right_;
            f32 min_;
            f32 max_;
            SegmentVector segments_;
        };

        typedef std::vector<Segment> SegmentVector;
        typedef std::vector<Node> NodeVector;

        static bool isSmaller(Segment& v0, Segment& v1);

        void sortSegments(s32 start, s32 end);
        u16 searchTree(u16 start, u16 end);
        void segmentInsertion(u16 n, u16 seg);

        u16 popNode();

        SegmentTree(const SegmentTree&);
        SegmentTree& operator=(const SegmentTree&);

        SegmentVector segments_;
        NodeVector nodes_;

        u16 numUseNode_;
        u16 root_;
    };
}
