/**
@file IOIK.cpp
@author t-sakai
@date 2010/07/29 create
*/
#include "IK.h"
#include <lgraphics/io/IOUtil.h>

#include "IOIK.h"

namespace ltools
{
    using namespace lcore;
    using namespace lgraphics;

    namespace
    {
        struct IKHeader
        {
            u32 effectorBone_;
            u32 targetBone_;
            u32 length_;
            u32 numIterations_;
            f32 weight_;
        };
    }

    bool IOIK::read(lcore::istream& is, IKChain** ppIKChains, u32& numChains)
    {
        LASSERT(ppIKChains != NULL);

        io::read(is, numChains);

        *ppIKChains = LIME_NEW IKChain[numChains];

        IKChain *ikChains = *ppIKChains;
        IKHeader header;
        for(u32 i=0; i<numChains; ++i){
            io::read(is, header);
            if(header.length_>IKChain::MAX_LENGTH){
                return false;
            }

            ikChains[i].set(header.length_, header.effectorBone_, header.targetBone_, header.numIterations_, header.weight_);
        }
        return true;

    }

    bool IOIK::write(lcore::ostream& os, IKChain* ikChains, u32 numChains)
    {
        LASSERT(ikChains != NULL);

        io::write(os, numChains);

        IKHeader header;
        for(u32 i=0; i<numChains; ++i){

            header.effectorBone_ = ikChains[i].getEffectorIndex();
            header.targetBone_ = ikChains[i].getTargetIndex();
            header.length_ = ikChains[i].getLength();
            header.numIterations_ = ikChains[i].getNumIterations();
            header.weight_ = ikChains[i].getWeight();

            io::write(os, header);
        }
        return true;
    }
}
