#pragma once
#include "Model.h"
#include <igl/AABB.h>

namespace Game {
	class Collidable
	{
	public:
		explicit Collidable(const std::shared_ptr<cg3d::Model>& wrappedMovable);
		Collidable();
		cg3d::Model* operator->();
		std::shared_ptr<cg3d::Model>& modelOwningPointer();
		bool isCollidingWith(const Collidable& other) const;
	private:
		std::shared_ptr<cg3d::Model> movable;
		igl::AABB<Eigen::MatrixXd, 3> boundingBox;

		static bool areBoxesIntersecting(
			const cg3d::Movable& object1,
			const cg3d::Movable& object2,
			const Eigen::AlignedBox<double, 3>& of1,
			const Eigen::AlignedBox<double, 3>& of2
		);
		static bool isIntersecting(
			const cg3d::Movable& object1,
			const cg3d::Movable& object2,
			const igl::AABB<Eigen::MatrixXd, 3>& of1,
			const igl::AABB<Eigen::MatrixXd, 3>& of2
		);
	};
}