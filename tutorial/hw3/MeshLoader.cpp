#include "MeshLoader.h"
#include "ObjLoader.h"

std::shared_ptr<cg3d::Mesh> Game::LoadObj(const std::string& name, const std::string& objFile)
{
    return cg3d::ObjLoader::MeshFromObj(name, { objFile });
}
