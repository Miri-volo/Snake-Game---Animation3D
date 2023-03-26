#include "Animator.h"

Game::BezierRepeatAnimator::BezierRepeatAnimator(const Eigen::Matrix3Xf& points, std::chrono::milliseconds duration, const std::shared_ptr<cg3d::Movable> animated)
	: points(points), duration(duration), currentTime(0), animated(animated)
{
	Eigen::ArrayXf factorials = Eigen::ArrayXf::Ones(points.cols());
	for (int i = 2; i < points.cols(); i++) {
		factorials[i] = factorials[i - 1] * i;
	}

	auto nFact = factorials[factorials.size() - 1];
	binomials = nFact / (factorials * factorials.reverse());
}

Eigen::Vector3f Game::BezierRepeatAnimator::getPoint(float time) const
{
	auto n = static_cast<float>(points.cols() - 1);
	Eigen::ArrayXf indices = Eigen::ArrayXf::LinSpaced(n+1, 0, n);
	Eigen::ArrayXf coeffs = binomials * Eigen::pow(1. - time, n - indices) * Eigen::pow(time, indices);
	return {
		(coeffs * points.row(0).transpose().array()).sum(),
		(coeffs * points.row(1).transpose().array()).sum(),
		(coeffs * points.row(2).transpose().array()).sum()
	};
}

float Game::BezierRepeatAnimator::millisecondsToTime(std::chrono::milliseconds currentTime)
{
	return (currentTime.count() % duration.count()) / static_cast<float>(duration.count());
}

void Game::BezierRepeatAnimator::update(const std::chrono::milliseconds& sinceLastUpdate)
{
	auto timeBefore = millisecondsToTime(currentTime);
	auto before = getPoint(timeBefore);
	auto timeAfter = millisecondsToTime(currentTime + sinceLastUpdate);
	auto after = getPoint(timeAfter);
	currentTime += sinceLastUpdate;
	if (timeBefore <= timeAfter) {
		animated->Translate(after - before);
	}
	else {
		auto endPart = points.col(points.cols() - 1) - before;
		auto startPart = points.col(0) - after;
		animated->Translate(endPart + startPart);
	}
}

Game::AnimatorComposer::AnimatorComposer(std::chrono::milliseconds duration)
	: duration(duration)
{}

void Game::AnimatorComposer::addUpdatable(const std::shared_ptr<Updatable>& updatable, std::chrono::milliseconds start, std::chrono::milliseconds duration)
{
	updatables.emplace_back(TimedUpdatable{updatable, start, duration});
}

void Game::AnimatorComposer::update(const std::chrono::milliseconds& sinceLastUpdate)
{
	for (const auto& timedUpdatable : updatables) {
		if (currentTime < timedUpdatable.startTime + timedUpdatable.duration) {
			if (timedUpdatable.startTime < currentTime) {
				if (currentTime + sinceLastUpdate < timedUpdatable.startTime + timedUpdatable.duration) {
					// Can update for entire length of current update.
					timedUpdatable.updatable->update(sinceLastUpdate);
				}
				else {
					// Partially update, ended before update ends.
					timedUpdatable.updatable->update(currentTime + sinceLastUpdate - (timedUpdatable.startTime + timedUpdatable.duration));
				}
			}
			else if (timedUpdatable.startTime < currentTime + sinceLastUpdate) {
				// Partially update, started after update started.
				timedUpdatable.updatable->update(timedUpdatable.startTime - currentTime);
			}
		}
	}
	currentTime += sinceLastUpdate;
	currentTime %= duration;
}
