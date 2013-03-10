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

    void correctUV(f32& u0, f32& u1)
    {
        f32 tu0, tu1;
        tu0 = lcore::absolute(u1 - u0);
        tu1 = lcore::absolute(u1 - u0 - 1.0f);
        if(tu0>tu1){
            u1 -= 1.0f;
        }
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
}

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
        //for(s32 i=0; i<scene->GetMaterialCount(); ++i){
        //     FbxSurfaceMaterial* src = scene->GetMaterial(i);

        //     u32 id = static_cast<u32>(src->GetUniqueID());
        //     lcore::Log("material%d: %s", id, src->GetName());
        //}

        FbxNode* root = scene->GetRootNode();

        for(s32 i=0; i<root->GetChildCount(); ++i){
            pushNode(root->GetChild(i), InvalidNode);
        }

        s32 count = 0;
        for(s32 i=0; i<root->GetChildCount(); ++i){
            const FbxNodeAttribute* attr = root->GetChild(i)->GetNodeAttribute();

            if(NULL != attr){
                switch(attr->GetAttributeType())
                {
                case FbxNodeAttribute::eCamera:
                case FbxNodeAttribute::eLight:
                    continue;
                };
            }
            traverseNode(root->GetChild(i), count);
            ++count;
        }
        return true;
    }

    void Converter::traverseNode(FbxNode* node, s32 index)
    {
        u8 childStartIndex = static_cast<u8>( nodes_.size() );
        for(s32 i=0; i<node->GetChildCount(); ++i){
            pushNode(node->GetChild(i), static_cast<u8>(index));
        }

        Node& n = nodes_[index];
        //lcore::Log("(%d)(%d) %s (%s)", n.parentIndex_, n.index_, node->GetName(), getAttributeType(node->GetNodeAttribute()));

        n.childStartIndex_ = childStartIndex;
        n.numChildren_ = static_cast<u8>( node->GetChildCount() );

        s32 count = 0;
        for(s32 i=0; i<node->GetChildCount(); ++i){
            const FbxNodeAttribute* attr = node->GetChild(i)->GetNodeAttribute();

            if(NULL != attr){
                switch(attr->GetAttributeType())
                {
                case FbxNodeAttribute::eCamera:
                case FbxNodeAttribute::eLight:
                    continue;
                };
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
        n.reserved_ = 0;
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
                pushMesh(n, node, parent);
                break;

            case FbxNodeAttribute::eCamera:
            case FbxNodeAttribute::eLight:
                return;
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


    void Converter::pushMesh(Node& dst, FbxNode* src, u8 /*parent*/)
    {
        //TODO:UVとマテリアル毎にジオメトリ分割
        FbxMesh* mesh = src->GetMesh();

        if(geometries_.size()<=0){
            geometries_.resize(1);
        }

        GeometryCVT& geometry = geometries_[0];
        s32 stride = geometry.vsize_ / sizeof(f32);
        u16 indexOffset = (stride>0)? static_cast<u16>( geometry.vertices_.size() / stride) : 0;

        Mesh meshCVT;
        meshCVT.geometry_ = 0;
        meshCVT.indexOffset_ = geometry.indices_.size();

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
        meshCVT.numIndices_ = numIndices;

        s32 normalLayerIndex = -1;

        for(s32 i=0; i<mesh->GetLayerCount(); ++i){
            FbxLayerElementNormal* elementNormal = mesh->GetLayer(i)->GetNormals();
            if(NULL != elementNormal){
                normalLayerIndex = i;
                break;
            }
        }

        u32 size = geometry.vertices_.size() + VertexStride * mesh->GetControlPointsCount();
        stride = VertexStride;
        geometry.vsize_ = sizeof(f32) * VertexStride;
        geometry.vflag_ = VElem_Position;
        if(normalLayerIndex>=0){
            size += NormalStride  * mesh->GetControlPointsCount();
            stride += NormalStride;
            geometry.vsize_ += sizeof(f32) * NormalStride;
            geometry.vflag_ |= VElem_Normal;
        }
        geometry.vertices_.resize(size);

        FbxVector4* srcVertices = mesh->GetControlPoints();
        u32 v = indexOffset * stride;
        for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
            geometry.vertices_[v + 0] = static_cast<f32>(srcVertices[i][0]);
            geometry.vertices_[v + 1] = static_cast<f32>(srcVertices[i][1]);
            geometry.vertices_[v + 2] = static_cast<f32>(srcVertices[i][2]);

            //ついでにクリア
            for(s32 j=VertexStride; j<stride; ++j){
                geometry.vertices_[v + j] = 0.0f;
            }
            v += stride;
        }

        // 法線
        if(normalLayerIndex>=0){
            FbxLayerElementNormal* elementNormal = mesh->GetLayer(normalLayerIndex)->GetNormals();
            FbxLayerElement::EMappingMode mappingMode = elementNormal->GetMappingMode();
            if(mappingMode == FbxLayerElement::eByPolygonVertex){
                FbxLayerElement::EReferenceMode reference = elementNormal->GetReferenceMode();

                s32 count = 0;
                switch(reference)
                {
                case FbxLayerElement::eDirect:
                    {
                        for(s32 i=0; i<numPolygons; ++i){
                            s32 num = mesh->GetPolygonSize(i);
                            for(s32 j=0; j<num; ++j){
                                FbxVector4 n = elementNormal->GetDirectArray().GetAt(count);
                                u32 index = indexOffset + static_cast<u16>( mesh->GetPolygonVertex(i, j) );
                                index *= stride;
                                index += VertexStride;
                                geometry.vertices_[index + 0] += static_cast<f32>(n[0]);
                                geometry.vertices_[index + 1] += static_cast<f32>(n[1]);
                                geometry.vertices_[index + 2] += static_cast<f32>(n[2]);
                                ++count;
                            }
                        }
                    }
                    break;

                case FbxLayerElement::eIndexToDirect:
                    break;
                };


            }else if(mappingMode == FbxLayerElement::eByControlPoint){
                //TODO:
                FbxLayerElement::EReferenceMode reference = elementNormal->GetReferenceMode();
                switch(reference)
                {
                case FbxLayerElement::eDirect:

                    break;

                case FbxLayerElement::eIndexToDirect:
                    break;
                };
            }

            //法線正規化
            v = indexOffset * stride + VertexStride;
            for(s32 i=0; i<mesh->GetControlPointsCount(); ++i){
                f32 x = geometry.vertices_[v + 0];
                f32 y = geometry.vertices_[v + 1];
                f32 z = geometry.vertices_[v + 2];
                f32 l = x*x + y*y + z*z;
                if(false == lmath::isZeroPositive(l)){
                    l = 1.0f/lmath::sqrt(l);
                    geometry.vertices_[v + 0] *= l;
                    geometry.vertices_[v + 1] *= l;
                    geometry.vertices_[v + 2] *= l;
                }
                v += stride;
            }
        } //if(normalLayerIndex>=0){

        //マテリアル
        if(src->GetMaterialCount()>0){
            meshCVT.material_ = pushMaterial(src->GetMaterial(0), mesh);
        }

        calcSphere(meshCVT);

        geometry.numIndices_ = geometry.indices_.size();
        geometry.numVertices_ = geometry.vertices_.size()/ stride;
        dst.meshStartIndex_ = static_cast<u8>(meshes_.size());
        dst.numMeshes_ = 1;
        meshes_.push_back(meshCVT);
    }


    s32 Converter::pushMaterial(FbxSurfaceMaterial* src, FbxGeometryBase* geometry)
    {
        for(u32 i=0; i<materials_.size(); ++i){
            if(materials_[i].uniqueID_ == src->GetUniqueID()){
                return static_cast<s32>(i);
            }

            if(0 == materials_[i].name_.compare(src->GetName())){
                return static_cast<s32>(i);
            }
        }

        //u32 id = static_cast<u64>(src->GetUniqueID());
        //lcore::Log("material%d: %s", id, src->GetName());

        MaterialCVT material;

        material.flags_ = 0;
        if(geometry->CastShadow.Get()){
            material.flags_ |= load::Material::Flag_CastShadow;
        }

        if(geometry->ReceiveShadow.Get()){
            material.flags_ |= load::Material::Flag_RecieveShadow;
        }

        material.texColor_ = -1;
        material.texNormal_ = -1;
        material.uniqueID_ = src->GetUniqueID();
        material.name_.assign(src->GetName());

        if( src->GetClassId().Is(FbxSurfacePhong::ClassId) ){
            FbxSurfacePhong* phong = (FbxSurfacePhong*)src;

            
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

        }else if( src->GetClassId().Is(FbxSurfaceLambert::ClassId) ){
            FbxSurfaceLambert* lambert = (FbxSurfaceLambert*)src;

            material.diffuse_.set(
                static_cast<f32>(lambert->Diffuse.Get()[0]),
                static_cast<f32>(lambert->Diffuse.Get()[1]),
                static_cast<f32>(lambert->Diffuse.Get()[2]),
                static_cast<f32>(lambert->TransparencyFactor.Get()));

            material.specular_.set(0.0f, 0.0f, 0.0f, 0.0f);
        }else{
            material.diffuse_.set(0.0f, 0.0f, 0.0f, 1.0f);
            material.specular_.set(0.0f, 0.0f, 0.0f, 0.0f);
        }

        FbxProperty prop;

        prop = src->FindProperty(FbxSurfaceMaterial::sDiffuse);


        s32 ret = static_cast<s32>(materials_.size());
        materials_.push_back(material);
        return ret;
    }

    bool Converter::findMesh(u16& meshStartIndex, u16& numMeshes, s32 geometryIndex)
    {
        for(u32 i=0; i<nodes_.size(); ++i){
            for(u32 j=0; j<nodes_[i].numMeshes_; ++j){
                Mesh& mesh = meshes_[ nodes_[i].meshStartIndex_ + j ];
                if(geometryIndex == mesh.geometry_){
                    meshStartIndex = nodes_[i].meshStartIndex_;
                    numMeshes = nodes_[i].numMeshes_;
                    return true;
                }
            }
        }
        return false;
    }

    void Converter::calcSphere(Mesh& mesh)
    {
        GeometryCVT& geometry = geometries_[ mesh.geometry_ ];
        lmath::Vector3* buffer = LIME_NEW lmath::Vector3[mesh.numIndices_];
        f32* vertices = &geometry.vertices_[0];

        s32 stride = geometry.vsize_/sizeof(f32);
        for(u16 i=0; i<mesh.numIndices_; ++i){
            u16 index = mesh.indexOffset_ + i;

            u16 i0 = geometry.indices_[index + 0];
            lmath::Vector3* v = (lmath::Vector3*)(vertices + i0*stride);
            buffer[i] = *v;
        }
        mesh.sphere_ = lmath::Sphere::calcMiniSphere(buffer, mesh.numIndices_);

        LIME_DELETE_ARRAY(buffer);
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

        header.elems_[Elem_Texture].number_ = 0;
        header.elems_[Elem_Texture].offset_ = offset;


        //-------------------------------------------
        write(file, header);

        for(u32 i=0; i<geometries_.size(); ++i){
            write(file, geometries_[i], sizeof(Geometry));
            f32* vertices = &(geometries_[i].vertices_[0]);
            writeBuffer(file, vertices, geometries_[i].numVertices_ * geometries_[i].vsize_);
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
        file.close();
        return true;
    }
}
