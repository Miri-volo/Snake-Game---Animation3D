#pragma once
#include <filesystem>
#include "Model.h"

namespace Game {
	std::shared_ptr<cg3d::Mesh> LoadObj(const std::string& name, const std::string& objFile);
}