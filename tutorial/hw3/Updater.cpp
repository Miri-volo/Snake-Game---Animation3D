#include "Updater.h"

void Game::Updater::update()
{
	auto updateStart = std::chrono::steady_clock::now();
	auto timeSinceLastUpdate = updateStart - lastUpdate;
	lastUpdate = updateStart;
	for (const auto& updatable : updatables) {
		updatable->update(std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceLastUpdate));
	}
}

void Game::Updater::addUpdatable(std::shared_ptr<Updatable>&& updatable)
{
	updatables.emplace_back(std::move(updatable));
}
