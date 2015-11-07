#include "ObjConverter.h"

#include <stdarg.h>
#include <iostream>

#include <lcore/liostream.h>
#include <lframework/scene/load/load_node.h>
#include <lframework/scene/load/load_mesh.h>
#include <lframework/scene/load/load_geometry.h>
#include <lframework/scene/load/load_material.h>
#include <lframework/scene/load/load_joint.h>
#include <lframework/scene/load/load_texture.h>

namespace lscene
{
namespace lload
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
        std::cerr << buffer;

        va_end(ap);
    }

    void LogElements(const std::vector<Char*>& elements)
    {
        for(std::vector<Char*>::const_iterator itr = elements.begin();
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

    LoaderObj::LoaderObj()
        :directory_(NULL)
        ,currentMaterialId_(0)
    {
    }

    LoaderObj::~LoaderObj()
    {
        LIME_DELETE_ARRAY(directory_);
    }

    bool LoaderObj::load(const Char* filepath)
    {
        LASSERT(NULL != filepath);
        std::ifstream input(filepath, std::ios::in|std::ios::binary);
        if(false == input.is_open()){
            return false;
        }

        s32 len = lcore::strlen(filepath);
        LIME_DELETE_ARRAY(directory_);
        directory_ = LIME_NEW Char[len + 2];
        s32 dlen = lcore::extractDirectoryPath(directory_, filepath, len);
        convDelimiter(directory_);
        if(0<dlen && directory_[dlen-1] != '/'){
            directory_[dlen] = '/';
            directory_[dlen+1] = '\0';
        }

        currentMaterialId_ = 0;
        currentMaterial_.clear();

        vertices_.clear();

        while(readLine(input));
        return true;
    }

    void LoaderObj::skip(std::ifstream& input)
    {
        s32 c = input.peek();
        while(isspace(c)
            || c == '\n'
            || c == '\r')
        {
            input.get();
            if(input.eof()
                || input.bad())
            {
                return;
            }
            c = input.peek();
        }
    }

    bool LoaderObj::getLine(std::ifstream& input)
    {
        skip(input);
        line_.clear();
        
        bool comment = false;
        s32 c = input.get();
        while(!input.eof()
            && input.good()
            && c != '\n'
            && c != '\r')
        {
            if(c == '#'){
                comment = true;
            }else if(!comment){
                line_.push_back(c);
            }
            c = input.get();
        }
        return (comment || 0<line_.size());
    }

    void LoaderObj::split()
    {
        elems_.clear();

        u32 i=0;
        while(i<line_.size()){
            elems_.push_back(&line_[i]);
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

    void LoaderObj::getFElems(f32 felems[4])
    {
        s32 numElements =  0;
        for(s32 i=1; i<elems_.size(); ++i){
            felems[numElements] = atof(elems_[i]);
            ++numElements;
            if(4<=numElements){
                break;
            }
        }
    }

    void LoaderObj::getFaceVertex(Char* str, Vertex& v)
    {
        v.position_ = -1;
        v.normal_ = -1;
        v.texcoord_ = -1;
        v.material_ = currentMaterialId_;

        s32* vp = reinterpret_cast<s32*>(&v);

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
            s32 id = atoi(e[i]) - 1;
            vp[i] = id;
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

    LoaderObj::Code LoaderObj::decodeLine(std::ifstream& input)
    {
        if(input.eof() || input.bad()){
            return Code_EOF;
        }

        if(false == getLine(input)){
            return Code_EOF;
        }

        split();
        if(elems_.size()<=0){
            return Code_None;
        }

        if(0 == strncmp(elems_[0], "mtllib", 6)){
            return Code_MtlLib;

        }else if(0 == strncmp(elems_[0], "usemtl", 6)){
            return Code_UseMtl;

        }else if(0 == strncmp(elems_[0], "vn", 2)){
            return Code_Normal;

        }else if(0 == strncmp(elems_[0], "vt", 2)){
            return Code_Texcoord;

        }else if(0 == strncmp(elems_[0], "v", 1)){
            return Code_Position;

        }else if(0 == strncmp(elems_[0], "f", 1)){
            return Code_Face;

        }else if(0 == strncmp(elems_[0], "o", 1)){
            return Code_Object;

        }else if(0 == strncmp(elems_[0], "g", 1)){
            return Code_Group;

        }else if(0 == strncmp(elems_[0], "s", 1)){
            return Code_Smooth;

        }
        return Code_None;
    }

    bool LoaderObj::readLine(std::ifstream& input)
    {
        LoaderObj::Code code = decodeLine(input);
        f32 felems[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        switch(code)
        {
        case Code_None:
            LOG("Code_None");
            return true;

        case Code_Position:
            {
                getFElems(felems);
                positions_.push_back(lmath::Vector3(felems[0], felems[1], felems[2]));
            }
            return true;

        case Code_Normal:
            {
                getFElems(felems);
                normals_.push_back(lmath::Vector3(felems[0], felems[1], felems[2]));
            }
            return true;

        case Code_Texcoord:
            {
                getFElems(felems);
                texcoords_.push_back(lmath::Vector2(felems[0], felems[1]));
            }
            return true;

        case Code_Face:
            {
                if(elems_.size()<=3){
                    return true;
                }

                VertexVector& vertices = vertices_;
                u32 i0 = vertices.size();
                Vertex v;
                for(s32 i=1; i<4; ++i){
                    getFaceVertex(elems_[i], v);
                    vertices.push_back(v);
                }

                u32 i1 = vertices.size()-1;
                for(s32 i=4; i<elems_.size(); ++i){
                    getFaceVertex(elems_[i], v);
                    vertices.push_back(vertices[i0]);
                    vertices.push_back(vertices[i1]);

                    i1 = vertices.size();
                    vertices.push_back(v);
                }
            }
            return true;

        case Code_MtlLib:
            LOG("Code_MtlLib");
            LOGELEMENTS(elems_);
            if(1<elems_.size()){
                loadMtl(elems_[1]);
            }
            return true;

        case Code_UseMtl:
            LOG("Code_UseMtl");
            LOGELEMENTS(elems_);
            if(1<elems_.size()){
                currentMaterial_ = elems_[1];
                currentMaterialId_ = getMaterial(currentMaterial_);
            }
            return true;

        case Code_Object:
            LOG("Code_Object");
            LOGELEMENTS(elems_);
            return true;
        case Code_Group:
            LOG("Code_Group");
            LOGELEMENTS(elems_);
            return true;
        case Code_Smooth:
            LOG("Code_Smooth");
            LOGELEMENTS(elems_);
            return true;
        case Code_Comment:
            LOG("Code_Comment");
            LOGELEMENTS(elems_);
            return true;
        case Code_EOF:
            LOG("Code_EOF");
            return false;

        default:
            return false;
        };
    }

    void LoaderObj::getInt(s32& dst)
    {
        if(1<elems_.size()){
            dst = atoi(elems_[1]);
        }else{
            dst = 0;
        }
    }

    void LoaderObj::getFloat(f32& dst)
    {
        if(1<elems_.size()){
            dst = atof(elems_[1]);
        }else{
            dst = 0.0f;
        }
    }

    void LoaderObj::getFloat3(lmath::Vector3& dst)
    {
        dst.zero();
        s32 numElements =  0;
        for(s32 i=1; i<elems_.size(); ++i){
            dst[numElements] = atof(elems_[i]);
            ++numElements;
            if(3<=numElements){
                break;
            }
        }
    }

    s32 LoaderObj::addMaterial(const std::string& name)
    {
        NameToMaterialMap::iterator itr = materials_.find(name);
        if(materials_.end() != itr){
            return itr->second;
        }

        Material material;
        material.diffuse_.zero();
        material.specular_.zero();
        material.shadow_.zero();
        material.shadow_.w_ = 1.0f;
        material.flags_ = Material::Flag_CastShadow | Material::Flag_RecieveShadow;
        material.texColor_ = -1;
        material.texNormal_ = -1;

        s32 id = static_cast<s32>(materialVector_.size());
        materialVector_.push_back(material);
        materials_[name] = id;
        return id;
    }

    s32 LoaderObj::getMaterial(const std::string& name)
    {
        return addMaterial(name);
    }

    void LoaderObj::loadTexture(const Char* name)
    {
        NameToTextureMap::iterator itr = textures_.find(name);
        if(textures_.end() != itr){
            return;
        }

        LASSERT(NULL != name);
        s32 len = lcore::strlen(name);
        len = (MaxFileNameSize<=len)? MaxFileNameSize-1 : len;
        Texture texture;
        lcore::memcpy(texture.name_, name, len);
        texture.name_[len] = '\0';

        s32 id = static_cast<s32>(textureVector_.size());
        textureVector_.push_back(texture);
        textures_[std::string(name)] = id;

    }

    s32 LoaderObj::getTexture(const Char* name)
    {
        LASSERT(NULL != name);
        NameToTextureMap::iterator itr = textures_.find(name);
        if(textures_.end() != itr){
            return itr->second;
        }
        return -1;
    }

    bool LoaderObj::loadMtl(const Char* filepath)
    {
        std::string path;
        if(NULL != directory_){
            path.append(directory_);
        }
        path.append(filepath);
        std::ifstream input(path.c_str(), std::ios::in|std::ios::binary);
        if(false == input.is_open()){
            return false;
        }

        while(readLineMtl(input));
        return true;
    }

    LoaderObj::CodeMtl LoaderObj::decodeLineMtl(std::ifstream& input)
    {
        if(input.eof() || input.bad()){
            return CodeMtl_EOF;
        }

        if(false == getLine(input)){
            return CodeMtl_EOF;
        }

        split();

        if(elems_.size()<=0){
            return CodeMtl_None;
        }

        if(0 == strncmp(elems_[0], "newmtl", 6)){
            return CodeMtl_NewMtl;

        }else if(0 == strncmp(elems_[0], "map_Kd", 6)){
            return CodeMtl_MapDiffuse;

        }else if(0 == strncmp(elems_[0], "map_bump", 8)){
            return CodeMtl_MapBump;

        }else if(0 == strncmp(elems_[0], "illum", 5)){
            return CodeMtl_Illumination;

        }else if(0 == strncmp(elems_[0], "Ns", 2)){
            return CodeMtl_SpecularExponent;

        }else if(0 == strncmp(elems_[0], "Ni", 2)){
            return CodeMtl_RefracttionIndex;

        }else if(0 == strncmp(elems_[0], "Ka", 2)){
            return CodeMtl_Ambient;

        }else if(0 == strncmp(elems_[0], "Kd", 2)){
            return CodeMtl_Diffuse;

        }else if(0 == strncmp(elems_[0], "Ks", 2)){
            return CodeMtl_Specular;

        }else if(0 == strncmp(elems_[0], "Tf", 2)){
            return CodeMtl_Transmission;

        }else if(0 == strncmp(elems_[0], "d", 1)){
            return CodeMtl_Dissolve;

        }
        return CodeMtl_None;
    }

    bool LoaderObj::readLineMtl(std::ifstream& input)
    {
        LoaderObj::CodeMtl code = decodeLineMtl(input);

        if(CodeMtl_EOF == code){
            return false;

        }else if(CodeMtl_NewMtl == code){
            currentMaterial_.clear();
            if(1<elems_.size()){
                currentMaterial_ = elems_[1];

            }
        }

        if(currentMaterial_.size()<=0){
            return true;
        }

        Material& material = materialVector_[ getMaterial(currentMaterial_) ];

        switch(code)
        {
        case CodeMtl_None:
            return true;

        case CodeMtl_NewMtl:
            return true;

        case CodeMtl_Model:
            return true;

        case CodeMtl_Dissolve:
            getFloat(material.diffuse_.w_);
            return true;

        case CodeMtl_SpecularExponent:
            {
                getFloat(material.specular_.w_);
            }
            return true;

        case CodeMtl_RefracttionIndex:
            {
                getFloat(material.shadow_.w_);
                material.ambient_.w_ = lcore::calcFresnelTerm(material.shadow_.w_);
                material.flags_ |= Material::Flag_RefractiveIndex;
            }
            return true;

        case CodeMtl_Ambient:
            {
                lmath::Vector3 ambient;
                getFloat3(ambient);
                material.ambient_.x_ = ambient.x_;
                material.ambient_.y_ = ambient.y_;
                material.ambient_.z_ = ambient.z_;
            }
            return true;

        case CodeMtl_Diffuse:
            {
                lmath::Vector3 diffuse;
                getFloat3(diffuse);
                material.diffuse_.x_ = diffuse.x_;
                material.diffuse_.y_ = diffuse.y_;
                material.diffuse_.z_ = diffuse.z_;
            }
            return true;

        case CodeMtl_Specular:
            {
                lmath::Vector3 specular;
                getFloat3(specular);
                material.specular_.x_ = specular.x_;
                material.specular_.y_ = specular.y_;
                material.specular_.z_ = specular.z_;
            }
            return true;

        case CodeMtl_Transmission:
            {
            }
            return true;

        case CodeMtl_MapDiffuse:
            {
                if(1<elems_.size()){
                    convDelimiter(elems_[1]);
                    loadTexture(elems_[1]);
                    s32 id = getTexture(elems_[1]);
                    material.texColor_ = (0<=id)? static_cast<s16>(id) : -1;
                }
            }
            return true;

        case CodeMtl_MapBump:
            {
                if(1<elems_.size()){
                    convDelimiter(elems_[1]);
                    loadTexture(elems_[1]);
                    s32 id = getTexture(elems_[1]);
                    material.texNormal_ = (0<=id)? static_cast<s16>(id) : -1;
                }
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

    void LoaderObj::setVertexProperty(Geometry& geometry)
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
        lmath::Vector3 t = n;
        t.normalize();

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
        lcore::ofstream file(filepath, lcore::ios::binary|lcore::ios::out);
        if(!file.is_open()){
            return;
        }

        std::vector< std::vector<Vector3Vector> > tmpPositions[VType_Num];
        std::vector< std::vector<U16Vector4Vector> > tmpNormals[VType_Num];
        std::vector< std::vector<U16Vector2Vector> > tmpTexcoords[VType_Num];
        std::vector< std::vector<S32Vector> > tmpIndices[VType_Num];
        std::vector< std::vector<S32Map> > indexToIndex[VType_Num];

        for(s32 i=0; i<VType_Num; ++i){
            tmpPositions[i].resize( materialVector_.size() );
            tmpNormals[i].resize( materialVector_.size() );
            tmpTexcoords[i].resize( materialVector_.size() );
            tmpIndices[i].resize( materialVector_.size() );
            indexToIndex[i].resize( materialVector_.size() );

            for(u32 j=0; j<materialVector_.size(); ++j){
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

        std::vector<Geometry> geometries;
        std::vector<Mesh> meshes;
        for(s32 i=0; i<VType_Num; ++i){
            Geometry geometry;
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

            for(u32 j=0; j<materialVector_.size(); ++j){

                for(u32 k=0; k<tmpPositions[i][j].size(); ++k){
                    if(tmpIndices[i][j][k].size()<=0){
                        continue;
                    }
                    Vector3Vector& positions = tmpPositions[i][j][k];
                    U16Vector4Vector& normals = tmpNormals[i][j][k];
                    U16Vector2Vector& tecoords = tmpTexcoords[i][j][k];
                    S32Vector& indices = tmpIndices[i][j][k];

                    Mesh mesh;
                    mesh.geometry_ = static_cast<s16>(geometries.size());
                    mesh.material_ = static_cast<s32>(j);
                    mesh.indexOffset_ = 0;
                    mesh.numIndices_ = indices.size();
                    mesh.sphere_.calcMiniSphere(&positions[0], positions.size());
                    meshes.push_back(mesh);

                    geometry.numVertices_ = positions.size();
                    geometry.numIndices_ = indices.size();
                    geometries.push_back(geometry);

                    LOG("mesh{ geom:%d, mat:%d, indices:%d }", mesh.geometry_, mesh.material_, mesh.numIndices_);
                    LOG("geometry{ vertices:%d, indices:%d }", geometry.numVertices_, geometry.numIndices_);
                }
            }
        }

        Header header;
        header.major_ = 1;
        header.minor_ = 0;
        
        u32 offset = sizeof(Header);
        header.elems_[Elem_Geometry].number_ = geometries.size();
        header.elems_[Elem_Geometry].offset_ = offset;
        offset += sizeof(Geometry) * geometries.size();

        header.elems_[Elem_Material].number_ = materials_.size();
        header.elems_[Elem_Material].offset_ = offset;
        offset += sizeof(Material) * materials_.size();

        header.elems_[Elem_Mesh].number_ = meshes.size();
        header.elems_[Elem_Mesh].offset_ = offset;
        offset += sizeof(Mesh) * meshes.size();

        header.elems_[Elem_Node].number_ = 1;
        header.elems_[Elem_Node].offset_ = offset;
        offset += sizeof(Node) * 1;

        header.elems_[Elem_Joint].number_ = 0;
        header.elems_[Elem_Joint].offset_ = offset;
        offset += sizeof(Joint) * 0;

        header.elems_[Elem_Texture].number_ = textureVector_.size();
        header.elems_[Elem_Texture].offset_ = offset;
        offset += sizeof(Texture) * textureVector_.size();

        //-------------------------------------------
        lcore::io::write(file, header);

        s32 numGeometries = 0;
        for(s32 i=0; i<VType_Num; ++i){
            Geometry geometry;
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

            for(u32 j=0; j<materialVector_.size(); ++j){

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


        for(u32 i=0; i<materialVector_.size(); ++i){
            lcore::io::write(file, materialVector_[i]);
        }

        for(u32 i=0; i<meshes.size(); ++i){
            lcore::io::write(file, meshes[i]);
        }

        Node node;
        node.index_ = 0;
        node.parentIndex_ = lload::InvalidNode;
        node.childStartIndex_ = lload::InvalidNode;
        node.numChildren_ = 0;
        node.meshStartIndex_ = 0;
        node.numMeshes_ = static_cast<u8>(meshes.size());
        lcore::io::write(file, node);


        for(u32 i=0; i<textureVector_.size(); ++i){
            lcore::io::write(file, textureVector_[i]);
        }

        file.close();
    }
}
}
