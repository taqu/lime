/**
@file ObjConverter.cpp
@author t-sakai
@date 2016/11/27 create
*/
#include "ObjConverter.h"

#include <iostream>
#include <vector>

#include <lcore/liostream.h>
#include <lmath/geometry/AABB.h>
#include <lgraphics/Enumerations.h>

#include <lframework/resource/load_node.h>
#include <lframework/resource/load_mesh.h>
#include <lframework/resource/load_joint.h>

namespace lfw
{
namespace
{
    void convDelimiter(Char* path)
    {
        while(*path != lcore::CharNull){
            if(*path == '\\'){
                *path = lcore::PathDelimiter;
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

        buffer[count]=lcore::CharLF;
        buffer[count+1]=lcore::CharNull;
        std::cerr << buffer;

        va_end(ap);
    }

    void LogElements(const lcore::Array<Char*>& elements)
    {
        for(lcore::Array<Char*>::const_iterator itr = elements.begin();
            itr != elements.end();
            ++itr)
        {
            std::cerr << *itr << ' ';
        }
        std::cerr << std::endl;
    }

#define LOGELEMENTS(v) LogElements(v)
#define LOG(str, ...) Log((str), __VA_ARGS__)
}


    //------------------------------------------------------
    //---
    //--- WorkMaterial
    //---
    //------------------------------------------------------
    void LoaderObj::WorkMaterial::clear()
    {
        name_[0] = lcore::CharNull;

        material_.diffuse_.one();
        material_.specular_.one();
        material_.ambient_.set(0.0f, 0.0f, 0.0f, 1.33f);
        material_.shadow_.zero();
        material_.flags_ = MaterialFlag_CastShadow | MaterialFlag_RecieveShadow | MaterialFlag_RefractiveIndex;
        for(s32 i=0; i<TextureType_Num; ++i){
            material_.textureIDs_[i] = TextureType_Invalid;
        }
    }

    void LoaderObj::WorkMaterial::setName(const Char* name)
    {
        lcore::strncpy(name_, MaxNameSize, name, lcore::strlen(name));
    }

    //------------------------------------------------------
    //---
    //--- MaterialPack
    //---
    //------------------------------------------------------
    s32 LoaderObj::MaterialPack::findMaterialIndex(const Char* name) const
    {
        for(s32 i=0; i<materials_.size(); ++i){
            if(0 == lcore::strncmp(name, materials_[i].name_, WorkMaterial::MaxNameSize)){
                return i;
            }
        }
        return -1;
    }

    s32 LoaderObj::MaterialPack::findTextureIndex(const Char* name) const
    {
        for(s32 i=0; i<textures_.size(); ++i){
            if(0 == lcore::strncmp(name, textures_[i].name_, MaxFileNameSize)){
                return i;
            }
        }
        return -1;
    }

    //------------------------------------------------------
    //---
    //--- LoaderMtl
    //---
    //------------------------------------------------------
    LoaderObj::LoaderMtl::LoaderMtl()
        :directory_(NULL)
        ,material_(NULL)
    {
    }

    LoaderObj::LoaderMtl::~LoaderMtl()
    {
    }

    s32 LoaderObj::LoaderMtl::loadFromFile(MaterialPack& materialPack, const Char* directory, const Char* filename)
    {
        directory_ = directory;

        s32 len = 0;
        s32 dlen = 0;
        if(NULL != directory){
            dlen = lcore::strlen(directory);
            len += dlen;
        }
        len += lcore::strlen(filename) + 1;

        Char* path = LNEW Char[len];
        path[0] = lcore::CharNull;
        if(NULL != directory){
            lcore::strcat(path, len, directory);
            //len -= dlen;
        }
        lcore::strcat(path, len, filename);

        file_.open(path, lcore::ios::binary);
        LDELETE(path);

        if(!file_.is_open()){
            return 0;
        }

        material_ = NULL;

        while(readLine(materialPack));
        return materialPack.materials_.size();
    }

    void LoaderObj::LoaderMtl::skip()
    {
        s32 pos = file_.tellg();
        s32 c = file_.get();

        while(isspace(c)
            || c == lcore::CharLF
            || c == lcore::CharCR)
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
            && c != lcore::CharLF
            && c != lcore::CharCR)
        {
            if(c == '#'){
                comment = true;
            }else if(!comment){
                line_.push_back((Char)c);
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
                line_[i] = lcore::CharNull;
            }
        }
        line_.push_back(lcore::CharNull);
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
            f = static_cast<f32>( atof(elements_[1]) );
        }else{
            f = 0.0f;
        }
    }

    void LoaderObj::LoaderMtl::load(lmath::Vector3& v)
    {
        v.zero();
        s32 numElements =  0;
        for(s32 i=1; i<elements_.size(); ++i){
            v[numElements] = static_cast<f32>( atof(elements_[i]) );
            ++numElements;
            if(3<=numElements){
                break;
            }
        }
    }


    void LoaderObj::LoaderMtl::load(lmath::Vector4& v)
    {
        v.zero();
        s32 numElements =  0;
        for(s32 i=1; i<elements_.size(); ++i){
            v[numElements] = static_cast<f32>( atof(elements_[i]) );
            ++numElements;
            if(4<=numElements){
                break;
            }
        }
    }

    LoadTexture LoaderObj::LoaderMtl::loadTexture(const Char* filename)
    {
        LoadTexture texture;
        lcore::strncpy(texture.name_, MaxFileNameSize, filename, lcore::strlen(filename));
        texture.addressUVW_ = lgfx::TexAddress_Clamp;
        texture.filterType_ = lgfx::TexFilter_MinMagMipLinear;
        texture.compFunc_ = lgfx::Cmp_Never;
        texture.borderColor_ = 0.0f;
        return texture;
    }

    bool LoaderObj::LoaderMtl::readLine(MaterialPack& materialPack)
    {
        CodeMtl code = decodeLine();

        if(CodeMtl_EOF == code){
            return false;

        }else if(CodeMtl_NewMtl == code){
            if(1<elements_.size()){
                s32 index = materialPack.findMaterialIndex(elements_[1]);
                if(0<=index){
                    material_ = &materialPack.materials_[index];

                } else{
                    WorkMaterial tmp;
                    tmp.clear();
                    tmp.setName(elements_[1]);
                    materialPack.materials_.push_back(tmp);
                    material_ = &materialPack.materials_[materialPack.materials_.size() - 1];
                }
            }
            return true;
        }

        if(NULL == material_){
            return true;
        }

        switch(code)
        {
        case CodeMtl_None:
            return true;

        //case CodeMtl_NewMtl:
        //    return true;

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
                material_->material_.specular_.w_ = alpha;
            }
            return true;

        case CodeMtl_RefracttionIndex:
            {
                f32 refractiveIndex = 1.01f;
                load(refractiveIndex);
                material_->material_.ambient_.w_ = refractiveIndex;
            }
            return true;

        case CodeMtl_Ambient:
            {
                lmath::Vector3 ambient;
                load(ambient);
                material_->material_.ambient_.x_ = ambient.x_;
                material_->material_.ambient_.y_ = ambient.y_;
                material_->material_.ambient_.z_ = ambient.z_;
            }
            return true;

        case CodeMtl_Diffuse:
            {
                lmath::Vector3 diffuse;
                load(diffuse);
                material_->material_.diffuse_.x_ = diffuse.x_;
                material_->material_.diffuse_.y_ = diffuse.y_;
                material_->material_.diffuse_.z_ = diffuse.z_;
            }
            return true;

        case CodeMtl_Specular:
            {
                lmath::Vector3 specular;
                load(specular);
                material_->material_.specular_.x_ = specular.x_;
                material_->material_.specular_.y_ = specular.y_;
                material_->material_.specular_.z_ = specular.z_;
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
                if(1<elements_.size()){
                    s32 index = materialPack.findTextureIndex(elements_[1]);
                    if(0<=index){
                        material_->material_.textureIDs_[TextureType_Albedo] = index;

                    }else{
                        LoadTexture texture = loadTexture(elements_[1]);
                        texture.type_ = TextureType_Albedo;
                        material_->material_.textureIDs_[TextureType_Albedo] = materialPack.textures_.size();
                        materialPack.textures_.push_back(texture);
                    }
                }
            }
            return true;

        case CodeMtl_MapBump:
            {
                if(1<elements_.size()){
                    s32 index = materialPack.findTextureIndex(elements_[1]);
                    if(0<=index){
                        material_->material_.textureIDs_[TextureType_Normal] = index;

                    } else{
                        LoadTexture texture = loadTexture(elements_[1]);
                        texture.type_ = TextureType_Normal;
                        material_->material_.textureIDs_[TextureType_Normal] = materialPack.textures_.size();
                        materialPack.textures_.push_back(texture);
                    }
                }

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
    LoaderObj::LoaderObj(bool generateNormals)
        :directory_(NULL)
        ,generateNormals_(generateNormals)
        ,lastNumTriangles_(0)
        ,currentMaterial_(0)
    {
    }

    LoaderObj::~LoaderObj()
    {
        LDELETE_ARRAY(directory_);
    }

    bool LoaderObj::load(const Char* filepath)
    {
        LASSERT(NULL != filepath);
        if(false == file_.open(filepath)){
            return false;
        }

        s32 len = lcore::strlen(filepath);
        LDELETE_ARRAY(directory_);
        directory_ = LNEW Char[len + 2];
        s32 dlen = lcore::extractDirectoryPath(directory_, len, filepath);
        convDelimiter(directory_);
        if(0<dlen && directory_[dlen-1] != lcore::PathDelimiter){
            directory_[dlen] = lcore::PathDelimiter;
            directory_[dlen+1] = lcore::CharNull;
        }

        lastNumTriangles_ = 0;

        currentMaterial_ = 0;
        materialPack_.materials_.clear();
        materialPack_.textures_.clear();

        positions_.clear();
        normals_.clear();
        texcoords_.clear();
        vertices_.clear();

        meshes_.clear();

        line_.reserve(1024);
        elements_.clear();

        while(readLine());
        file_.close();

        if(materialPack_.materials_.size()<=0){
            //マテリアルがない
            WorkMaterial tmpMaterial;
            tmpMaterial.clear();
            materialPack_.materials_.push_back(tmpMaterial);
        }
        if(meshes_.size()<=0){
            //メッシュがない
            WorkMesh tmpMesh;
            tmpMesh.geometry_ = 0;
            tmpMesh.material_ = 0;
            tmpMesh.indexOffset_ = 0;
            tmpMesh.numIndices_ = 0;
            meshes_.push_back(tmpMesh);
        }
        meshes_[meshes_.size()-1].numIndices_ = lastNumTriangles_ * 3;
        lastNumTriangles_ = 0;

        return true;
    }

    void LoaderObj::skip()
    {
        s32 pos = file_.tellg();
        s32 c = file_.get();

        while(isspace(c)
            || c == lcore::CharLF
            || c == lcore::CharCR)
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
            && c != lcore::CharLF
            && c != lcore::CharCR)
        {
            if(c == '#'){
                comment = true;
            }else if(!comment){
                line_.push_back((Char)c);
            }
            c = file_.get();
        }
        if(0!=c){
            line_.push_back((Char)c);
        }
        return (comment || 0<line_.size());
    }

    void LoaderObj::split()
    {
        elements_.clear();

        u32 i=0;
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
                line_[i] = lcore::CharNull;
            }
        }
        line_.push_back(lcore::CharNull);
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
            felems[numElements] = static_cast<f32>( atof(elements_[i]) );
            ++numElements;
            if(4<=numElements){
                break;
            }
        }
        return numElements;
    }

    void LoaderObj::getFaceVertex(Char* str, Vertex& v)
    {
        v.position_ = -1;
        v.normal_ = -1;
        v.texcoord_ = -1;
        v.material_ = currentMaterial_;

        s32 numElements = 1;
        Char* e[3] = {NULL, NULL, NULL};
        e[0] = str;

        for(s32 i=0; lcore::CharNull!=str[i]; ++i){
            if(lcore::PathDelimiter == str[i]){
                str[i] = lcore::CharNull;
                e[numElements] = &str[i+1];
                ++numElements;
                if(3<=numElements){
                    break;
                }
            }
        }

        for(s32 i=0; i<numElements; ++i){
            if(e[i][0] == lcore::CharNull){
                continue;
            }
            s32 id = atoi(e[i]);
            if(0 == id){
                continue;
            }
            switch(i)
            {
            case 0:
                v.position_ = (0<id)? id-1 : (positions_.size() + id);
                break;
            case 1:
                v.texcoord_ = (0<id)? id-1 : (texcoords_.size() + id);
                break;
            case 2:
                v.normal_ = (0<id)? id-1 : (normals_.size() + id);
                break;
            }
        }

        if(0<=v.normal_ && 0<=v.texcoord_){
            v.type_ = VType_PNU;
        }else if(0<=v.normal_ && v.texcoord_<0){
            v.type_ = VType_PN;
        }else if(v.normal_<0 && 0<=v.texcoord_){
            v.type_ = VType_PU;
        }else{
            v.type_ = VType_P;
        }
    }

    bool LoaderObj::readLine()
    {
        LoaderObj::Code code = decodeLine();
        f32 felems[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        s32 numElements;

        switch(code)
        {
        case Code_None:
            LOG("Code_None");
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
                n = normalizeChecked(n);
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
                    ++lastNumTriangles_;
                }
            }
            return true;

        case Code_MtlLib:
            LOG("Code_MtlLib");
            LOGELEMENTS(elements_);
            if(1<elements_.size()){
                LoaderMtl loaderMtl;
                loaderMtl.loadFromFile(materialPack_, directory_, elements_[1]);
            }
            return true;

        case Code_UseMtl:
            LOG("Code_UseMtl");
            LOGELEMENTS(elements_);
            if(1<elements_.size()){
                currentMaterial_ = materialPack_.findMaterialIndex(elements_[1]);
            }
            return true;

        case Code_Object:
            LOG("Code_Object");
            LOGELEMENTS(elements_);
            return true;
        case Code_Group:
            LOG("Code_Group");
            LOGELEMENTS(elements_);
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
        if(0<meshes_.size()){
            WorkMesh& mesh = meshes_[meshes_.size()-1];
            mesh.numIndices_ = lastNumTriangles_ * 3;
            lastNumTriangles_ = 0;
        }
        WorkMesh tmp;
        tmp.geometry_ = 0;
        tmp.material_ = currentMaterial_;
        tmp.indexOffset_ = vertices_.size();
        tmp.numIndices_ = 0;
        meshes_.push_back(tmp);
    }

    void LoaderObj::setVertexProperty(LoadGeometry& geometry)
    {
        u32 vflag = 0;
        vflag |= (0<positions_.size())? VElem_Position : 0;
        vflag |= (0<normals_.size())? VElem_Normal : 0;
        vflag |= (0<texcoords_.size())? VElem_Texcoord : 0;

        u32 vsize = 0;
        vsize += (0<positions_.size())? VSize_Position : 0;
        vsize += (0<normals_.size())? VSize_Normal : 0;
        vsize += (0<texcoords_.size())? VSize_Texcoord : 0;

        geometry.vflag_ = vflag;
        geometry.vsize_ = vsize;
    }

    void LoaderObj::push_back_scale(Vector3Vector& v, const lmath::Vector3& p, f32 scale)
    {
        lmath::Vector3 tmp;
        tmp = p;
        tmp *= scale;
        v.push_back(tmp);
    }

    void LoaderObj::push_back(U16Vector4Vector& v, const lmath::Vector3& n)
    {
        lmath::Vector3 t = normalize(n);

        U16Vector4 tmp;
        tmp.x_ = lcore::toBinary16Float(t.x_);
        tmp.y_ = lcore::toBinary16Float(t.y_);
        tmp.z_ = lcore::toBinary16Float(t.z_);
        tmp.w_ = 0;
        v.push_back(tmp);
    }

    void LoaderObj::push_back(U16Vector2Vector& v, const lmath::Vector2& t)
    {
        U16Vector2 tmp;
        tmp.x_ = lcore::toBinary16Float(t.x_);
        tmp.y_ = lcore::toBinary16Float(t.y_);
        v.push_back(tmp);
    }

    void LoaderObj::save(const Char* filepath, f32 scale)
    {
        if(vertices_.size()<=0){
            return;
        }

        LASSERT(NULL != filepath);
        lcore::ofstream file(filepath);
        if(!file.is_open()){
            return;
        }

        std::vector< std::vector<Vector3Vector> > tmpPositions[VType_Num];
        std::vector< std::vector<U16Vector4Vector> > tmpNormals[VType_Num];
        std::vector< std::vector<U16Vector2Vector> > tmpTexcoords[VType_Num];
        std::vector< std::vector<S32Vector> > tmpIndices[VType_Num];
        std::vector< std::vector<S32Map> > indexToIndex[VType_Num];

        for(s32 i=0; i<VType_Num; ++i){
            tmpPositions[i].resize( materialPack_.materials_.size() );
            tmpNormals[i].resize( materialPack_.materials_.size() );
            tmpTexcoords[i].resize( materialPack_.materials_.size() );
            tmpIndices[i].resize( materialPack_.materials_.size() );
            indexToIndex[i].resize( materialPack_.materials_.size() );

            for(u32 j=0; j<materialPack_.materials_.size(); ++j){
                tmpPositions[i][j].push_back(Vector3Vector());
                tmpNormals[i][j].push_back(U16Vector4Vector());
                tmpTexcoords[i][j].push_back(U16Vector2Vector());
                tmpIndices[i][j].push_back(S32Vector());
                indexToIndex[i][j].push_back(S32Map());
            }
        }

        for(u32 i=0; i<vertices_.size(); ++i){
            const Vertex& v = vertices_[i];

            s32 n = tmpPositions[v.type_][v.material_].size() - 1;
            if(MaxVertices<=tmpPositions[v.type_][v.material_][n].size()){
                tmpPositions[v.type_][v.material_].push_back(Vector3Vector());
                tmpNormals[v.type_][v.material_].push_back(U16Vector4Vector());
                tmpTexcoords[v.type_][v.material_].push_back(U16Vector2Vector());
                tmpIndices[v.type_][v.material_].push_back(S32Vector());
                indexToIndex[v.type_][v.material_].push_back(S32Map());
                ++n;
            }
            Vector3Vector& positions = tmpPositions[v.type_][v.material_][n];
            U16Vector4Vector& normals = tmpNormals[v.type_][v.material_][n];
            U16Vector2Vector& tecoords = tmpTexcoords[v.type_][v.material_][n];
            S32Vector& indices = tmpIndices[v.type_][v.material_][n];
            S32Map& indexMap = indexToIndex[v.type_][v.material_][n];

            S32Map::iterator itr = indexMap.find( v.position_ );
            if(indexMap.end() == itr){
                indexMap[v.position_] = positions.size();

                switch(v.type_)
                {
                case VType_P:
                    {
                        positions.push_back( positions_[v.position_] );
                        push_back_scale(positions, positions_[v.position_], scale);
                    }
                    break;
                case VType_PU:
                    {
                        push_back_scale(positions, positions_[v.position_], scale);
                        push_back(tecoords, texcoords_[v.texcoord_]);
                    }
                    break;
                case VType_PN:
                    {
                        push_back_scale(positions, positions_[v.position_], scale);
                        push_back(normals, normals_[v.normal_]);
                    }
                    break;
                case VType_PNU:
                    {
                        push_back_scale(positions, positions_[v.position_], scale);
                        push_back(normals, normals_[v.normal_]);
                        push_back(tecoords, texcoords_[v.texcoord_]);
                    }
                    break;
                }

            }
            indices.push_back( v.position_ );
        }

        std::vector<LoadGeometry> geometries;
        std::vector<LoadMesh> meshes;
        for(s32 i=0; i<VType_Num; ++i){
            LoadGeometry geometry;
            switch(i)
            {
            case VType_P:
                geometry.vflag_ = VElem_Position;
                geometry.vsize_ = VSize_Position;
                break;

            case VType_PU:
                geometry.vflag_ = VElem_Position | VElem_Texcoord;
                geometry.vsize_ = VSize_Position + VSize_Texcoord;
                break;

            case VType_PN:
                geometry.vflag_ = VElem_Position | VElem_Normal;
                geometry.vsize_ = VSize_Position + VSize_Normal;
                break;

            case VType_PNU:
                geometry.vflag_ = VElem_Position | VElem_Normal | VElem_Texcoord;
                geometry.vsize_ = VSize_Position + VSize_Normal + VSize_Texcoord;
                break;
            };

            for(u32 j=0; j<materialPack_.materials_.size(); ++j){

                for(u32 k=0; k<tmpPositions[i][j].size(); ++k){
                    if(tmpIndices[i][j][k].size()<=0){
                        continue;
                    }
                    Vector3Vector& positions = tmpPositions[i][j][k];
                    U16Vector4Vector& normals = tmpNormals[i][j][k];
                    U16Vector2Vector& tecoords = tmpTexcoords[i][j][k];
                    S32Vector& indices = tmpIndices[i][j][k];

                    LoadMesh mesh;
                    mesh.geometry_ = static_cast<s16>(geometries.size());
                    mesh.material_ = static_cast<s32>(j);
                    mesh.indexOffset_ = 0;
                    mesh.numIndices_ = indices.size();
                    mesh.sphere_ = lmath::Sphere::calcMiniSphere(&positions[0], positions.size());
                    meshes.push_back(mesh);

                    geometry.numVertices_ = positions.size();
                    geometry.numIndices_ = indices.size();
                    geometries.push_back(geometry);

                    LOG("mesh{ geom:%d, mat:%d, indices:%d }", mesh.geometry_, mesh.material_, mesh.numIndices_);
                    LOG("geometry{ vertices:%d, indices:%d }", geometry.numVertices_, geometry.numIndices_);
                }
            }
        }

        LoadHeader header;
        header.major_ = 1;
        header.minor_ = 0;
        
        u32 offset = sizeof(LoadHeader);
        header.elems_[Elem_Geometry].number_ = geometries.size();
        header.elems_[Elem_Geometry].offset_ = offset;
        offset += sizeof(LoadGeometry) * geometries.size();

        header.elems_[Elem_Material].number_ = materialPack_.materials_.size();
        header.elems_[Elem_Material].offset_ = offset;
        offset += sizeof(LoadMaterial) * materialPack_.materials_.size();

        header.elems_[Elem_Mesh].number_ = meshes.size();
        header.elems_[Elem_Mesh].offset_ = offset;
        offset += sizeof(LoadMesh) * meshes.size();

        header.elems_[Elem_Node].number_ = 1;
        header.elems_[Elem_Node].offset_ = offset;
        offset += sizeof(LoadNode) * 1;

        header.elems_[Elem_Joint].number_ = 0;
        header.elems_[Elem_Joint].offset_ = offset;
        offset += sizeof(LoadJoint) * 0;

        header.elems_[Elem_Texture].number_ = materialPack_.textures_.size();
        header.elems_[Elem_Texture].offset_ = offset;
        offset += sizeof(LoadTexture) * materialPack_.textures_.size();

        //-------------------------------------------
        lcore::io::write(file, header);

        s32 numGeometries = 0;
        for(s32 i=0; i<VType_Num; ++i){
            LoadGeometry geometry;
            switch(i)
            {
            case VType_P:
                geometry.vflag_ = VElem_Position;
                geometry.vsize_ = VSize_Position;
                break;

            case VType_PU:
                geometry.vflag_ = VElem_Position | VElem_Texcoord;
                geometry.vsize_ = VSize_Position + VSize_Texcoord;
                break;

            case VType_PN:
                geometry.vflag_ = VElem_Position | VElem_Normal;
                geometry.vsize_ = VSize_Position + VSize_Normal;
                break;

            case VType_PNU:
                geometry.vflag_ = VElem_Position | VElem_Normal | VElem_Texcoord;
                geometry.vsize_ = VSize_Position + VSize_Normal + VSize_Texcoord;
                break;
            };

            for(u32 j=0; j<materialPack_.materials_.size(); ++j){

                for(u32 k=0; k<tmpPositions[i][j].size(); ++k){
                    if(tmpIndices[i][j][k].size()<=0){
                        continue;
                    }
                    Vector3Vector& positions = tmpPositions[i][j][k];
                    U16Vector4Vector& normals = tmpNormals[i][j][k];
                    U16Vector2Vector& tecoords = tmpTexcoords[i][j][k];
                    S32Vector& indices = tmpIndices[i][j][k];
                    S32Map& indexMap = indexToIndex[i][j][k];

                    lcore::io::write(file, geometries[numGeometries]);

                    switch(i)
                    {
                    case VType_P:
                        for(u32 h=0; h<positions.size(); ++h){
                            lcore::io::write(file, positions[h]);
                        }
                        break;

                    case VType_PU:
                        for(u32 h=0; h<positions.size(); ++h){
                            lcore::io::write(file, positions[h]);
                            lcore::io::write(file, tecoords[h]);
                        }
                        break;

                    case VType_PN:
                        for(u32 h=0; h<positions.size(); ++h){
                            lcore::io::write(file, positions[h]);
                            lcore::io::write(file, normals[h]);
                        }
                        break;

                    case VType_PNU:
                        for(u32 h=0; h<positions.size(); ++h){
                            lcore::io::write(file, positions[h]);
                            lcore::io::write(file, normals[h]);
                            lcore::io::write(file, tecoords[h]);
                        }
                        break;
                    };

                    for(u32 i=0; i<indices.size(); ++i){
                        u16 index = static_cast<u16>(indexMap[indices[i]]);
                        lcore::io::write(file, index);
                    }
                    ++numGeometries;
                }
            }
        }


        for(u32 i=0; i<materialPack_.materials_.size(); ++i){
            lcore::io::write(file, materialPack_.materials_[i].material_);
        }

        for(u32 i=0; i<meshes.size(); ++i){
            lcore::io::write(file, meshes[i]);
        }

        LoadNode node;
        node.index_ = 0;
        node.parentIndex_ = InvalidNode;
        node.childStartIndex_ = InvalidNode;
        node.numChildren_ = 0;
        node.meshStartIndex_ = 0;
        node.numMeshes_ = static_cast<u8>(meshes.size());
        lcore::io::write(file, node);


        for(u32 i=0; i<materialPack_.textures_.size(); ++i){
            lcore::io::write(file, materialPack_.textures_[i]);
        }

        file.close();
    }
}
