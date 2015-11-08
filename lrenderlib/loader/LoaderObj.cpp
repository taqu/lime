/**
@file LoaderObj.cpp
@author t-sakai
@date 2015/09/14 create
*/
#include "LoaderObj.h"
#include <stdarg.h>
#include <stdio.h>
#include "core/AABB.h"
#include "bsdf/Microfacet.h"
#include "IOPNG.h"
#include "image/Image.h"
#include "texture/Texture2D.h"

namespace lrender
{
namespace
{
    void convDelimiter(Char* path)
    {
        while(*path != '\0'){
            if(*path == '\\'){
                *path = '/';
            }
            ++path;
        }
    }

    void Log(const Char* format, ...)
    {
        if(format == NULL){
            return;
        }

        va_list ap;
        va_start(ap, format);

        static const u32 MaxBuffer = 1024;

        Char buffer[MaxBuffer+2];
        int count=vsnprintf_s(buffer, MaxBuffer, format, ap);
        if(count<0){
            count = MaxBuffer;
        }

        buffer[count]='\n';
        buffer[count+1]='\0';
        OutputDebugString(buffer);

        va_end(ap);
    }

    void LogElements(const lcore::vector_arena<Char*>& elements)
    {
        for(lcore::vector_arena<Char*>::const_iterator itr = elements.begin();
            itr != elements.end();
            ++itr)
        {
            OutputDebugString(*itr);
            OutputDebugString(" ");
        }
        OutputDebugString("\n");
    }
#ifdef _DEBUG
#define LOGELEMENTS(v) LogElements(v)
#define LOG(str, ...) Log((str), __VA_ARGS__)
#else
#define LOGELEMENTS(v)
#define LOG(str, ...)
#endif
}


    //------------------------------------------------------
    //---
    //--- LoaderMtl
    //---
    //------------------------------------------------------
    LoaderObj::LoaderMtl::LoaderMtl()
        :directory_(NULL)
    {
    }

    LoaderObj::LoaderMtl::~LoaderMtl()
    {
    }

    s32 LoaderObj::LoaderMtl::loadFromFile(BSDFVector& brdfs, const Char* directory, const Char* filename)
    {
        directory_ = directory;

        s32 len = 0;
        s32 dlen = 0;
        if(NULL != directory){
            dlen = lcore::strlen(directory);
            len += dlen;
        }
        len += lcore::strlen(filename) + 1;

        Char* path = LIME_NEW Char[len];
        path[0] = '\0';
        if(NULL != directory){
            lcore::strcat(path, len, directory);
            //len -= dlen;
        }
        lcore::strcat(path, len, filename);

        file_.open(path, lcore::ios::binary);
        LIME_DELETE(path);

        if(!file_.is_open()){
            return 0;
        }

        while(readLine(brdfs));
        for(s32 i=0; i<brdfs.size(); ++i){
            brdfs[i]->initialize();
        }
        return brdfs.size();
    }

    void LoaderObj::LoaderMtl::skip()
    {
        s32 pos = file_.tellg();
        s32 c = file_.get();

        while(isspace(c)
            || c == '\n'
            || c == '\r')
        {
            if(file_.eof()
                || !file_.good())
            {
                return;
            }
            ++pos;
            c = file_.get();
        }
        file_.seekg(pos, lcore::ios::beg);
    }

    bool LoaderObj::LoaderMtl::getLine()
    {
        skip();
        line_.clear();
        
        bool comment = false;
        s32 c = file_.get();
        while(!file_.eof()
            && file_.good()
            && c != '\n'
            && c != '\r')
        {
            if(c == '#'){
                comment = true;
            }else if(!comment){
                line_.push_back(c);
            }
            c = file_.get();
        }
        return (comment || 0<line_.size());
    }

    void LoaderObj::LoaderMtl::split()
    {
        elements_.clear();

        s32 i=0;
        while(i<line_.size()){
            elements_.push_back(&line_[i]);
            for(;i<line_.size(); ++i){
                if(isspace(line_[i])){
                    break;
                }
            }
            for(;i<line_.size(); ++i){
                if(!isspace(line_[i])){
                    break;
                }
                line_[i] = '\0';
            }
        }
        line_.push_back('\0');
    }

    LoaderObj::LoaderMtl::CodeMtl LoaderObj::LoaderMtl::decodeLine()
    {
        if(file_.eof() || !file_.good()){
            return CodeMtl_EOF;
        }

        if(false == getLine()){
            return CodeMtl_EOF;
        }

        split();

        if(elements_.size()<=0){
            return CodeMtl_None;
        }

        if(0 == strncmp(elements_[0], "newmtl", 6)){
            return CodeMtl_NewMtl;

        }else if(0 == strncmp(elements_[0], "map_Kd", 6)){
            return CodeMtl_MapDiffuse;

        }else if(0 == strncmp(elements_[0], "map_bump", 8)){
            return CodeMtl_MapBump;

        }else if(0 == strncmp(elements_[0], "illum", 5)){
            return CodeMtl_Illumination;

        }else if(0 == strncmp(elements_[0], "Ns", 2)){
            return CodeMtl_SpecularExponent;

        }else if(0 == strncmp(elements_[0], "Ni", 2)){
            return CodeMtl_RefracttionIndex;

        }else if(0 == strncmp(elements_[0], "Ka", 2)){
            return CodeMtl_Ambient;

        }else if(0 == strncmp(elements_[0], "Kd", 2)){
            return CodeMtl_Diffuse;

        }else if(0 == strncmp(elements_[0], "Ks", 2)){
            return CodeMtl_Specular;

        }else if(0 == strncmp(elements_[0], "Tf", 2)){
            return CodeMtl_Transmission;

        }else if(0 == strncmp(elements_[0], "Ke", 2)){
            return CodeMtl_Emission;

        }else if(0 == strncmp(elements_[0], "d", 1)){
            return CodeMtl_Dissolve;

        }
        return CodeMtl_None;
    }

    void LoaderObj::LoaderMtl::load(f32& f)
    {
        if(1<elements_.size()){
            f = atof(elements_[1]);
        }else{
            f = 0.0f;
        }
    }

    void LoaderObj::LoaderMtl::load(Vector3& v)
    {
        v.zero();
        s32 numElements =  0;
        for(s32 i=1; i<elements_.size(); ++i){
            v[numElements] = atof(elements_[i]);
            ++numElements;
            if(3<=numElements){
                break;
            }
        }
    }


    void LoaderObj::LoaderMtl::load(Color3& rgb)
    {
        rgb = Color3::black();

        s32 numElements =  0;
        for(s32 i=1; i<elements_.size(); ++i){
            rgb[numElements] = atof(elements_[i]);
            ++numElements;
            if(3<=numElements){
                break;
            }
        }
    }

    Texture::pointer LoaderObj::LoaderMtl::loadTexture(const Char* filename)
    {
        s32 len = 0;
        s32 dlen = 0;
        if(NULL != directory_){
            dlen = lcore::strlen(directory_);
            len += dlen;
        }
        len += lcore::strlen(filename) + 1;

        Char* path = LIME_NEW Char[len];
        path[0] = '\0';
        if(NULL != directory_){
            lcore::strcat(path, len, directory_);
        }
        lcore::strcat(path, len, filename);

        lcore::ifstream file;
        file.open(path, lcore::ios::binary);
        LIME_DELETE(path);

        if(!file.is_open()){
            return Texture::pointer(NULL);
        }

        lrender::s32 width, height, rowBytes;
        lrender::IOPNG::read(file, NULL, width, height, rowBytes, lrender::IOPNG::Swap_BGR);
        lrender::Color4* rgba = LIME_NEW lrender::Color4[width*height];
        if(false == lrender::IOPNG::read(file, rgba, width, height, rowBytes, lrender::IOPNG::Swap_BGR)){
            LIME_DELETE_ARRAY(rgba);
            return Texture::pointer(NULL);
        }

        Image image(width, height, rgba);
        image.standardToLinearRGB();

        return Texture::pointer(LIME_NEW Texture2D(image));
    }

    Texture::pointer LoaderObj::LoaderMtl::loadAsConstant()
    {
        Color3 rgb = Color3::white();

        s32 numElements =  0;
        for(s32 i=1; i<elements_.size(); ++i){
            rgb[numElements] = atof(elements_[i]);
            ++numElements;
            if(3<=numElements){
                break;
            }
        }
        return Texture::pointer(LIME_NEW TextureConstant(rgb));
    }

    Texture::pointer LoaderObj::LoaderMtl::loadAsImage()
    {
        if(1<elements_.size()){
            convDelimiter(elements_[1]);
            return loadTexture(elements_[1]);
        }
        return Texture::pointer(NULL);
    }

    bool LoaderObj::LoaderMtl::readLine(BSDFVector& bsdfs)
    {
        CodeMtl code = decodeLine();

        if(CodeMtl_EOF == code){
            return false;

        }else if(CodeMtl_NewMtl == code){
            bsdfs.push_back( BSDF::pointer(LIME_NEW Microfacet()) );

            Microfacet* brdf = dynamic_cast<Microfacet*>(bsdfs[bsdfs.size()-1].get());
            if(1<elements_.size()){
                brdf->setName(elements_[1]);
            }
            return true;
        }

        Microfacet* brdf = (0<bsdfs.size())? dynamic_cast<Microfacet*>(bsdfs[bsdfs.size()-1].get()) : NULL;

        switch(code)
        {
        case CodeMtl_None:
            return true;

        case CodeMtl_NewMtl:
            return true;

        case CodeMtl_Model:
            return true;

        case CodeMtl_Dissolve:
            return true;

        case CodeMtl_SpecularExponent:
            {
                f32 specularExponent = 0.0f;
                load(specularExponent);
                specularExponent = lcore::clamp01(specularExponent*(1.0f/255.0f));
                f32 alpha = 1.0f - specularExponent;
                //brdf->distribution_.setAlphaU(alpha);
                //brdf->distribution_.setAlphaV(alpha);
                brdf->distribution_.setAlpha(alpha);
            }
            return true;

        case CodeMtl_RefracttionIndex:
            {
                f32 refractiveIndex = 1.0f;
                load(refractiveIndex);
                brdf->fresnel_.setDielectic(1.00028f, (refractiveIndex<1.0e-5f)? 1.0e-3f : refractiveIndex);
            }
            return true;

        case CodeMtl_Ambient:
            {
                lmath::Vector3 ambient;
                load(ambient);
            }
            return true;

        case CodeMtl_Diffuse:
            {
                brdf->diffuseScale_ = loadAsConstant();
            }
            return true;

        case CodeMtl_Specular:
            {
                brdf->specularScale_ = loadAsConstant();
            }
            return true;

        case CodeMtl_Transmission:
            {
            }
            return true;

        case CodeMtl_Emission:
            {
            }
            return true;

        case CodeMtl_MapDiffuse:
            {
                brdf->diffuseScale_ = loadAsImage();
            }
            return true;

        case CodeMtl_MapBump:
            {
                //if(1<elems_.size()){
                //    convDelimiter(elems_[1]);
                //    loadTexture(elems_[1]);
                //    s32 id = getTexture(elems_[1]);
                //    material.texNormal_ = (0<=id)? static_cast<s16>(id) : -1;
                //}
            }
            return true;

        case CodeMtl_Illumination:
            return true;

        case CodeMtl_EOF:
            return false;

        default:
            return false;
        }
    }

    //------------------------------------------------------
    //---
    //--- LoaderObj
    //---
    //------------------------------------------------------
    LoaderObj::LoaderObj(bool adjustGeometryNormals)
        :directory_(NULL)
        ,adjustGeometryNormals_(adjustGeometryNormals)
        ,lastNumTriangles_(0)
    {
    }

    LoaderObj::~LoaderObj()
    {
        LIME_DELETE_ARRAY(directory_);
    }

    s32 LoaderObj::loadFromFile(Shape::ShapeVector& shapes, const Char* filename)
    {
        LASSERT(NULL != filename);
        if(!file_.open(filename, lcore::ios::binary)){
            return -1;
        }

        shapes.clear();

        s32 len = lcore::strlen(filename);
        LIME_DELETE_ARRAY(directory_);
        directory_ = LIME_NEW Char[len + 2];
        s32 dlen = lcore::extractDirectoryPath(directory_, filename, len);
        convDelimiter(directory_);
        if(0<dlen && directory_[dlen-1] != '/'){
            directory_[dlen] = '/';
            directory_[dlen+1] = '\0';
        }

        mesh_ = LIME_NEW Mesh();

        line_.reserve(1024);
        elements_.clear();

        positions_.clear();
        normals_.clear();
        texcoords_.clear();
        vertices_.clear();
        bsdfs_.clear();

        shapes_ = &shapes;
        lastNumTriangles_ = 0;
        while(readLine());

        //マテリアルがない
        if(bsdfs_.size()<=0){
            Microfacet::pointer microfacet(LIME_NEW Microfacet());
            microfacet->initialize();
            bsdfs_.push_back(microfacet);
        }

        //shapeがない
        if(shapes_->size()<=0){
            MeshInstance::pointer meshInstance(LIME_NEW MeshInstance(mesh_, 0, 0));
            meshInstance->setBSDF(bsdfs_[0]);
            shapes_->push_back(meshInstance);
        }

        {
            MeshInstance::pointer& meshInstance = getLastMeshInstance();
            meshInstance->setPrimitiveCount(lastNumTriangles_);
            lastNumTriangles_ = 0;
        }

        BufferVector3 points;
        BufferVector3 normals;
        BufferVector2 texcoords;
        BufferTriangle triangles;

        s32 numVertices = vertices_.size();
        BufferVector3::create(points, numVertices);
        BufferVector3::create(normals, numVertices);
        lcore::memset(&normals[0], 0, sizeof(BufferVector3::element_type)*numVertices);
        BufferVector2::create(texcoords, numVertices);
        lcore::memset(&texcoords[0], 0, sizeof(BufferVector2::element_type)*numVertices);

        s32 numTriangles = vertices_.size()/3;
        BufferTriangle::create(triangles, numTriangles);

        AABB totalBBox;
        totalBBox.setInvalid();

        bool existNormal = false;
        for(Shape::ShapeVector::iterator itr = shapes_->begin();
            itr != shapes_->end();
            ++itr)
        {
            MeshInstance::pointer& meshInstance = reinterpret_cast<MeshInstance::pointer&>(*itr);
            s32 offset = meshInstance->getPrimitiveOffset();
            s32 num = meshInstance->getPrimitiveCount();

            AABB bbox;
            bbox.setInvalid();

            for(s32 i=offset; i<(num+offset); ++i){
                Triangle& triangle = triangles[i];

                s32 index = i*3;
                for(s32 j=0; j<3; ++j){
                    Vertex& v = vertices_[index+j];
                    triangle.indices_[j] = index+j;
                    LASSERT(0<v.position_);
                    s32 p = index+j;
                    triangle.indices_[j] = p;
                    points[p] = positions_[v.position_-1];
                    if(0<v.normal_){
                        normals[p] = normals_[v.normal_-1];
                        meshInstance->addComponent(MeshInstance::Component_Normal);
                        existNormal = true;
                    }
                    if(0<v.texcoord_){
                        texcoords[p] = texcoords_[v.texcoord_-1];
                        meshInstance->addComponent(MeshInstance::Component_Texcoord);
                    }

                    bbox.extend(points[p]);
                }//for(s32 j=0;
            }//for(s32 i=offset;
            meshInstance->setBBoxMin(bbox.bmin_);
            meshInstance->setBBoxMax(bbox.bmax_);

            totalBBox.extend(bbox);
        }//for(MeshInstanceVector

        if(0<normals.size() && !existNormal){
            generateNormals(normals, triangles, points);
        }

        if(0<normals.size() && adjustGeometryNormals_){
            adjustGeometryNormals(triangles, normals, points);
        }

        Mesh::create(
            *mesh_,
            totalBBox.bmin_,
            totalBBox.bmax_,
            points,
            normals,
            texcoords,
            triangles);

        file_.close();
        return shapes_->size();
    }

    void LoaderObj::skip()
    {
        s32 pos = file_.tellg();
        s32 c = file_.get();

        while(isspace(c)
            || c == '\n'
            || c == '\r')
        {
            if(file_.eof()
                || !file_.good())
            {
                return;
            }
            ++pos;
            c = file_.get();
        }
        file_.seekg(pos, lcore::ios::beg);
    }

    bool LoaderObj::getLine()
    {
        skip();
        line_.clear();
        
        s32 c = file_.get();
        bool comment = false;
        while(!file_.eof()
            && file_.good()
            && c != '\n'
            && c != '\r')
        {
            if(c == '#'){
                comment = true;
            }else if(!comment){
                line_.push_back(c);
            }
            c = file_.get();
        }
        if(0!=c){
            line_.push_back(c);
        }
        return (comment || 0<line_.size());
    }

    void LoaderObj::split()
    {
        elements_.clear();

        s32 i=0;
        while(i<line_.size()){
            elements_.push_back(&line_[i]);
            for(;i<line_.size(); ++i){
                if(isspace(line_[i])){
                    break;
                }
            }
            for(;i<line_.size(); ++i){
                if(!isspace(line_[i])){
                    break;
                }
                line_[i] = '\0';
            }
        }
        line_.push_back('\0');
    }

    LoaderObj::Code LoaderObj::decodeLine()
    {
        if(file_.eof() || !file_.good()){
            return Code_EOF;
        }

        if(false == getLine()){
            return Code_EOF;
        }

        split();
        if(elements_.size()<=0){
            return Code_None;
        }

        if(0 == strncmp(elements_[0], "mtllib", 6)){
            return Code_MtlLib;

        }else if(0 == strncmp(elements_[0], "usemtl", 6)){
            return Code_UseMtl;

        }else if(0 == strncmp(elements_[0], "vn", 2)){
            return Code_Normal;

        }else if(0 == strncmp(elements_[0], "vt", 2)){
            return Code_Texcoord;

        }else if(0 == strncmp(elements_[0], "v", 1)){
            return Code_Position;

        }else if(0 == strncmp(elements_[0], "f", 1)){
            return Code_Face;

        }else if(0 == strncmp(elements_[0], "o", 1)){
            return Code_Object;

        }else if(0 == strncmp(elements_[0], "g", 1)){
            return Code_Group;

        }else if(0 == strncmp(elements_[0], "s", 1)){
            return Code_Smooth;

        }
        return Code_None;
    }

    s32 LoaderObj::getFaceElements(f32 felems[4])
    {
        s32 numElements =  0;
        for(s32 i=1; i<elements_.size(); ++i){
            felems[numElements] = atof(elements_[i]);
            ++numElements;
            if(4<=numElements){
                break;
            }
        }
        return numElements;
    }

    void LoaderObj::getFaceVertex(Char* str, Vertex& v)
    {
        v.position_ = 0;
        v.normal_ = 0;
        v.texcoord_ = 0;

        s32 numElements = 1;
        Char* e[3] = {NULL, NULL, NULL};
        e[0] = str;

        for(s32 i=0; '\0'!=str[i]; ++i){
            if('/' == str[i]){
                str[i] = '\0';
                e[numElements] = &str[i+1];
                ++numElements;
                if(3<=numElements){
                    break;
                }
            }
        }

        for(s32 i=0; i<numElements; ++i){
            if(e[i][0] == '\0'){
                continue;
            }
            s32 id = atoi(e[i]);
            if(0 == id){
                continue;
            }
            switch(i)
            {
            case 0:
                v.position_ = (0<id)? id : (positions_.size() + id + 1);
                break;
            case 1:
                v.texcoord_ = (0<id)? id : (texcoords_.size() + id + 1);
                break;
            case 2:
                v.normal_ = (0<id)? id : (normals_.size() + id + 1);
                break;
            }
        }
    }

    bool LoaderObj::readLine()
    {
        Code code = decodeLine();
        f32 felems[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        s32 numElements;

        switch(code)
        {
        case Code_None:
            //LOG("Code_None");
            return true;

        case Code_Position:
            {
                numElements = getFaceElements(felems);
                positions_.push_back(lmath::Vector3(felems[0], felems[1], felems[2]));
            }
            return true;

        case Code_Normal:
            {
                numElements = getFaceElements(felems);
                lmath::Vector3 n(felems[0], felems[1], felems[2]);
                n.normalizeChecked();
                normals_.push_back(n);
            }
            return true;

        case Code_Texcoord:
            {
                numElements = getFaceElements(felems);
                texcoords_.push_back(lmath::Vector2(felems[0], felems[1]));
            }
            return true;

        case Code_Face:
            {
                if(elements_.size()<=3){
                    return true;
                }

                u32 i0 = vertices_.size();
                Vertex v;
                for(s32 i=1; i<4; ++i){
                    getFaceVertex(elements_[i], v);
                    vertices_.push_back(v);
                    //LASSERT(0<v.position_ && v.position_<=positions_.size());
                    //LASSERT(0<v.normal_ && v.normal_<=normals_.size());
                    //LASSERT(0<v.texcoord_ && v.texcoord_<=texcoords_.size());
                }
                //lcore::swap(vertices_[i0+1], vertices_[i0+2]);
                ++lastNumTriangles_;

                u32 i1 = vertices_.size()-1;
                for(s32 i=4; i<elements_.size(); ++i){
                    getFaceVertex(elements_[i], v);
                    vertices_.push_back(vertices_[i0]);
                    vertices_.push_back(vertices_[i1]);

                    i1 = vertices_.size();
                    vertices_.push_back(v);
                    ++lastNumTriangles_;;
                }
            }
            return true;

        case Code_MtlLib:
            LOG("Code_MtlLib");
            LOGELEMENTS(elements_);
            if(1<elements_.size()){
                LoaderMtl loaderMtl;
                loaderMtl.loadFromFile(bsdfs_, directory_, elements_[1]);
            }
            return true;

        case Code_UseMtl:
            LOG("Code_UseMtl");
            LOGELEMENTS(elements_);
            if(shapes_->size()<=0){
                pushMesh();

            }else if((*shapes_)[shapes_->size()-1]->getBSDF() != NULL){
                pushMesh();
            }

            if(1<elements_.size()){
                for(BSDFVector::iterator itr = bsdfs_.begin();
                    itr != bsdfs_.end();
                    ++itr)
                {
                    if(0 == (*itr)->getName().compare(elements_[1])){
                        (*shapes_)[shapes_->size()-1]->setBSDF(*itr);
                        break;
                    }
                }
            }
            return true;

        case Code_Object:
            LOG("Code_Object");
            LOGELEMENTS(elements_);
            return true;
        case Code_Group:
            LOG("Code_Group");
            LOGELEMENTS(elements_);
            pushMesh();
            return true;

        case Code_Smooth:
            LOG("Code_Smooth");
            LOGELEMENTS(elements_);
            return true;
        case Code_Comment:
            LOG("Code_Comment");
            LOGELEMENTS(elements_);
            return true;
        case Code_EOF:
            LOG("Code_EOF");
            return false;

        default:
            return false;
        };
    }

    void LoaderObj::pushMesh()
    {
        if(0<shapes_->size()){

            MeshInstance::pointer& meshInstance = getLastMeshInstance();
            meshInstance->setPrimitiveCount(lastNumTriangles_);
            lastNumTriangles_ = 0;
        }
        s32 numTriangles = vertices_.size()/3;
        shapes_->push_back(MeshInstance::pointer(LIME_NEW MeshInstance(mesh_, numTriangles, 0)));
    }

    void LoaderObj::generateNormals(BufferVector3& normals, const BufferTriangle& triangles, const BufferVector3& positions)
    {
        lcore::memset(&normals[0], 0, sizeof(Vector3)*normals.size());

        for(s32 i=0; i<triangles.size(); ++i){
            const Vector3& p0 = positions[ triangles[i].indices_[0] ];
            const Vector3& p1 = positions[ triangles[i].indices_[1] ];
            const Vector3& p2 = positions[ triangles[i].indices_[2] ];

            Vector3 d01; d01.sub(p1, p0);
            Vector3 d02; d02.sub(p2, p0);
            Vector3 n; n.cross(d01, d02);
            n.normalizeChecked();

            Vector3& n0 = normals[ triangles[i].indices_[0] ];
            Vector3& n1 = normals[ triangles[i].indices_[1] ];
            Vector3& n2 = normals[ triangles[i].indices_[2] ];
            n0 += n;
            n1 += n;
            n2 += n;
        }

        for(s32 i=0; i<normals.size(); ++i){
            normals[i].normalizeChecked();
        }
    }

    void LoaderObj::adjustGeometryNormals(BufferTriangle& triangles, const BufferVector3& normals, const BufferVector3& positions)
    {
        //シェーディングノーマルとジオメトリノーマルが反対なら, ジオメトリノーマル反転
        for(s32 i=0; i<triangles.size(); ++i){
            const Vector3& p0 = positions[triangles[i].indices_[0]];
            const Vector3& p1 = positions[triangles[i].indices_[1]];
            const Vector3& p2 = positions[triangles[i].indices_[2]];

            const Vector3& n0 = normals[triangles[i].indices_[0]];
            const Vector3& n1 = normals[triangles[i].indices_[1]];
            const Vector3& n2 = normals[triangles[i].indices_[2]];

            Vector3 e0; e0.sub(p1, p0);
            Vector3 e1; e1.sub(p2, p0);
            Vector3 ng; ng.cross(e0,e1);
            ng.normalizeChecked();

            Vector3 ns;
            ns.add(n0, n1);
            ns += n2;
            ns.normalizeChecked();

            if(ns.dot(ng)<0.0f){
                lcore::swap(triangles[i].indices_[1], triangles[i].indices_[2]);
            }
        }
    }
}
