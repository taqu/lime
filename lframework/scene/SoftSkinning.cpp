/**
@file SoftSkinning.cpp
@author t-sakai
@date 2010/07/29 create
*/
#include <lmath/Vector3.h>
#include <lmath/Matrix43.h>

#include "SoftSkinning.h"
#include <lgraphics/api/VertexDeclarationRef.h>

using namespace lmath;

namespace lscene
{
    SoftSkinning::SoftSkinning()
    {
        clear();
    }

    SoftSkinning::~SoftSkinning()
    {
    }

#if defined(LIME_GLES1)
    void SoftSkinning::initialize(VertexDeclarationRef& decl, VertexBufferUPRef& src)
    {
        createDecl(decl);

        src_ = src;
        dst_ = VertexBufferUP::create(src_.getVertexSize(), src_.getVertexNum(), src_.getStream(), src_.getOffset());

        void *s = NULL;
        void *d = NULL;
        src_.lock(&s);
        dst_.lock(&d);

        lcore::memcpy(d, s, src_.getVertexNum() * src_.getVertexSize());

        src_.unlock();
        dst_.unlock();


        updatePack_.stride_ = src_.getVertexSize();
        updatePack_.num_ = src_.getVertexNum();
    }
#endif

#if defined(LIME_DX9)
    void SoftSkinning::initialize(VertexDeclarationRef& decl, VertexBufferRef& src)
    {
        createDecl(decl);

        src_ = VertexBufferUP::create(src.getVertexSize(), src.getVertexNum());

        u32 size = src.getVertexSize() * src.getVertexNum();
        void *s = NULL;
        void *d = NULL;
        src.lock(0, size, &s);
        src_.lock(&d);

        lcore::memcpy(d, s, size);

        src.unlock();
        src_.unlock();


        updatePack_.stride_ = src.getVertexSize();
        updatePack_.num_ = src.getVertexNum();
    }
#endif

    void SoftSkinning::update(lmath::Matrix43* matrices, VertexBufferRef& target)
    {
#if defined(LIME_DX9)
        LASSERT(matrices != NULL);
        //LASSERT(target.getVertexSize() == src_.getVertexSize());
        //LASSERT(target.getVertexNum() == src_.getVertexNum());

        u32 size = target.getVertexSize() * target.getVertexNum();

        target.lock(0, size, (void**)&(updatePack_.dst_), Lock_Discard);
        src_.lock((void**)&(updatePack_.src_));

        updatePack_.matrices_ = matrices;

        func_(updatePack_);

        target.unlock();
        src_.unlock();

#elif defined(LIME_GLES1)
        LASSERT(matrices != NULL);
        LASSERT(target.getVertexSize() == src_.getVertexSize());
        LASSERT(target.getVertexNum() == src_.getVertexNum());

        src_.lock((void**)&(updatePack_.src_));
        dst_.lock((void**)&(updatePack_.dst_));

        updatePack_.matrices_ = matrices;

        func_(updatePack_);

        target.blit(updatePack_.dst_, true);

        src_.unlock();
        dst_.unlock();
#endif
    }

    void SoftSkinning::clear()
    {
        func_ = UpdateDummy;
    }

    void SoftSkinning::createDecl(VertexDeclarationRef& decl)
    {
        lgraphics::VertexElement decls[VertexDeclarationRef::MAX_ELEMENTS];
        decl.getDecl(decls);
        u32 num = decl.getNumElements() - 1; //最後は終端要素

        bool pos = false;
        bool normal = false;

        for(u32 i=0; i<num; ++i){
            switch(decls[i].usage_)
            {
            case DeclUsage_Position:
                pos = true;
                updatePack_.offsetPosition_ = decls[i].offset_;
                break;

            case DeclUsage_BlendWeight: //とりあえず、floatx1対応
                updatePack_.offsetWeights_ = decls[i].offset_;
                break;

            case DeclUsage_BlendIndicies: //とりあえず、1バイトx4インデックス対応
                updatePack_.offsetIndices_ = decls[i].offset_;
                break;

            case DeclUsage_Normal:
                updatePack_.offsetNormal_ = decls[i].offset_;
                normal = true;
                break;

            default:
                break;
            };
        }
        if(pos && normal){
            func_ = UpdatePosNormal;
        }else if(pos){
            func_ = UpdatePosition;
        }else if(normal){
            func_ = UpdateNormal;
        }else{
            func_ = UpdateDummy;
        }

    }


    void SoftSkinning::UpdatePosition(UpdatePack /*pack*/)
    {
    }

    void SoftSkinning::UpdateNormal(UpdatePack /*pack*/)
    {
    }

    void SoftSkinning::UpdatePosNormal(UpdatePack pack)
    {
        const u8* spos = pack.src_ + pack.offsetPosition_;
        u8* dpos = pack.dst_ + pack.offsetPosition_;

        const u8* snormal = pack.src_ + pack.offsetNormal_;
        u8* dnormal = pack.dst_ + pack.offsetNormal_;

        const u8* sindices = pack.src_ + pack.offsetIndices_;
        //u8* dindices = pack.dst_ + pack.offsetPosition_;

        const u8* sw = pack.src_ + pack.offsetWeights_;
        //u8* dw = pack.dst_ + pack.offsetPosition_;

        f32 w;
        Vector3 tmp0, tmp1;
        for(u32 i=0; i<pack.num_; ++i){
            const Vector3& sp = *(reinterpret_cast<const Vector3*>(spos));
            Vector3& dp = *(reinterpret_cast<Vector3*>(dpos));

            const Vector3& sn = *(reinterpret_cast<const Vector3*>(snormal));
            Vector3& dn = *(reinterpret_cast<Vector3*>(dnormal));

            Matrix43& m0 = pack.matrices_[sindices[0]];
            Matrix43& m1 = pack.matrices_[sindices[1]];

            w = *(reinterpret_cast<const f32*>(sw));

            //変形
            tmp0.mul(sp, m0);
            tmp1.mul(sp, m1);

            dp.lerp(tmp1, tmp0, w);
            
            tmp0.mul33(sn, m0);
            tmp1.mul33(sn, m1);
            dn.lerp(tmp1, tmp0, w);

            spos += pack.stride_;
            dpos += pack.stride_;
            snormal += pack.stride_;
            dnormal += pack.stride_;

            sindices += pack.stride_;
            sw += pack.stride_;
        }
    }

    void SoftSkinning::UpdateDummy(UpdatePack /*pack*/)
    {
    }
}
