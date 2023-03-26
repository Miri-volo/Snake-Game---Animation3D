#pragma once
#include "Scene.h"
#include "Renderer.h"

namespace Game {
	class SceneSwitcher
	{
	public:
		explicit SceneSwitcher(cg3d::Renderer& renderer, cg3d::Display& display);
		void addScene(const std::string& key, std::function<std::shared_ptr<cg3d::Scene>()> sceneFactory);
		void setScene(const std::string& key);
	private:
		std::map<std::string, std::function<std::shared_ptr<cg3d::Scene>()>> scenes{};
		// Need to keep the previous viewports because the engine uses it after freeing.
		std::vector<std::shared_ptr<cg3d::Viewport>> viewports{};
		cg3d::Renderer& renderer;
		cg3d::Display& display;
	};
}