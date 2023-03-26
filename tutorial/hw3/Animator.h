#pragma once
#include <memory>
#include "Movable.h"
#include "Updater.h"

namespace Game {
	class BezierRepeatAnimator : public Game::Updatable {
	public:
		BezierRepeatAnimator(const Eigen::Matrix3Xf& points, std::chrono::milliseconds duration, const std::shared_ptr<cg3d::Movable> animated);
		Eigen::Vector3f getPoint(float time) const;
	private:
		std::shared_ptr<cg3d::Movable> animated;
		// Binomials are pre-calculated in the constructor.
		Eigen::ArrayXf binomials;
		Eigen::Matrix3Xf points;
		std::chrono::milliseconds duration;
		std::chrono::milliseconds currentTime;

		float millisecondsToTime(std::chrono::milliseconds currentTime);

		virtual void update(const std::chrono::milliseconds& sinceLastUpdate) override;
	};

	class AnimatorComposer : public Game::Updatable {
	public:
		AnimatorComposer(std::chrono::milliseconds duration);
		void addUpdatable(const std::shared_ptr<Updatable>& updatable, std::chrono::milliseconds start, std::chrono::milliseconds duration);

	private:
		struct TimedUpdatable {
			std::shared_ptr<Updatable> updatable;
			std::chrono::milliseconds startTime;
			std::chrono::milliseconds duration;
		};

		std::chrono::milliseconds duration;
		std::chrono::milliseconds currentTime{0};
		std::vector<TimedUpdatable> updatables{};

		virtual void update(const std::chrono::milliseconds& sinceLastUpdate) override;
	};
}