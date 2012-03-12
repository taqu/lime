/**
@file Object.cpp
@author t-sakai
@date 2011/02/16
*/
#include "Object.h"

#include "Geometry.h"
#include "Material.h"
#include "ShaderManager.h"
#include <lframework/scene/Scene.h>
#include <lframework/scene/ShaderBase.h>
#include "../render/Batch.h"

#include <lgraphics/api/VertexBufferRef.h>
#include <lgraphics/api/GeometryBuffer.h>

#include "../System.h"
#include "../render/RenderingSystem.h"

namespace lscene
{
    Object::Object()
    {
    }


    Object::Object(
        u32 numGeometries,
        u32 numMaterials)
        :lrender::Drawable(numGeometries, numMaterials)
    {
    }

    Object::~Object()
    {
    }
}
