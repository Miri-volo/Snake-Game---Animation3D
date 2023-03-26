#pragma once
#include <chrono>
#include <vector>
#include <memory>

namespace Game {
	class Updatable {
	public:
		virtual ~Updatable() = default;
		virtual void update(const std::chrono::milliseconds& sinceLastUpdate) = 0;
	};

	class Updater {
	public:
		void update();
		void addUpdatable(std::shared_ptr<Updatable>&& updatable);
	private:
		std::vector<std::shared_ptr<Updatable>> updatables;
		std::chrono::time_point<std::chrono::steady_clock> lastUpdate = std::chrono::steady_clock::now();
	};
}