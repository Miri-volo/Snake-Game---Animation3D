#include "SceneSwitcher.h"

Game::SceneSwitcher::SceneSwitcher(cg3d::Renderer& renderer, cg3d::Display& display)
	: renderer(renderer), display(display)
{}

void Game::SceneSwitcher::addScene(const std::string& key, std::function<std::shared_ptr<cg3d::Scene>()> sceneFactory)
{
	scenes[key] = sceneFactory;
}

void Game::SceneSwitcher::setScene(const std::string& key)
{
	auto scene = scenes[key]();
	auto viewport = std::make_shared<cg3d::Viewport>(scene->name + "Viewport", 0, 0, 0, 0);
	viewport->scene = scene;
	renderer.AddViewport(viewport);
	if (0 < viewports.size()) {
		renderer.RemoveViewport(viewports.back());
	}
	viewports.emplace_back(viewport);
}
