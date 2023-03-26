#include "Collidable.h"

Game::Collidable::Collidable(const std::shared_ptr<cg3d::Model>& wrappedMovable)
	: movable(wrappedMovable)
{
    boundingBox.init(
        wrappedMovable->GetMesh()->data[0].vertices,
        wrappedMovable->GetMesh()->data[0].faces
    );
}

Game::Collidable::Collidable()
{}

cg3d::Model* Game::Collidable::operator->()
{
    return movable.get();
}

std::shared_ptr<cg3d::Model>& Game::Collidable::modelOwningPointer()
{
    return movable;
}

bool Game::Collidable::isCollidingWith(const Collidable& other) const
{
    return isIntersecting(*movable.get(), *other.movable, boundingBox, other.boundingBox);
}

bool Game::Collidable::areBoxesIntersecting(const cg3d::Movable& object1, const cg3d::Movable& object2, const Eigen::AlignedBox<double, 3>& of1, const Eigen::AlignedBox<double, 3>& of2)
{
    auto R0 = 0.;
    auto R1 = 0.;
    auto R = 0.;

    Eigen::Matrix3d A = object1.GetRotation().transpose().cast<double>();
    Eigen::Matrix3d B = object2.GetRotation().transpose().cast<double>();
    Eigen::Matrix3d C = A * B.transpose();
    Eigen::Vector3d a = of1.sizes() / 2.;
    Eigen::Vector3d b = of2.sizes() / 2.;

    Eigen::Vector4d CenterOf1 = Eigen::Vector4d(of1.center()[0], of1.center()[1], of1.center()[2], 1);
    Eigen::Vector4d CenterOf2 = Eigen::Vector4d(of2.center()[0], of2.center()[1], of2.center()[2], 1);
    Eigen::Vector3d D = (object2.GetAggregatedTransform().cast<double>() * CenterOf2 - object1.GetAggregatedTransform().cast<double>() * CenterOf1).head(3);

    // A_0
    R0 = a(0);
    R1 = b(0) * abs(C.row(0)(0)) + b(1) * abs(C.row(0)(1)) + b(2) * abs(C.row(0)(2));
    R = abs(A.row(0) * D);
    if (R0 + R1 < R) {
        return false;
    }

    // A_1
    R0 = a(1);
    R1 = b(0) * abs(C.row(1)(0)) + b(1) * abs(C.row(1)(1)) + b(2) * abs(C.row(1)(2));
    R = abs(A.row(1) * D);
    if (R0 + R1 < R) {
        return false;
    }

    // A_2
    R0 = a(2);
    R1 = b(0) * abs(C.row(2)(0)) + b(1) * abs(C.row(2)(1)) + b(2) * abs(C.row(2)(2));
    R = abs(A.row(2) * D);
    if (R0 + R1 < R) {
        return false;
    }

    // B_0
    R0 = a(0) * abs(C.row(0)(0)) + a(1) * abs(C.row(1)(0)) + a(2) * abs(C.row(2)(0));
    R1 = b(0);
    R = abs(B.row(0) * D);
    if (R0 + R1 < R) {
        return false;
    }

    // B_1
    R0 = a(0) * abs(C.row(0)(1)) + a(1) * abs(C.row(1)(1)) + a(2) * abs(C.row(2)(1));
    R1 = b(1);
    R = abs(B.row(1) * D);
    if (R0 + R1 < R) {
        return false;
    }

    // B_2
    R0 = a(0) * abs(C.row(0)(2)) + a(1) * abs(C.row(1)(2)) + a(2) * abs(C.row(2)(2));
    R1 = b(2);
    R = abs(B.row(2) * D);
    if (R0 + R1 < R) {
        return false;
    }

    // A_0 * B_0
    R0 = a(1) * abs(C.row(2)(0)) + a(2) * abs(C.row(1)(0));
    R1 = b(1) * abs(C.row(0)(2)) + b(2) * abs(C.row(0)(1));
    R = C.row(1)(0) * A.row(2) * D;
    R -= C.row(2)(0) * A.row(1) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_0 * B_1
    R0 = a(1) * abs(C.row(2)(1)) + a(2) * abs(C.row(1)(1));
    R1 = b(0) * abs(C.row(0)(2)) + b(2) * abs(C.row(0)(0));
    R = C.row(1)(1) * A.row(2) * D;
    R -= C.row(2)(1) * A.row(1) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_0 * B_2
    R0 = a(1) * abs(C.row(2)(2)) + a(2) * abs(C.row(1)(2));
    R1 = b(0) * abs(C.row(0)(1)) + b(1) * abs(C.row(0)(0));
    R = C.row(1)(2) * A.row(2) * D;
    R -= C.row(2)(2) * A.row(1) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_1 * B_0
    R0 = a(0) * abs(C.row(2)(0)) + a(2) * abs(C.row(0)(0));
    R1 = b(1) * abs(C.row(1)(2)) + b(2) * abs(C.row(1)(1));
    R = C.row(2)(0) * A.row(0) * D;
    R -= C.row(0)(0) * A.row(2) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_1 * B_1
    R0 = a(0) * abs(C.row(2)(1)) + a(2) * abs(C.row(0)(1));
    R1 = b(0) * abs(C.row(1)(2)) + b(2) * abs(C.row(1)(0));
    R = C.row(2)(1) * A.row(0) * D;
    R -= C.row(0)(1) * A.row(2) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_1 * B_2
    R0 = a(0) * abs(C.row(2)(2)) + a(2) * abs(C.row(0)(2));
    R1 = b(0) * abs(C.row(1)(1)) + b(1) * abs(C.row(1)(0));
    R = C.row(2)(2) * A.row(0) * D;
    R -= C.row(0)(2) * A.row(2) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_2 * B_0
    R0 = a(0) * abs(C.row(1)(0)) + a(1) * abs(C.row(0)(0));
    R1 = b(1) * abs(C.row(2)(2)) + b(2) * abs(C.row(2)(1));
    R = C.row(0)(0) * A.row(1) * D;
    R -= C.row(1)(0) * A.row(0) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_2 * B_1
    R0 = a(0) * abs(C.row(1)(1)) + a(1) * abs(C.row(0)(1));
    R1 = b(0) * abs(C.row(2)(2)) + b(2) * abs(C.row(2)(0));
    R = C.row(0)(1) * A.row(1) * D;
    R -= C.row(1)(1) * A.row(0) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    // A_2 * B_2
    R0 = a(0) * abs(C.row(1)(2)) + a(1) * abs(C.row(0)(2));
    R1 = b(0) * abs(C.row(2)(1)) + b(1) * abs(C.row(2)(0));
    R = C.row(0)(2) * A.row(1) * D;
    R -= C.row(1)(2) * A.row(0) * D;
    R = abs(R);
    if (R0 + R1 < R) {
        return false;
    }

    return true;
}

bool Game::Collidable::isIntersecting(const cg3d::Movable& object1, const cg3d::Movable& object2, const igl::AABB<Eigen::MatrixXd, 3>& of1, const igl::AABB<Eigen::MatrixXd, 3>& of2)
{
    if (!areBoxesIntersecting(object1, object2, of1.m_box, of2.m_box)) {
        return false;
    }

    if (of1.is_leaf() && of2.is_leaf()) {
        return true;
    }

    if (of1.is_leaf() && !of2.is_leaf()) {
        return isIntersecting(object1, object2, of1, *of2.m_right)
            || isIntersecting(object1, object2, of1, *of2.m_left);
    }
    if (!of1.is_leaf() && of2.is_leaf()) {
        return  isIntersecting(object1, object2, *of1.m_right, of2)
            || isIntersecting(object1, object2, *of1.m_left, of2);
    }
    return isIntersecting(object1, object2, *of1.m_right, *of2.m_right)
        || isIntersecting(object1, object2, *of1.m_right, *of2.m_left)
        || isIntersecting(object1, object2, *of1.m_left, *of2.m_right)
        || isIntersecting(object1, object2, *of1.m_left, *of2.m_left);
}