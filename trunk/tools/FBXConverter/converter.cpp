/**
@file converter.cpp
@author t-sakai
@date 2012/10/30 create

*/
#include "converter.h"
#include <fstream>

#include <lmath/lmath.h>

namespace load
{
namespace
{
    template<class T>
    void write(std::ostream& output, const T& v)
    {
        output.write((const Char*)&v, sizeof(T));
    }

    template<class T>
    void write(std::ostream& output, const T& v, u32 size)
    {
        output.write((const Char*)&v, size);
    }

    template<class T>
    void writeBuffer(std::ostream& output, const T* v, u32 size)
    {
        output.write((const Char*)v, size);
    }

    void writeName(std::ostream& output, const Char* name)
    {
        while(*name != '\0'){
            output.write(name, 1);
            ++name;
        }
        output.write(name, 1);
    }

    void copyName(Char* dst, const Char* src)
    {
        u32 len = lcore::strlen(src);
        len = lcore::minimum(len, MaxNameLength);
        lcore::strncpy(dst, MaxNameSize, src, len);
        dst[len]='\0';
    }

    void correctUV(f32& u0, f32& u1)
    {
        f32 tu0, tu1;
        tu0 = lcore::absolute(u1 - u0);
        tu1 = lcore::absolute(u1 - u0 - 1.0f);
        if(tu0>tu1){
            u1 -= 1.0f;
        }
    }

    u32 colorToRGBA(FbxColor& color)
    {
        u8 r = static_cast<u8>(color.mRed * 255);
        u8 g = static_cast<u8>(color.mGreen * 255);
        u8 b = static_cast<u8>(color.mBlue * 255);
        u8 a = static_cast<u8>(color.mAlpha * 255);

        return lcore::getRGBA(a, r, g, b);
    }

    void calcTangent(
        lmath::Vector3& tangent,
        const lmath::Vector3& p0,
        const lmath::Vector3& p1,
        const lmath::Vector3& p2,
        const lmath::Vector2& tuv0,
        const lmath::Vector2& tuv1,
        const lmath::Vector2& tuv2)
    {

        lmath::Vector2 uv0 = tuv0;
        lmath::Vector2 uv1 = tuv1;
        lmath::Vector2 uv2 = tuv2;

        correctUV(uv0.x_, uv1.x_);
        correctUV(uv1.x_, uv2.x_);

        correctUV(uv0.y_, uv1.y_);
        correctUV(uv1.y_, uv2.y_);

        lmath::Vector3 t0[3] =
        {
            lmath::Vector3(p0.x_, uv0.x_, uv0.y_),
            lmath::Vector3(p0.y_, uv0.x_, uv0.y_),
            lmath::Vector3(p0.z_, uv0.x_, uv0.y_),
        };

        lmath::Vector3 t1[3] =
        {
            lmath::Vector3(p1.x_, uv1.x_, uv1.y_),
            lmath::Vector3(p1.y_, uv1.x_, uv1.y_),
            lmath::Vector3(p1.z_, uv1.x_, uv1.y_),
        };


        lmath::Vector3 t2[3] =
        {
            lmath::Vector3(p2.x_, uv2.x_, uv2.y_),
            lmath::Vector3(p2.y_, uv2.x_, uv2.y_),
            lmath::Vector3(p2.z_, uv2.x_, uv2.y_),
        };


        lmath::Vector3 d0, d1, p;
        for(s32 i=0; i<3; ++i){
            d0.sub(t1[i], t0[i]);
            d1.sub(t2[i], t1[i]);
            p.cross(d0, d1);

            if(lmath::isEqual(p.x_, 0.0f)){
                tangent.set(0.0f, 0.0f, 0.0f);
                return;
            }
            tangent[i] = -p.y_ / p.x_;
        }

        tangent.normalize();
    }

    void extractFileName(Char* filename, s32 size, const Char* filepath)
    {
        const Char* name = filepath;
        s32 len = lcore::strlen(filepath);
        for(s32 i=len-1; 0<=i; --i){
            if(filepath[i] == '\\' || filepath[i] == '/'){
                name = filepath + i + 1;
                break;
            }
        }

        len = lcore::strlen(name);
        len = lcore::minimum(len, size-1);
        for(s32 i=0; i<len; ++i){
            filename[i] = name[i];
        }
        filename[len] = '\0';
    }
}


    //-----------------------------------------------------------------------------
    void JointAnimationCVT::add(u32 frame, const lmath::Vector3& translation, const lmath::Quaternion& rotation)
    {
        if(poses_.size()<=0){
            JointPoseWithFrame pose;
            pose.frameNo_ = frame;
            pose.translation_ = translation;
            pose.rotation_ = rotation;
            poses_.push_back(pose);
            return;
        }

        u32 index = poses_.size();
        for(u32 i=0; i<poses_.size(); ++i){
            if(poses_[i].frameNo_ == frame){
                poses_[i].translation_ = translation;
                poses_[i].rotation_ = rotation;
                return;
            }

            if(frame<poses_[i].frameNo_){
                index = i+1;
                break;
            }
        }
        JointPoseWithFrame pose;
        poses_.push_back(pose);

        for(u32 i=index+1; i<poses_.size(); ++i){
            poses_[i] = poses_[i-1];
        }

        poses_[index].frameNo_ = frame;
        poses_[index].translation_ = translation;
        poses_[index].rotation_ = rotation;
    }

    //-----------------------------------------------------------------------------
    void AnimationClipCVT::calcLastFrame()
    {
        u32 lastFrame = 0;
        for(u32 i=0; i<jointAnims_.size(); ++i){
            for(u32 j=0; j<jointAnims_[i].poses_.size(); ++j){
                u32 t = jointAnims_[i].poses_[j].frameNo_;
                if(lastFrame<t){
                    lastFrame = t;
                }
            }
        }
        lastFrame_ = static_cast<f32>(lastFrame);
    }

    //-----------------------------------------------------------------------------
    Manager::Manager()
    {
        manager_ = FbxManager::Create();

        if(NULL != manager_){
            FbxIOSettings *ios = FbxIOSettings::Create(manager_, IOSROOT);
            manager_->SetIOSettings(ios);

            ios->SetBoolProp(IMP_FBX_MATERIAL, true);
            ios->SetBoolProp(IMP_FBX_TEXTURE, true);
            ios->SetBoolProp(IMP_FBX_LINK, false);
            ios->SetBoolProp(IMP_FBX_SHAPE, false);
            ios->SetBoolProp(IMP_FBX_GOBO, false);
            ios->SetBoolProp(IMP_FBX_ANIMATION, true);
            ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
        }

    }

    Manager::~Manager()
    {
        if(NULL != manager_){
            manager_->Destroy();
            manager_ = NULL;
        }
    }

    bool Manager::initialize(Importer& importer, const char* filename)
    {
        LASSERT(NULL != filename);
        FbxImporter* imp = FbxImporter::Create(manager_, "");
        if(NULL == imp){
            return false;
        }

        if(!imp->Initialize(filename, -1, manager_->GetIOSettings())){
            imp->Destroy();
            return false;
        }

        Importer tmp(imp);
        importer.swap(tmp);
        return true;
    }

    bool Manager::import(Scene& scene, Importer& importer)
    {
        FbxScene* sc = FbxScene::Create(manager_, "scene");
        if(NULL == sc){
            return false;
        }

        if(!importer.importer_->Import(sc)){
            sc->Destroy();
            return false;
        }

        Scene tmp(sc);
        scene.swap(tmp);
        return true;
    }


    Importer::Importer()
        :importer_(NULL)
    {
    }

    Importer::Importer(FbxImporter* importer)
        :importer_(importer)
    {
    }

    Importer::~Importer()
    {
        if(NULL != importer_){
            importer_->Destroy(true);
            importer_ = NULL;
        }
    }


    Scene::Scene()
        :scene_(NULL)
    {
    }

    Scene::Scene(FbxScene* scene)
        :scene_(scene)
    {
    }

    Scene::~Scene()
    {
        if(NULL != scene_){
            scene_->Destroy(true);
            scene_ = NULL;
        }
    }


    Converter::Converter()
    {
    }

    Converter::~Converter()
    {
    }

    bool Converter::process(FbxScene* scene)
    {
        {//テクスチャ保存
            s32 count = scene->GetTextureCount();
            for(s32 i=0; i<count; ++i){
                FbxTexture* texture = scene->GetTexture(i);
                if(false == texture->GetClassId().Is(FbxFileTexture::ClassId)){
                    continue;
                }
                FbxFileTexture* fileTexture = (FbxFileTexture*)texture;
                Texture tex;
                if(fileTexture->GetTextureUse() == FbxFileTexture::eStandard){
                    tex.type_ = TexType_Albedo;

                }else if(fileTexture->GetTextureUse() == FbxFileTexture::eShadowMap){
                    continue;
                }else if(fileTexture->GetTextureUse() == FbxFileTexture::eLightMap){
                    continue;
                }else if(fileTexture->GetTextureUse() == FbxFileTexture::eSphericalReflectionMap){
                    continue;
                }else if(fileTexture->GetTextureUse() == FbxFileTexture::eSphereReflectionMap){
                    continue;
                }else if(fileTexture->GetTextureUse() == FbxFileTexture::eBumpNormalMap){
                    tex.type_ = TexType_Normal;
                }

                extractFileName(tex.name_, MaxFileNameSize, fileTexture->GetFileName());
  
                fbxTextures_.push_back(texture);
                textures_.push_back(tex);
            }
        }

        pushMaterial(scene);

        FbxNode* root = scene->GetRootNode();

        {//スケルトン生成
            traverseSkeletonNode(root);
            for(u32 i=0; i<skeletonNodes_.size(); ++i){
                Joint joint;
                s32 index = findSkeletonNode(skeletonNodes_[i]->GetParent());
                joint.parent_ = (index<0)? InvalidNode : static_cast<u8>(index);
                joint.subjectTo_ = 0;
                joint.type_ = 0;
                joint.flag_ = 0;

                const FbxAMatrix& transform = skeletonNodes_[i]->EvaluateGlobalTransform();
                const FbxDouble4 translation = transform.GetT();
                //const FbxDouble3 local = skeletonNodes_[i]->LclTranslation;
                joint.position_.set(
                    static_cast<f32>(translation[0]),
                    static_cast<f32>(translation[1]),
                    static_cast<f32>(translation[2]));
                
                copyName(joint.name_, skeletonNodes_[i]->GetName());

                joints_.push_back(joint);
            }
        }

        for(s32 i=0; i<root->GetChildCount(); ++i){
            if(!acceptNodeAttribute(root->GetChild(i)->GetNodeAttribute())){
                continue;
            }

            pushNode(root->GetChild(i), InvalidNode);
        }

        s32 count = 0;
        for(s32 i=0; i<root->GetChildCount(); ++i){
            if(!acceptNodeAttribute(root->GetChild(i)->GetNodeAttribute())){
                continue;
            }
            traverseNode(root->GetChild(i), count);
            ++count;
        }

        traverseAnimation(scene);
        return true;
    }

    bool Converter::acceptNodeAttribute(const FbxNodeAttribute* attribute)
    {
        if(NULL == attribute){
            return true;
        }

        switch(attribute->GetAttributeType())
        {
        case FbxNodeAttribute::eSkeleton:
        case FbxNodeAttribute::eCamera:
        case FbxNodeAttribute::eLight:
            return false;
        };

        return true;
    }

    void Converter::traverseSkeletonNode(FbxNode* root)
    {
        for(s32 i=0; i<root->GetChildCount(); ++i){
            FbxNode* child = root->GetChild(i);
            const FbxNodeAttribute* attr = child->GetNodeAttribute();
            if(NULL == attr){
                continue;
            }
            if(FbxNodeAttribute::eSkeleton != attr->GetAttributeType()){
                continue;
            }
            skeletonNodes_.push_back(child);
            traverseSkeletonNode(child);
        }

        for(s32 i=0; i<root->GetChildCount(); ++i){
            FbxNode* child = root->GetChild(i);
            const FbxNodeAttribute* attr = child->GetNodeAttribute();
            if(NULL == attr){
                continue;
            }
            if(FbxNodeAttribute::eSkeleton == attr->GetAttributeType()){
                continue;
            }
            traverseSkeletonNode(child);
        }
    }

    void Converter::traverseNode(FbxNode* node, s32 index)
    {
        u8 childStartIndex = static_cast<u8>( nodes_.size() );
        for(s32 i=0; i<node->GetChildCount(); ++i){
            if(!acceptNodeAttribute(node->GetChild(i)->GetNodeAttribute())){
                continue;
            }
            pushNode(node->GetChild(i), static_cast<u8>(index));
        }

        Node& n = nodes_[index];

        n.childStartIndex_ = childStartIndex;
        n.numChildren_ = static_cast<u8>( node->GetChildCount() );

        s32 count = 0;
        for(s32 i=0; i<node->GetChildCount(); ++i){
            if(!acceptNodeAttribute(node->GetChild(i)->GetNodeAttribute())){
                continue;
            }

            traverseNode(node->GetChild(i), nodes_[index].childStartIndex_ + count);
            ++count;
        }
    }

    void Converter::pushNode(FbxNode* node, u8 parent)
    {
        Node n;
        n.index_ = static_cast<u8>( nodes_.size() );
        n.parentIndex_ = parent;
        n.childStartIndex_ = 0;
        n.numChildren_ = 0;
        n.rotationOrder_ = RotationOrder_EulerXYZ;
        n.type_ = Node::Type_Null;
        n.meshStartIndex_ = 0;
        n.numMeshes_ = 0;

        const FbxDouble3 translation = node->LclTranslation;
        const FbxDouble3 rotation = node->LclRotation;
        const FbxDouble3 scale = node->LclScaling;

        n.translation_.set(
            static_cast<f32>(translation[0]),
            static_cast<f32>(translation[1]),
            static_cast<f32>(translation[2]));

        n.rotation_.set(
            static_cast<f32>(rotation[0]) * DEG_TO_RAD,
            static_cast<f32>(rotation[1]) * DEG_TO_RAD,
            static_cast<f32>(rotation[2]) * DEG_TO_RAD);

        n.scale_.set(
            static_cast<f32>(scale[0]),
            static_cast<f32>(scale[1]),
            static_cast<f32>(scale[2]));
        FBXSDK_printf("Node %s\n", node->GetName());
        FBXSDK_printf("    translation %f, %f, %f\n", translation[0], translation[1], translation[2]);
        FBXSDK_printf("    rotation %f, %f, %f\n", rotation[0], rotation[1], rotation[2]);

        EFbxRotationOrder order;
        node->GetRotationOrder(FbxNode::eSourcePivot, order);
        switch(order)
        {
        case eEulerXYZ:
            n.rotationOrder_ = RotationOrder_EulerXYZ;
            break;
        case eEulerXZY:
            n.rotationOrder_ = RotationOrder_EulerXZY;
            break;
        case eEulerYZX:
            n.rotationOrder_ = RotationOrder_EulerYZX;
            break;
        case eEulerYXZ:
            n.rotationOrder_ = RotationOrder_EulerYXZ;
            break;
        case eEulerZXY:
            n.rotationOrder_ = RotationOrder_EulerZXY;
            break;
        case eEulerZYX:
            n.rotationOrder_ = RotationOrder_EulerZYX;
            break;
        case eSphericXYZ:
            n.rotationOrder_ = RotationOrder_SphericXYZ;
            break;
        };

        const FbxNodeAttribute* attr = node->GetNodeAttribute();

        if(NULL != attr){
            switch(attr->GetAttributeType())
            {
            case FbxNodeAttribute::eMesh:
                pushMesh(n, node);
                break;

            //case FbxNodeAttribute::eSkeleton:
            //case FbxNodeAttribute::eCamera:
            //case FbxNodeAttribute::eLight:
            //    return;
            };
        }

        nodes_.push_back(n);
    }


    const Char* Converter::getAttributeType(const FbxNodeAttribute* attr)
    {
        static const Char NoAttr[] = "No attribute type";
        static const Char Marker[] = "Marker";
        static const Char Skeleton[] = "Skeleton";
        static const Char Mesh[] = "Mesh";
        static const Char Camera[] = "Camera";
        static const Char Light[] = "Light";
        static const Char Boundary[] = "Boundary";
        static const Char OpticalMarker[] = "OpticalMarker";
        static const Char OpticalReference[] = "OpticalReference";
        static const Char CameraSwitcher[] = "CameraSwitcher";
        static const Char Null[] = "Null";
        static const Char Patch[] = "Patch";
        static const Char NURBS[] = "NURBS";
        static const Char NURBSSurface[] = "NURBS Surface";
        static const Char NURBSCurve[] = "NURBS Curve";
        static const Char TrimNURBSSurface[] = "Trim NURBS Surface";
        static const Char Unknown[] = "Unknown";

        if(NULL == attr){
            return NoAttr;
        }

        switch(attr->GetAttributeType())
        {
        case FbxNodeAttribute::eMarker: return Marker;
        case FbxNodeAttribute::eSkeleton: return Skeleton;
        case FbxNodeAttribute::eMesh: return Mesh;
        case FbxNodeAttribute::eCamera: return Camera;
        case FbxNodeAttribute::eLight: return Light;
        case FbxNodeAttribute::eBoundary: return Boundary;
        case FbxNodeAttribute::eOpticalMarker: return OpticalMarker;
        case FbxNodeAttribute::eOpticalReference: return OpticalReference;
        case FbxNodeAttribute::eCameraSwitcher: return CameraSwitcher;
        case FbxNodeAttribute::eNull: return Null;
        case FbxNodeAttribute::ePatch: return Patch;
        case FbxNodeAttribute::eNurbs: return NURBS;
        case FbxNodeAttribute::eNurbsSurface: return NURBSSurface;
        case FbxNodeAttribute::eNurbsCurve: return NURBSCurve;
        case FbxNodeAttribute::eTrimNurbsSurface: return TrimNURBSSurface;
        case FbxNodeAttribute::eUnknown: return Unknown;
        };
        return Unknown;
    }

    s16 Converter::pushGeometry(FbxNode* node)
    {
        FbxMesh* mesh = node->GetMesh();

        s16 geometryIndex = static_cast<s16>( geometries_.size() );
        geometries_.resize(geometries_.size()+1);

        GeometryCVT& geometry = geometries_[geometryIndex];
        u16 indexOffset = 0;

        s32 numPolygons = mesh->GetPolygonCount();
        u16 numIndices = 0;

        for(s32 i=0; i<numPolygons; ++i){
            s32 num = mesh->GetPolygonSize(i);
            switch(num)
            {
            case 0:
                break;

            case 1:
                {
                    u16 index = indexOffset + static_cast<u16>( mesh->GetPolygonVertex(i, 0) );
                    geometry.indices_.push_back(index);
                    geometry.indices_.push_back(index);
                    geometry.indices_.push_back(index);
                    numIndices += 3;
                }
                break;

            case 2:
                {
                    u16 index0 = indexOffset + static_cast<u16>( mesh->GetPolygonVertex(i, 0) );
                    u16 index1 = indexOffset + static_cast<u16>( mesh->GetPolygonVertex(i, 1) );
                    geometry.indices_.push_back(index0);
                    geometry.indices_.push_back(index1);
                    geometry.indices_.push_back(index1);
                    numIndices += 3;
                }
                break;

            default:
                {
                    u16 index0 = indexOffset + static_cast<u16>( mesh->GetPolygonVertex(i, 0) );
                    for(s32 j=2; j<num; ++j){
                        u16 index1 = indexOffset + static_cast<u16>( mesh->GetPolygonVertex(i, j-1) );
                        u16 index2 = indexOffset + static_cast<u16>( mesh->GetPolygonVertex(i, j) );
                        geometry.indices_.push_back(index0);
                        geometry.indices_.push_back(index1);
                        geometry.indices_.push_back(index2);
                        numIndices += 3;
                    }
                }
                break;
            };
        }

        geometry.positions_.resize(DimPosition*mesh->GetControlPointsCount());

        FbxVector4* srcVertices = mesh->GetControlPoints();
        u32 v = 0;
        for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
            geometry.positions_[v + 0] = static_cast<f32>(srcVertices[i][0]);
            geometry.positions_[v + 1] = static_cast<f32>(srcVertices[i][1]);
            geometry.positions_[v + 2] = static_cast<f32>(srcVertices[i][2]);
            v += DimPosition;
        }

        geometry.numVertices_ = geometry.positions_.size()/DimPosition;
        geometry.numIndices_ = geometry.indices_.size();

        for(int i=0; i<mesh->GetLayerCount(); ++i){
            FbxLayer* layer = mesh->GetLayer(i);
            if(NULL != layer->GetNormals()){
                getNormals(geometry, mesh, layer->GetNormals());
                continue;
            }

            if(NULL != layer->GetTangents()){
                getTangents(geometry, mesh, layer->GetTangents());
                continue;
            }

            if(NULL != layer->GetBinormals()){
                getBinormals(geometry, mesh, layer->GetBinormals());
                continue;
            }

            if(NULL != layer->GetVertexColors()){
                getColors(geometry, mesh, layer->GetVertexColors());
                continue;
            }
        }

        for(int i=0; i<mesh->GetElementUVCount(); ++i){
            getUVs(geometry, mesh, mesh->GetElementUV(i));
            break;
        }

        //スキン
        if(0<mesh->GetDeformerCount(FbxDeformer::eSkin)){
            geometry.bones_.resize(geometry.numVertices_*DimBone);
            geometry.weights_.resize(geometry.numVertices_*DimBoneWeight);

            //無効値で埋める
            lcore::memset(&geometry.bones_[0], 0xFF, sizeof(u16)*geometry.numVertices_*DimBone);
            lcore::memset(&geometry.weights_[0], 0, sizeof(u16)*geometry.numVertices_*DimBoneWeight);

            u32 count = 0;
            s32 skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);

            for(s32 i=0; i<skinCount; ++i){
                FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin);
                count += getSkinInfo(geometry, skin);
            }

            //無効なボーンは0に
            for(u32 i=0; i<geometry.bones_.size(); ++i){
                if(0xFFFFU == geometry.bones_[i]){
                    geometry.bones_[i] = 0;
                }
            }
            //LASSERT(count == geometry.numVertices_);
        }


        return geometryIndex;
    }

    void Converter::getNormals(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementNormal* elementNormal)
    {
        geometry.normals_.resize(geometry.numVertices_ * DimNormal);
        lcore::memset(&geometry.normals_[0], 0, geometry.numVertices_ * DimNormal * sizeof(f32));

        FbxLayerElement::EMappingMode mappingMode = elementNormal->GetMappingMode();
        FbxLayerElement::EReferenceMode referenceMode = elementNormal->GetReferenceMode();
        switch(mappingMode)
        {
        case FbxLayerElement::eByPolygonVertex:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector4 n = elementNormal->GetDirectArray().GetAt(count);
                            s32 index = mesh->GetPolygonVertex(i, j) * DimNormal;

                            geometry.normals_[index + 0] += static_cast<f32>(n[0]);
                            geometry.normals_[index + 1] += static_cast<f32>(n[1]);
                            geometry.normals_[index + 2] += static_cast<f32>(n[2]);
                            ++count;
                        }
                    }
                    s32 size = elementNormal->GetDirectArray().GetCount();
                    LASSERT(size == count);
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector4 n = elementNormal->GetDirectArray().GetAt( elementNormal->GetIndexArray().GetAt(count) );
                            s32 index = mesh->GetPolygonVertex(i, j) * DimNormal;

                            geometry.normals_[index + 0] += static_cast<f32>(n[0]);
                            geometry.normals_[index + 1] += static_cast<f32>(n[1]);
                            geometry.normals_[index + 2] += static_cast<f32>(n[2]);
                            ++count;
                        }
                    }
                    s32 size = elementNormal->GetIndexArray().GetCount();
                    LASSERT(size == count);
                }
                break;
            }
            break;

        case FbxLayerElement::eByControlPoint:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector4 n = elementNormal->GetDirectArray().GetAt(i);
                        s32 index = DimNormal*i;
                        geometry.normals_[index + 0] += static_cast<f32>(n[0]);
                        geometry.normals_[index + 1] += static_cast<f32>(n[1]);
                        geometry.normals_[index + 2] += static_cast<f32>(n[2]);
                    }
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector4 n = elementNormal->GetDirectArray().GetAt( elementNormal->GetIndexArray().GetAt(i) );
                        s32 index = DimNormal*i;
                        geometry.normals_[index + 0] += static_cast<f32>(n[0]);
                        geometry.normals_[index + 1] += static_cast<f32>(n[1]);
                        geometry.normals_[index + 2] += static_cast<f32>(n[2]);
                    }
                }
                break;
            }
            break;
        }

        //正規化
        for(u32 i=0; i<geometry.numVertices_; ++i){
            u32 index = DimNormal*i;
            f32 x = geometry.normals_[index + 0];
            f32 y = geometry.normals_[index + 1];
            f32 z = geometry.normals_[index + 2];
            f32 l = x*x + y*y + z*z;
            if(false == lmath::isZeroPositive(l)){
                l = -1.0f/lmath::sqrt(l);
                geometry.normals_[index + 0] *= l;
                geometry.normals_[index + 1] *= l;
                geometry.normals_[index + 2] *= l;
            }
        }
    }

    void Converter::getTangents(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementTangent* elementTangent)
    {
        geometry.tangents_.resize(geometry.numVertices_ * DimTangent);
        lcore::memset(&geometry.tangents_[0], 0, geometry.numVertices_ * DimTangent * sizeof(f32));

        FbxLayerElement::EMappingMode mappingMode = elementTangent->GetMappingMode();
        FbxLayerElement::EReferenceMode referenceMode = elementTangent->GetReferenceMode();
        switch(mappingMode)
        {
        case FbxLayerElement::eByPolygonVertex:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector4 n = elementTangent->GetDirectArray().GetAt(count);
                            s32 index = mesh->GetPolygonVertex(i, j) * DimTangent;

                            geometry.tangents_[index + 0] += static_cast<f32>(n[0]);
                            geometry.tangents_[index + 1] += static_cast<f32>(n[1]);
                            geometry.tangents_[index + 2] += static_cast<f32>(n[2]);
                            ++count;
                        }
                    }
                    s32 size = elementTangent->GetDirectArray().GetCount();
                    LASSERT(size == count);
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector4 n = elementTangent->GetDirectArray().GetAt( elementTangent->GetIndexArray().GetAt(count) );
                            s32 index = mesh->GetPolygonVertex(i, j) * DimTangent;

                            geometry.tangents_[index + 0] += static_cast<f32>(n[0]);
                            geometry.tangents_[index + 1] += static_cast<f32>(n[1]);
                            geometry.tangents_[index + 2] += static_cast<f32>(n[2]);
                            ++count;
                        }
                    }
                    s32 size = elementTangent->GetIndexArray().GetCount();
                    LASSERT(size == count);
                }
                break;
            }
            break;

        case FbxLayerElement::eByControlPoint:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector4 n = elementTangent->GetDirectArray().GetAt(i);
                        s32 index = DimTangent*i;
                        geometry.tangents_[index + 0] += static_cast<f32>(n[0]);
                        geometry.tangents_[index + 1] += static_cast<f32>(n[1]);
                        geometry.tangents_[index + 2] += static_cast<f32>(n[2]);
                    }
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector4 n = elementTangent->GetDirectArray().GetAt( elementTangent->GetIndexArray().GetAt(i) );
                        s32 index = DimTangent*i;
                        geometry.tangents_[index + 0] += static_cast<f32>(n[0]);
                        geometry.tangents_[index + 1] += static_cast<f32>(n[1]);
                        geometry.tangents_[index + 2] += static_cast<f32>(n[2]);
                    }
                }
                break;
            }
            break;
        }

        //正規化
        for(u32 i=0; i<geometry.numVertices_; ++i){
            u32 index = DimTangent*i;
            f32 x = geometry.tangents_[index + 0];
            f32 y = geometry.tangents_[index + 1];
            f32 z = geometry.tangents_[index + 2];
            f32 l = x*x + y*y + z*z;
            if(false == lmath::isZeroPositive(l)){
                l = -1.0f/lmath::sqrt(l);
                geometry.tangents_[index + 0] *= l;
                geometry.tangents_[index + 1] *= l;
                geometry.tangents_[index + 2] *= l;
            }
        }
    }

    void Converter::getBinormals(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementBinormal* elementBinormal)
    {
        geometry.binormals_.resize(geometry.numVertices_ * DimBinormal);
        lcore::memset(&geometry.binormals_[0], 0, geometry.numVertices_ * DimBinormal * sizeof(f32));

        FbxLayerElement::EMappingMode mappingMode = elementBinormal->GetMappingMode();
        FbxLayerElement::EReferenceMode referenceMode = elementBinormal->GetReferenceMode();
        switch(mappingMode)
        {
        case FbxLayerElement::eByPolygonVertex:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector4 n = elementBinormal->GetDirectArray().GetAt(count);
                            s32 index = mesh->GetPolygonVertex(i, j) * DimBinormal;

                            geometry.binormals_[index + 0] += static_cast<f32>(n[0]);
                            geometry.binormals_[index + 1] += static_cast<f32>(n[1]);
                            geometry.binormals_[index + 2] += static_cast<f32>(n[2]);
                            ++count;
                        }
                    }
                    s32 size = elementBinormal->GetDirectArray().GetCount();
                    LASSERT(size == count);
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector4 n = elementBinormal->GetDirectArray().GetAt( elementBinormal->GetIndexArray().GetAt(count) );
                            s32 index = mesh->GetPolygonVertex(i, j) * DimBinormal;

                            geometry.binormals_[index + 0] += static_cast<f32>(n[0]);
                            geometry.binormals_[index + 1] += static_cast<f32>(n[1]);
                            geometry.binormals_[index + 2] += static_cast<f32>(n[2]);
                            ++count;
                        }
                    }
                    s32 size = elementBinormal->GetIndexArray().GetCount();
                    LASSERT(size == count);
                }
                break;
            }
            break;

        case FbxLayerElement::eByControlPoint:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector4 n = elementBinormal->GetDirectArray().GetAt(i);
                        s32 index = DimBinormal*i;
                        geometry.binormals_[index + 0] += static_cast<f32>(n[0]);
                        geometry.binormals_[index + 1] += static_cast<f32>(n[1]);
                        geometry.binormals_[index + 2] += static_cast<f32>(n[2]);
                    }
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector4 n = elementBinormal->GetDirectArray().GetAt( elementBinormal->GetIndexArray().GetAt(i) );
                        s32 index = DimBinormal*i;
                        geometry.binormals_[index + 0] += static_cast<f32>(n[0]);
                        geometry.binormals_[index + 1] += static_cast<f32>(n[1]);
                        geometry.binormals_[index + 2] += static_cast<f32>(n[2]);
                    }
                }
                break;
            }
            break;
        }

        //正規化
        for(u32 i=0; i<geometry.numVertices_; ++i){
            u32 index = DimBinormal*i;
            f32 x = geometry.binormals_[index + 0];
            f32 y = geometry.binormals_[index + 1];
            f32 z = geometry.binormals_[index + 2];
            f32 l = x*x + y*y + z*z;
            if(false == lmath::isZeroPositive(l)){
                l = -1.0f/lmath::sqrt(l);
                geometry.binormals_[index + 0] *= l;
                geometry.binormals_[index + 1] *= l;
                geometry.binormals_[index + 2] *= l;
            }
        }
    }

    void Converter::getColors(GeometryCVT& geometry, FbxMesh* mesh, FbxLayerElementVertexColor* elementColor)
    {
        geometry.colors_.resize(geometry.numVertices_ * DimColor);
        lcore::memset(&geometry.colors_[0], 0, geometry.numVertices_ * DimColor * sizeof(u32));

        FbxLayerElement::EMappingMode mappingMode = elementColor->GetMappingMode();
        FbxLayerElement::EReferenceMode referenceMode = elementColor->GetReferenceMode();
        switch(mappingMode)
        {
        case FbxLayerElement::eByPolygonVertex:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxColor color = elementColor->GetDirectArray().GetAt(count);
                            s32 index = mesh->GetPolygonVertex(i, j) * DimColor;
                            geometry.colors_[index] = colorToRGBA(color);
                            ++count;
                        }
                    }
                    s32 size = elementColor->GetDirectArray().GetCount();
                    LASSERT(size == count);
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxColor color = elementColor->GetDirectArray().GetAt( elementColor->GetIndexArray().GetAt(count) );
                            s32 index = mesh->GetPolygonVertex(i, j) * DimColor;

                            geometry.colors_[index] = colorToRGBA(color);
                            ++count;
                        }
                    }
                    s32 size = elementColor->GetIndexArray().GetCount();
                    LASSERT(size == count);
                }
                break;
            }
            break;

        case FbxLayerElement::eByControlPoint:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxColor color = elementColor->GetDirectArray().GetAt(i);
                        s32 index = DimColor*i;
                        geometry.colors_[index] = colorToRGBA(color);
                    }
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxColor color = elementColor->GetDirectArray().GetAt( elementColor->GetIndexArray().GetAt(i) );
                        s32 index = DimColor*i;
                        geometry.colors_[index] = colorToRGBA(color);
                    }
                }
                break;
            }
            break;
        }
    }

    void Converter::getUVs(GeometryCVT& geometry, FbxMesh* mesh, FbxGeometryElementUV* elementUV)
    {
        geometry.texcoords_.resize(geometry.numVertices_ * DimUV);
        lcore::memset(&geometry.texcoords_[0], 0, geometry.numVertices_ * DimUV * sizeof(u16));

        FbxLayerElement::EMappingMode mappingMode = elementUV->GetMappingMode();
        FbxLayerElement::EReferenceMode referenceMode = elementUV->GetReferenceMode();
        switch(mappingMode)
        {
        case FbxLayerElement::eByPolygonVertex:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector2 uv = elementUV->GetDirectArray().GetAt(count);
                            s32 index = mesh->GetPolygonVertex(i, j) * DimUV;

                            geometry.texcoords_[index + 0] = lcore::toBinary16Float( static_cast<f32>(uv[0]) );
                            geometry.texcoords_[index + 1] = lcore::toBinary16Float( static_cast<f32>(uv[1]) );
                            ++count;
                        }
                    }
                    s32 size = elementUV->GetDirectArray().GetCount();
                    LASSERT(size == count);
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    s32 count = 0;
                    s32 numPolygons = mesh->GetPolygonCount();
                    for(s32 i=0; i<numPolygons; ++i){
                        s32 num = mesh->GetPolygonSize(i);
                        for(s32 j=0; j<num; ++j){
                            FbxVector2 uv = elementUV->GetDirectArray().GetAt( elementUV->GetIndexArray().GetAt(count) );
                            s32 index = mesh->GetPolygonVertex(i, j) * DimUV;

                            geometry.texcoords_[index + 0] = lcore::toBinary16Float( static_cast<f32>(uv[0]) );
                            geometry.texcoords_[index + 1] = lcore::toBinary16Float( static_cast<f32>(uv[1]) );
                            ++count;
                        }
                    }
                    s32 size = elementUV->GetIndexArray().GetCount();
                    LASSERT(size == count);
                }
                break;
            }
            break;

        case FbxLayerElement::eByControlPoint:
            switch(referenceMode)
            {
            case FbxLayerElement::eDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector2 uv = elementUV->GetDirectArray().GetAt(i);
                        s32 index = DimUV*i;
                        geometry.texcoords_[index + 0] = lcore::toBinary16Float( static_cast<f32>(uv[0]) );
                        geometry.texcoords_[index + 1] = lcore::toBinary16Float( static_cast<f32>(uv[1]) );
                    }
                }
                break;

            case FbxLayerElement::eIndexToDirect:
                {
                    for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                        FbxVector2 uv = elementUV->GetDirectArray().GetAt( elementUV->GetIndexArray().GetAt(i) );
                        s32 index = DimUV*i;
                        geometry.texcoords_[index + 0] = lcore::toBinary16Float( static_cast<f32>(uv[0]) );
                        geometry.texcoords_[index + 1] = lcore::toBinary16Float( static_cast<f32>(uv[1]) );
                    }
                }
                break;
            }
            break;
        }
    }

    u32 Converter::getSkinInfo(GeometryCVT& geometry, FbxSkin* skin)
    {
        u32 count = 0;
        for(s32 i=0; i<skin->GetClusterCount(); ++i){
            FbxCluster* cluster = skin->GetCluster(i);

            s32* indices = cluster->GetControlPointIndices();
            f64* weights = cluster->GetControlPointWeights();
            for(s32 j=0; j<cluster->GetControlPointIndicesCount(); ++j){
                s32 index = indices[j] * DimBone;

                if(geometry.bones_[index] != 0xFFFFU){
                    ++index;
                    if(geometry.bones_[index] != 0xFFFFU){
                        continue;
                    }
                }
                geometry.bones_[index] = static_cast<u16>(i);
                geometry.weights_[index] = lcore::toBinary16Float( static_cast<f32>(weights[j]) );
                ++count;
            }
        }
        return count;
    }

    void Converter::pushMesh(Node& dst, FbxNode* src)
    {
        FbxMesh* mesh = src->GetMesh();

        s16 geometryIndex = pushGeometry(src);
        GeometryCVT& geometry = geometries_[geometryIndex];

        int meshStart = meshes_.size();
        int numMeshes = 0;

        int numMaterialLayers = 0;
        for(int i=0; i<mesh->GetLayerCount(); ++i){
            FbxLayer* layer = mesh->GetLayer(i);
            if(NULL != layer->GetMaterials()){
                ++numMaterialLayers;
            }
        }

        if(0<numMaterialLayers){
            //マテリアル毎にインデックス配列をリマップ
            U16Vector* indexVectors = LIME_NEW U16Vector[materials_.size()];

            for(int i=0; i<mesh->GetLayerCount(); ++i){
                FbxLayer* layer = mesh->GetLayer(i);
                if(NULL == layer->GetMaterials()){
                    continue;
                }
                FbxLayerElementMaterial* elementMaterial = layer->GetMaterials();
                FbxLayerElement::EMappingMode mappingMode = elementMaterial->GetMappingMode();
                FbxLayerElement::EReferenceMode referenceMode = elementMaterial->GetReferenceMode();
                if(mappingMode != FbxLayerElement::eByPolygon || referenceMode != FbxLayerElement::eIndexToDirect){
                    continue;
                }

                FbxLayerElementArrayTemplate<int>& indexArray = elementMaterial->GetIndexArray();

                s32 numPolygons = mesh->GetPolygonCount();

                for(s32 i=0; i<numPolygons; ++i){
                    s32 num = mesh->GetPolygonSize(i);
                    U16Vector& indices = indexVectors[ indexArray.GetAt(i) ];

                    switch(num)
                    {
                    case 0:
                        break;

                    case 1:
                        {
                            u16 index = static_cast<u16>( mesh->GetPolygonVertex(i, 0) );
                            indices.push_back(index);
                            indices.push_back(index);
                            indices.push_back(index);
                        }
                        break;

                    case 2:
                        {
                            u16 index0 = static_cast<u16>( mesh->GetPolygonVertex(i, 0) );
                            u16 index1 = static_cast<u16>( mesh->GetPolygonVertex(i, 1) );
                            indices.push_back(index0);
                            indices.push_back(index1);
                            indices.push_back(index1);
                        }
                        break;

                    default:
                        {
                            u16 index0 = static_cast<u16>( mesh->GetPolygonVertex(i, 0) );
                            for(s32 j=2; j<num; ++j){
                                u16 index1 = static_cast<u16>( mesh->GetPolygonVertex(i, j-1) );
                                u16 index2 = static_cast<u16>( mesh->GetPolygonVertex(i, j) );
                                indices.push_back(index0);
                                indices.push_back(index1);
                                indices.push_back(index2);
                            }
                        }
                        break;
                    };
                }
            }

            geometry.indices_.clear();
            for(u32 i=0; i<materials_.size(); ++i){
                if(indexVectors[i].size()<=0){
                    continue;
                }
                u32 indexOffset = geometry.indices_.size();
                for(u32 j=0; j<indexVectors[i].size(); ++j){
                    geometry.indices_.push_back(indexVectors[i][j]);
                }

                Mesh meshCVT;
                meshCVT.geometry_ = geometryIndex;
                meshCVT.material_ = static_cast<s16>(i);
                meshCVT.indexOffset_ = indexOffset;
                meshCVT.numIndices_ = indexVectors[i].size();
                calcSphere(meshCVT);
                meshes_.push_back(meshCVT);
                ++numMeshes;
            }
            geometry.numIndices_ = geometry.indices_.size();
            LIME_DELETE_ARRAY(indexVectors);

        }else{
            numMeshes = 1;
            Mesh meshCVT;
            meshCVT.geometry_ = geometryIndex;
            meshCVT.material_ = 0;
            meshCVT.indexOffset_ = 0;
            meshCVT.numIndices_ = geometry.indices_.size();
            calcSphere(meshCVT);
            meshes_.push_back(meshCVT);
        }

        dst.meshStartIndex_ = static_cast<u8>(meshStart);
        dst.numMeshes_ = static_cast<u8>(numMeshes);
        dst.type_ = Node::Type_Mesh;
    }

    void Converter::pushMaterial(FbxScene* scene)
    {
        for(int i=0; i<scene->GetMaterialCount(); ++i){
            FbxSurfaceMaterial* surface = scene->GetMaterial(i);
            Material material;

            material.flags_ = 0;
            //if(geometry->CastShadow.Get()){
            //    material.flags_ |= load::Material::Flag_CastShadow;
            //}

            //if(geometry->ReceiveShadow.Get()){
            //    material.flags_ |= load::Material::Flag_RecieveShadow;
            //}

            material.texColor_ = -1;
            material.texNormal_ = -1;

            if( surface->GetClassId().Is(FbxSurfacePhong::ClassId) ){
                FbxSurfacePhong* phong = (FbxSurfacePhong*)surface;

                //f32 reflectionFactor = static_cast<f32>(phong->ReflectionFactor.Get());

                material.diffuse_.set(
                    static_cast<f32>(phong->Diffuse.Get()[0]),
                    static_cast<f32>(phong->Diffuse.Get()[1]),
                    static_cast<f32>(phong->Diffuse.Get()[2]),
                    static_cast<f32>(1.0 - phong->TransparencyFactor.Get()));

                material.specular_.set(
                    static_cast<f32>(phong->Specular.Get()[0]),
                    static_cast<f32>(phong->Specular.Get()[1]),
                    static_cast<f32>(phong->Specular.Get()[2]),
                    static_cast<f32>(phong->Shininess.Get()));

                //material.transparent_.set(
                //    static_cast<f32>(phong->TransparentColor.Get()[0]),
                //    static_cast<f32>(phong->TransparentColor.Get()[1]),
                //    static_cast<f32>(phong->TransparentColor.Get()[2]),
                //    static_cast<f32>(phong->SpecularFactor.Get()));

                material.shadow_.set(
                    0.0f,
                    0.0f,
                    0.0f,
                    static_cast<f32>(phong->SpecularFactor.Get()));


            }else if( surface->GetClassId().Is(FbxSurfaceLambert::ClassId) ){
                FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)surface;

                material.diffuse_.set(
                    static_cast<f32>(lambert->Diffuse.Get()[0]),
                    static_cast<f32>(lambert->Diffuse.Get()[1]),
                    static_cast<f32>(lambert->Diffuse.Get()[2]),
                    static_cast<f32>(1.0 - lambert->TransparencyFactor.Get()));

                material.specular_.set(0.0f, 0.0f, 0.0f, 0.0f);

                //material.transparent_.set(
                //    static_cast<f32>(lambert->TransparentColor.Get()[0]),
                //    static_cast<f32>(lambert->TransparentColor.Get()[1]),
                //    static_cast<f32>(lambert->TransparentColor.Get()[2]),
                //    1.0f);

                material.shadow_.set(
                    0.0f,
                    0.0f,
                    0.0f,
                    1.0f);

            }else{
                material.diffuse_.set(0.0f, 0.0f, 0.0f, 1.0f);
                material.specular_.set(0.0f, 0.0f, 0.0f, 0.0f);
                //material.transparent_.set(0.0f, 0.0f, 0.0f, 1.0f);
                material.shadow_.set(0.0f, 0.0f, 0.0f, 1.0f);
            }

            {
                FbxProperty prop = surface->FindProperty(FbxSurfaceMaterial::sDiffuse);
                s32 fileTextureCount = prop.GetSrcObjectCount<FbxFileTexture>();//prop.GetSrcObjectCount(FbxFileTexture::ClassId);
                if(0<fileTextureCount){
                    FbxFileTexture* color = prop.GetSrcObject<FbxFileTexture>();//(FbxFileTexture*)prop.GetSrcObject(FbxFileTexture::ClassId, 0);
                    material.texColor_ = findTexture(color);
                }
            }

            {
                FbxProperty prop = surface->FindProperty(FbxSurfaceMaterial::sNormalMap);
                s32 fileTextureCount = prop.GetSrcObjectCount<FbxFileTexture>();//prop.GetSrcObjectCount(FbxFileTexture::ClassId);
                if(0<fileTextureCount){
                    FbxFileTexture* normal = prop.GetSrcObject<FbxFileTexture>();//(FbxFileTexture*)prop.GetSrcObject(FbxFileTexture::ClassId, 0);
                    material.texNormal_ = findTexture(normal);
                }
            }
            materials_.push_back(material);
        }

        if(materials_.size()<=0){
            Material material;

            material.diffuse_.one();
            material.specular_.one();
            material.shadow_.set(0.0f, 0.0f, 0.0f, 1.0f);
            material.flags_ = 0;
            material.texColor_ = -1;
            material.texNormal_ = -1;
            materials_.push_back(material);
        }
    }

    s32 Converter::findSkeletonNode(const FbxNode* node) const
    {
        for(u32 i=0; i<skeletonNodes_.size(); ++i){
            if(node == skeletonNodes_[i]){
                return i;
            }
        }
        return -1;
    }

    s16 Converter::findTexture(const FbxTexture* texture) const
    {
        for(u32 i=0; i<fbxTextures_.size(); ++i){
            if(texture == fbxTextures_[i]){
                return static_cast<s16>(i);
            }
        }
        return -1;
    }

    void Converter::calcSphere(Mesh& mesh)
    {
        GeometryCVT& geometry = geometries_[ mesh.geometry_ ];
        lmath::Vector3* buffer = LIME_NEW lmath::Vector3[mesh.numIndices_];
        f32* positions = &geometry.positions_[0];

        for(u16 i=0; i<mesh.numIndices_; ++i){
            u32 index = mesh.indexOffset_ + i;

            u16 i0 = geometry.indices_[index];
            lmath::Vector3* v = (lmath::Vector3*)(positions + i0*DimPosition);
            buffer[i] = *v;
        }
        mesh.sphere_ = lmath::Sphere::calcMiniSphere(buffer, mesh.numIndices_);

        LIME_DELETE_ARRAY(buffer);
    }

    void Converter::setVertexProperty(GeometryCVT& geometry)
    {
        u32 vflag = 0;
        vflag |= (0<geometry.positions_.size())? VElem_Position : 0;
        vflag |= (0<geometry.normals_.size())? VElem_Normal : 0;
        vflag |= (0<geometry.tangents_.size())? VElem_Tangent : 0;
        vflag |= (0<geometry.binormals_.size())? VElem_Binormal : 0;
        vflag |= (0<geometry.colors_.size())? VElem_Color : 0;
        vflag |= (0<geometry.texcoords_.size())? VElem_Texcoord : 0;
        vflag |= (0<geometry.bones_.size())? VElem_Bone : 0;
        vflag |= (0<geometry.weights_.size())? VElem_BoneWeight : 0;

        u32 vsize = 0;
        vsize += (0<geometry.positions_.size())? VSize_Position : 0;
        vsize += (0<geometry.normals_.size())? VSize_Normal : 0;
        vsize += (0<geometry.tangents_.size())? VSize_Tangent : 0;
        vsize += (0<geometry.binormals_.size())? VSize_Binormal : 0;
        vsize += (0<geometry.colors_.size())? VSize_Color : 0;
        vsize += (0<geometry.texcoords_.size())? VSize_Texcoord : 0;
        vsize += (0<geometry.bones_.size())? VSize_Bone : 0;
        vsize += (0<geometry.weights_.size())? VSize_BoneWeight : 0;

        geometry.vflag_ = vflag;
        geometry.vsize_ = vsize;
    }

    bool Converter::out(const Char* path)
    {
        LASSERT(NULL != path);
        std::ofstream file(path, std::ios::binary);
        if(!file.is_open()){
            return false;
        }

        Header header;
        header.major_ = 1;
        header.minor_ = 0;
        
        u32 offset = sizeof(Header);
        header.elems_[Elem_Geometry].number_ = geometries_.size();
        header.elems_[Elem_Geometry].offset_ = offset;

        for(u32 i=0; i<geometries_.size(); ++i){
            offset += sizeof(Geometry);
            setVertexProperty(geometries_[i]);
            offset += geometries_[i].vsize_ * geometries_[i].numVertices_;
            offset += sizeof(u16) * geometries_[i].numIndices_;
        }

        header.elems_[Elem_Material].number_ = materials_.size();
        header.elems_[Elem_Material].offset_ = offset;
        for(u32 i=0; i<materials_.size(); ++i){
            offset += sizeof(Material);
        }

        header.elems_[Elem_Mesh].number_ = meshes_.size();
        header.elems_[Elem_Mesh].offset_ = offset;
        for(u32 i=0; i<meshes_.size(); ++i){
            offset += sizeof(Mesh);
        }

        header.elems_[Elem_Node].number_ = nodes_.size();
        header.elems_[Elem_Node].offset_ = offset;
        for(u32 i=0; i<nodes_.size(); ++i){
            offset += sizeof(Node);
        }

        header.elems_[Elem_Joint].number_ = joints_.size();
        header.elems_[Elem_Joint].offset_ = offset;
        for(u32 i=0; i<joints_.size(); ++i){
            offset += sizeof(Joint);
        }

        header.elems_[Elem_Texture].number_ = textures_.size();
        header.elems_[Elem_Texture].offset_ = offset;
        for(u32 i=0; i<textures_.size(); ++i){
            offset += sizeof(Texture);
        }

        //-------------------------------------------
        write(file, header);

        u16 tmp[4] = {0,0,0,0};
        for(u32 i=0; i<geometries_.size(); ++i){
            write(file, geometries_[i], sizeof(Geometry));

            for(u32 j=0; j<geometries_[i].numVertices_; ++j){
                f32* position = &(geometries_[i].positions_[DimPosition*j]);
                writeBuffer(file, position, VSize_Position);

                if(0 < geometries_[i].normals_.size()){
                    f32* normal = &(geometries_[i].normals_[DimNormal*j]);
                    tmp[0] = lcore::toBinary16Float(normal[0]);
                    tmp[1] = lcore::toBinary16Float(normal[1]);
                    tmp[2] = lcore::toBinary16Float(normal[2]);
                    writeBuffer(file, tmp, VSize_Normal);
                }

                if(0 < geometries_[i].tangents_.size()){
                    f32* tangent = &(geometries_[i].tangents_[DimTangent*j]);
                    tmp[0] = lcore::toBinary16Float(tangent[0]);
                    tmp[1] = lcore::toBinary16Float(tangent[1]);
                    tmp[2] = lcore::toBinary16Float(tangent[2]);
                    writeBuffer(file, tmp, VSize_Tangent);
                }

                if(0 < geometries_[i].binormals_.size()){
                    f32* binormal = &(geometries_[i].binormals_[DimBinormal*j]);
                    tmp[0] = lcore::toBinary16Float(binormal[0]);
                    tmp[1] = lcore::toBinary16Float(binormal[1]);
                    tmp[2] = lcore::toBinary16Float(binormal[2]);
                    writeBuffer(file, tmp, VSize_Binormal);
                }

                if(0 < geometries_[i].colors_.size()){
                    u32* color = &(geometries_[i].colors_[DimColor*j]);
                    writeBuffer(file, color, VSize_Color);
                }

                if(0 < geometries_[i].texcoords_.size()){
                    u16* uv = &(geometries_[i].texcoords_[DimUV*j]);
                    writeBuffer(file, uv, VSize_Texcoord);
                }

                if(0 < geometries_[i].bones_.size()){
                    u16* bone = &(geometries_[i].bones_[DimBone*j]);
                    writeBuffer(file, bone, VSize_Bone);
                }

                if(0 < geometries_[i].weights_.size()){
                    u16* weight = &(geometries_[i].weights_[DimBoneWeight*j]);
                    writeBuffer(file, weight, VSize_BoneWeight);
                }
            }
            u16* indices = &(geometries_[i].indices_[0]);
            writeBuffer(file, indices, geometries_[i].numIndices_ * sizeof(u16));
        }

        for(u32 i=0; i<materials_.size(); ++i){
            write(file, materials_[i], sizeof(Material));
        }

        for(u32 i=0; i<meshes_.size(); ++i){
            write(file, meshes_[i], sizeof(Mesh));
        }

        for(u32 i=0; i<nodes_.size(); ++i){
            write(file, nodes_[i], sizeof(Node));
        }

        for(u32 i=0; i<joints_.size(); ++i){
            write(file, joints_[i], sizeof(Joint));
        }

        for(u32 i=0; i<textures_.size(); ++i){
            write(file, textures_[i], sizeof(Texture));
        }

        file.close();

        return true;
    }

    bool Converter::outAnimation(const Char* path)
    {
        LASSERT(NULL != path);
        if(animationClip_.jointAnims_.size()<=0){
            return false;
        }

        std::ofstream file(path, std::ios::binary);
        if(!file.is_open()){
            return false;
        }

        
        Char name[MaxNameSize];
        //lcore::extractFileName(name, MaxNameSize, path);
        copyName(animationClip_.name_, name);

        writeName(file, animationClip_.name_);

        write(file, animationClip_.lastFrame_);
        u32 numClips = animationClip_.jointAnims_.size();
        write(file, numClips);

        for(u32 i=0; i<animationClip_.jointAnims_.size(); ++i){
            const JointAnimationCVT& jointAnim = animationClip_.jointAnims_[i];
            writeName(file, jointAnim.name_);
        }

        for(u32 i=0; i<animationClip_.jointAnims_.size(); ++i){
            const JointAnimationCVT& jointAnim = animationClip_.jointAnims_[i];
            s32 size = (s32)jointAnim.poses_.size();
            write(file, size);

            for(u32 j=0; j<jointAnim.poses_.size(); ++j){
                const JointPoseWithFrame& frame = jointAnim.poses_[j];
                write(file, frame.frameNo_);
                write(file, frame.translation_);
                write(file, frame.rotation_);
            }
        }

        file.close();
        return true;
    }

    void Converter::traverseAnimation(FbxScene* scene)
    {
        s32 animStackCount = scene->GetSrcObjectCount<FbxAnimStack>();
        for(s32 i=0; i<animStackCount; ++i){
            FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(i);
            s32 numAnimLayers = animStack->GetMemberCount<FbxAnimLayer>();
            for(s32 j=0; j<numAnimLayers; ++j){
                FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(i);
                traverseAnimation(animLayer, scene->GetRootNode());
            }
            break;
        }
        animationClip_.calcLastFrame();
    }

    void Converter::traverseAnimation(FbxAnimLayer* animLayer, FbxNode* node)
    {
        FbxNodeAttribute* attr = node->GetNodeAttribute();
        if(NULL != attr && attr->GetAttributeType() == FbxNodeAttribute::eSkeleton){
            u32 index = animationClip_.jointAnims_.size();
            JointAnimationCVT jointAnimation;
            animationClip_.jointAnims_.push_back(jointAnimation);
            traverseAnimation(animationClip_.jointAnims_[index], animLayer, node);

            if(animationClip_.jointAnims_[index].poses_.size() <= 0){
                animationClip_.jointAnims_.pop_back();
            }else if(animationClip_.jointAnims_[index].poses_.size() == 1){

                JointPoseWithFrame& pose = animationClip_.jointAnims_[index].poses_[0];
                if(lmath::isZero(pose.translation_.x_)
                    && lmath::isZero(pose.translation_.y_)
                    && lmath::isZero(pose.translation_.z_)
                    && lmath::isEqual(pose.rotation_.w_, 1.0f)
                    && lmath::isZero(pose.rotation_.x_)
                    && lmath::isZero(pose.rotation_.y_)
                    && lmath::isZero(pose.rotation_.z_))
                {
                    animationClip_.jointAnims_.pop_back();
                }
            }
        }


        for(s32 i=0; i<node->GetChildCount(); ++i){
            traverseAnimation(animLayer, node->GetChild(i));
        }
    }

    void Converter::traverseAnimation(JointAnimationCVT& jointAnimation, FbxAnimLayer* animLayer, FbxNode* node)
    {
        copyName(jointAnimation.name_, node->GetName());

        FbxAnimCurve* animCurve = NULL;

        animCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
        pushAnimation(jointAnimation, animCurve, node);

        animCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        pushAnimation(jointAnimation, animCurve, node);

        animCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        pushAnimation(jointAnimation, animCurve, node);

        animCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
        pushAnimation(jointAnimation, animCurve, node);

        animCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        pushAnimation(jointAnimation, animCurve, node);

        animCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        pushAnimation(jointAnimation, animCurve, node);
    }

    void Converter::pushAnimation(JointAnimationCVT& jointAnimation, FbxAnimCurve* animCurve, FbxNode* node)
    {
        if(NULL == animCurve){
            return;
        }

        ////  今の時間のモデル基準姿勢を貰う
        //FbxAMatrix currentFbxMat = pNode->EvaluateGlobalTransform( fbxTime );

        ////  骨の親を貰う
        //FbxNode* pParent = GetParentSkeltonNode( pNode->GetParent() );
        //if( pParent ){
        //    //  親がいるなら、親の今の時間のモデル基準姿勢を貰い、その逆行列を乗算して親から見た姿勢にする
        //    const FbxAMatrix& parentMat = pParent->EvaluateGlobalTransform( fbxTime );
        //    const FbxAMatrix  parentInvMat = parentMat.Inverse();
        //    currentFbxMat.operator*=( parentInvMat );
        //}

        ////  Matrix4x3に変換
        //LOOP( r, 4 ){
        //    LOOP( c, 3 ){
        //        currentMatrix.m[ r ][ c ] = SCAST( F32 )( currentFbxMat[ r ][ c ] );
        //    }
        //}

        ////  回転部分のクォータニオンを貰う
        //currentMatrix.DecomposeRotate( &keys[ i ].m_Value );

        ////  正規化
        //keys[ i ].m_Value.Normalize();

        //keys[ i ].m_Frame = SCAST( U16 )( frame - startFrame );

        FbxAMatrix localMatrix = node->EvaluateLocalTransform();
        localMatrix.Inverse();

        s32 keyCount = animCurve->KeyGetCount();
        FbxTime keyTime;
        FbxDouble4 t;
        FbxQuaternion r;
        lmath::Vector3 tt;
        lmath::Quaternion tr;
        for(s32 i=0; i<keyCount; ++i){
            keyTime = animCurve->KeyGetTime(i).GetFramedTime();

            FbxAMatrix transform = node->EvaluateLocalTransform(keyTime);
            transform *= localMatrix;

            t = transform.GetT();
            r = transform.GetQ();

            tt.x_ = static_cast<f32>(t[0]);
            tt.y_ = static_cast<f32>(t[1]);
            tt.z_ = static_cast<f32>(t[2]);

            tr.w_ = static_cast<f32>(r[0]);
            tr.x_ = static_cast<f32>(r[0]);
            tr.y_ = static_cast<f32>(r[1]);
            tr.z_ = static_cast<f32>(r[2]);
            u32 frame = keyTime.GetFrameCount();
            jointAnimation.add(frame, tt, tr);
        }
    }

    void Converter::print(FbxScene* scene)
    {
        s32 animStackCount = scene->GetSrcObjectCount<FbxAnimStack>();
        for(s32 i=0; i<animStackCount; ++i){
            FbxAnimStack* animStack = scene->GetSrcObject<FbxAnimStack>(i);

            FbxString str = "Animation Stack Name: ";
            str += animStack->GetName();
            str += "\n\n";
            FBXSDK_printf(str);

            print(animStack, scene->GetRootNode());
        }
    }

    void Converter::print(FbxAnimStack* animStack, FbxNode* node)
    {
        s32 numAnimLayers = animStack->GetMemberCount<FbxAnimLayer>();
        FbxString str = "Animation stack contains ";
        str += numAnimLayers;
        str += " Animation Layer(s)\n";
        FBXSDK_printf(str);

        for(s32 i=0; i<numAnimLayers; ++i){
            FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(i);
            str = "AnimLayer ";
            str += i;
            str += " ";
            str += animLayer->GetName();
            str += "\n";
            FBXSDK_printf(str);

            print(animLayer, node);
        }
    }

    void Converter::print(FbxAnimLayer* animLayer, FbxNode* node)
    {
        FbxString str = "    Node Name: ";
        str += node->GetName();
        str += "\n\n";
        FBXSDK_printf(str);

        printChannels(animLayer, node, printCurve, printListCurve);
        FBXSDK_printf("\n");

        for(s32 i=0; i<node->GetChildCount(); ++i){
            print(animLayer, node->GetChild(i));
        }
    }

    void Converter::printChannels(FbxAnimLayer* animLayer, FbxNode* node, void (*printCurve)(FbxAnimCurve* curve), void (*printListCurve)(FbxAnimCurve* curve, FbxProperty* prop))
    {
        FbxAnimCurve* animCurve = NULL;

        animCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if(animCurve){
            FBXSDK_printf("        TX\n");
            printCurve(animCurve);
        }

        animCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if(animCurve){
            FBXSDK_printf("        TY\n");
            printCurve(animCurve);
        }

        animCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if(animCurve){
            FBXSDK_printf("        TZ\n");
            printCurve(animCurve);
        }

        animCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if(animCurve){
            FBXSDK_printf("        RX\n");
            printCurve(animCurve);
        }

        animCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if(animCurve){
            FBXSDK_printf("        RY\n");
            printCurve(animCurve);
        }

        animCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if(animCurve){
            FBXSDK_printf("        RZ\n");
            printCurve(animCurve);
        }

        animCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if(animCurve){
            FBXSDK_printf("        SX\n");
            printCurve(animCurve);
        }

        animCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if(animCurve){
            FBXSDK_printf("        SY\n");
            printCurve(animCurve);
        }

        animCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if(animCurve){
            FBXSDK_printf("        SZ\n");
            printCurve(animCurve);
        }


        FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
        if(nodeAttribute){
            animCurve = nodeAttribute->Color.GetCurve(animLayer, FBXSDK_CURVENODE_COLOR_RED);
            if(animCurve){
                FBXSDK_printf("        Red\n");
                printCurve(animCurve);
            }

            animCurve = nodeAttribute->Color.GetCurve(animLayer, FBXSDK_CURVENODE_COLOR_GREEN);
            if(animCurve){
                FBXSDK_printf("        Green\n");
                printCurve(animCurve);
            }

            animCurve = nodeAttribute->Color.GetCurve(animLayer, FBXSDK_CURVENODE_COLOR_BLUE);
            if(animCurve){
                FBXSDK_printf("        Blue\n");
                printCurve(animCurve);
            }

            if(nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh
                || nodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs
                || nodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
            {
                FbxGeometry* geometry = (FbxGeometry*)nodeAttribute;
                s32 blendShapeDeformerCount = geometry->GetDeformerCount(FbxDeformer::eBlendShape);
                for(s32 i=0; i<blendShapeDeformerCount; ++i){
                    FbxBlendShape* blendShape = (FbxBlendShape*)geometry->GetDeformer(i, FbxDeformer::eBlendShape);
                    s32 blendShapeChannelCount = blendShape->GetBlendShapeChannelCount();
                    for(s32 j=0; j<blendShapeChannelCount; ++j){
                        FbxBlendShapeChannel* channel = blendShape->GetBlendShapeChannel(j);
                        animCurve = geometry->GetShapeChannel(i, j, animLayer, true);
                        if(animCurve){
                            FBXSDK_printf("        Shape %s\n", channel->GetName());
                            printCurve(animCurve);
                        }
                    }
                }
            }
        }

        FbxLight* light = node->GetLight();
        if(light){
            animCurve = light->Intensity.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Intensity\n");
                printCurve(animCurve);
            }

            animCurve = light->OuterAngle.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        OuterAngle\n");
                printCurve(animCurve);
            }

            animCurve = light->Fog.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Fog\n");
                printCurve(animCurve);
            }
        }

        FbxCamera* camera = node->GetCamera();
        if(camera){
            animCurve = camera->FieldOfView.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Field of View\n");
                printCurve(animCurve);
            }

            animCurve = camera->FieldOfViewX.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Field of View X\n");
                printCurve(animCurve);
            }

            animCurve = camera->FieldOfViewY.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Field of View Y\n");
                printCurve(animCurve);
            }

            animCurve = camera->OpticalCenterX.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Optical Center X\n");
                printCurve(animCurve);
            }

            animCurve = camera->OpticalCenterY.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Optical Center Y\n");
                printCurve(animCurve);
            }

            animCurve = camera->Roll.GetCurve(animLayer);
            if(animCurve){
                FBXSDK_printf("        Roll\n");
                printCurve(animCurve);
            }
        }
    }

namespace
{
    static int InterpolationFlagToIndex(int flags)
    {
        if( (flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant ) return 1;
        if( (flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear ) return 2;
        if( (flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic ) return 3;
        return 0;
    }

    static int ConstantmodeFlagToIndex(int flags)
    {
        if( (flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard ) return 1;
        if( (flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext ) return 2;
        return 0;
    }

    static int TangentmodeFlagToIndex(int flags)
    {
        if( (flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto ) return 1;
        if( (flags & FbxAnimCurveDef::eTangentAutoBreak)== FbxAnimCurveDef::eTangentAutoBreak ) return 2;
        if( (flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB ) return 3;
        if( (flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser ) return 4;
        if( (flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak ) return 5;
        if( (flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak ) return 6;
        return 0;
    }

    static int TangentweightFlagToIndex(int flags)
    {
        if( (flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone ) return 1;
        if( (flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight ) return 2;
        if( (flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft ) return 3;
        return 0;
    }

    static int TangentVelocityFlagToIndex(int flags)
    {
        if( (flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone ) return 1;
        if( (flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight ) return 2;
        if( (flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft ) return 3;
        return 0;
    }
}
    void Converter::printCurve(FbxAnimCurve* curve)
    {
        static const Char* interpolation[] = { "?", "constant", "linear", "cubic"};
        static const Char* constantMode[] =  { "?", "Standard", "Next" };
        static const Char* cubicMode[] =     { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
        static const Char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

        Char timeString[256];
        FbxString str;
        s32 keyCount = curve->KeyGetCount();
        f32 keyValue;
        FbxTime keyTime;
        for(s32 i=0; i<keyCount; ++i){
            keyValue = static_cast<f32>(curve->KeyGetValue(i));
            keyTime = curve->KeyGetTime(i);

            str = "        Key Time: ";
            str += keyTime.GetTimeString(timeString, FbxUShort(256));
            str += ".... Key Value: ";
            str += keyValue;
            str += " [ ";
            str += interpolation[ InterpolationFlagToIndex(curve->KeyGetInterpolation(i)) ];

            if((curve->KeyGetInterpolation(i)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant){
                str += " | ";
                str += constantMode[ ConstantmodeFlagToIndex(curve->KeyGetConstantMode(i)) ];
            }else if((curve->KeyGetInterpolation(i)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic){
                str += " | ";
                str += cubicMode[ TangentmodeFlagToIndex(curve->KeyGetTangentMode(i)) ];
                str += " | ";
                str += tangentWVMode[ TangentweightFlagToIndex(curve->KeyGet(i).GetTangentWeightMode()) ];
                str += " | ";
                str += tangentWVMode[ TangentVelocityFlagToIndex(curve->KeyGet(i).GetTangentWeightMode()) ];
            }
            str += " ]\n";
            FBXSDK_printf(str);
        }
    }

    void Converter::printListCurve(FbxAnimCurve* curve, FbxProperty* prop)
    {
    }
}
