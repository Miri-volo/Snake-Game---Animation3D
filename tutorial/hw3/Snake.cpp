#include "Snake.h"
#include "igl/directed_edge_parents.h"
#include "igl/forward_kinematics.h"
#include "igl/dqs.h"
#include "igl/per_vertex_normals.h"
#include "MeshLoader.h"

void Game::Snake::init(const Game::CameraSettings& cameraSettings)
{
    auto program{ std::make_shared<cg3d::Program>("shaders/basicShader") };
    auto material1{ std::make_shared<cg3d::Material>("material", program) };
    material1->AddTexture(0, "textures/pinkSnake.jpg", 2);
    auto sphereMesh{ cg3d::IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj") };
    sphere = Game::Collidable(cg3d::Model::Create("sphere", sphereMesh, material1));

    auto material2{ std::make_shared<cg3d::Material>("material", program) };
    material2->AddTexture(0, "textures/pinkSnake.jpg", 2);
    auto cylMesh{ cg3d::IglLoader::MeshFromFiles("cyl_igl","data/zcylinder.obj") };
    cyls.push_back(Game::Collidable(cg3d::Model::Create("cyl", cylMesh, material2)));
    cyls[0]->RotateByDegree(90, cg3d::Movable::Axis::Y);
    cyls[0]->SetCenter({ 0,0, -0.8f });
    cyls[0]->isHidden = true;
    for (int i = 1; i < 16; i++)
    {
        cyls.push_back(Game::Collidable(cg3d::Model::Create("cyl", cylMesh, material2)));
        cyls[i]->Translate(1.6f, cg3d::Movable::Axis::Z);
        cyls[i]->SetCenter({ 0,0, -0.8f });
        cyls[i - 1]->AddChild(cyls[i].modelOwningPointer());
        cyls[i]->isHidden = true;
    }
    sphere->isHidden = true;
    // Translate target to snake head.
    sphere->Translate({ 1.6f * cyls.size(),0,0});

    thirdPerson = cg3d::Camera::Create("snake", cameraSettings.fov, float(cameraSettings.width) / cameraSettings.height, cameraSettings.near, cameraSettings.far);
    thirdPerson->RotateByDegree(-90, cg3d::Model::Axis::Y);
    thirdPerson->Translate({ -10, 4, 0 });
    thirdPerson->RotateByDegree(-15, cg3d::Model::Axis::X);
    sphere->AddChild(thirdPerson);

    auto snakeTexture = std::make_shared<cg3d::Material>("snakeSkin", program);
    snakeTexture->AddTexture(0, "textures/pinkSnake.jpg", 2);

    auto snakeMesh{ Game::LoadObj("snake", "data/snake-vt.obj") };
    snake = cg3d::Model::Create("snake", snakeMesh, snakeTexture);
    snake->showWireframe = false;
    cyls[0]->AddChild(snake);
    snake->Translate({ 0, 0, -0.8 });

    C = Eigen::MatrixXd::Zero(cyls.size() + 1, 3);
    auto zPoints = Eigen::ArrayXd::LinSpaced(cyls.size() + 1, 0, cyls.size() * 1.6).transpose();
    C.col(2) = zPoints;

    BE = Eigen::MatrixXi(cyls.size(), 2);
    auto indices = Eigen::ArrayXi::LinSpaced(cyls.size(), 0, cyls.size() - 1).transpose();
    BE.col(0) = indices;
    BE.col(1) = 1 + indices;

    igl::directed_edge_parents(BE, P);

    W = Eigen::MatrixXd::Zero(snake->GetMesh()->data[0].vertices.rows(), C.rows() - 1);
    for (size_t i = 0; i < snake->GetMesh()->data[0].vertices.rows(); i++)
    {
        Eigen::Vector3f v = snake->GetMesh()->data[0].vertices.row(i).cast<float>().eval();
        auto res = findClosestVertices(v, C);
        auto weights = Eigen::Vector2f(1 / log10f(powf(res[2], 2.0f) + 1), 1 / log10f(powf(res[3], 2.0f) + 1)).normalized();
        W.row(i)[(int)res[0]] = weights[0];
        W.row(i)[(int)res[1]] = weights[1];
    }

    V = snake->GetMesh()->data[0].vertices;
    U = snake->GetMesh()->data[0].vertices;
}

std::shared_ptr<cg3d::Model>& Game::Snake::snakeRootModel()
{
	return cyls[0].modelOwningPointer();
}


std::shared_ptr<cg3d::Model>& Game::Snake::targetModel()
{
	return sphere.modelOwningPointer();
}

std::shared_ptr<cg3d::Camera>& Game::Snake::thirdPersonCamera()
{
    return thirdPerson;
}

void Game::Snake::update(const std::chrono::milliseconds& sinceLastUpdate)
{
    auto translation = sphere->Tout.rotation() * speed;
    sphere->Translate((static_cast<float>(sinceLastUpdate.count()) / 25.) * translation);
    FabrikishMoveToTarget();
    Skin();
}

void Game::Snake::onUp()
{
    sphere->Rotate(Eigen::Quaternionf(cos(0.0349065850399), 0.0, 0.0, sin(0.0349065850399)).toRotationMatrix());
}

void Game::Snake::onDown()
{
    sphere->Rotate(Eigen::Quaternionf(cos(-0.0349065850399), 0.0, 0.0, sin(-0.0349065850399)).toRotationMatrix());
}

void Game::Snake::onLeft()
{
    sphere->Rotate(Eigen::Quaternionf(cos(0.0349065850399), 0.0, sin(0.0349065850399), 0.0).toRotationMatrix());
}

void Game::Snake::onRight()
{
    sphere->Rotate(Eigen::Quaternionf(cos(-0.0349065850399), 0.0, sin(-0.0349065850399), 0.0).toRotationMatrix());
}

bool Game::Snake::isSelfColliding()
{
    for (int i = 0; i < cyls.size() - 1; i++)
    {
        if (sphere.isCollidingWith(cyls[i])) {
            return true;
        }
    }
    return false;
}

bool Game::Snake::isCollidingWith(const Game::Collidable& other) const
{
    return sphere.isCollidingWith(other);
}

Eigen::Vector3f Game::Snake::getCylinderEndPosition(std::shared_ptr<cg3d::Model> cylinder)
{
	return cylinder->GetAggregatedTransform().col(3).head(3) + cylinder->GetRotation() * Eigen::Vector3f(0, 0, 0.8);
}

Eigen::Vector3f Game::Snake::getCylinderStartPosition(std::shared_ptr<cg3d::Model> cylinder)
{
	return cylinder->GetAggregatedTransform().col(3).head(3) - cylinder->GetRotation() * Eigen::Vector3f(0, 0, 0.8);
}

Eigen::Vector3f Game::Snake::getArmEndPosition()
{
	return getCylinderEndPosition(cyls[cyls.size() - 1].modelOwningPointer());
}

Eigen::Vector3f Game::Snake::getArmStartPosition()
{
	return getCylinderStartPosition(cyls[0].modelOwningPointer());
}

Eigen::Vector3f Game::Snake::getTargetPosition()
{
	return sphere->GetAggregatedTransform().col(3).head(3);
}

void Game::Snake::FabrikishMoveToTarget()
{
    std::vector<Eigen::Vector3f> p;
    for (int i = 0; i < cyls.size(); i++) {
        p.push_back(getCylinderStartPosition(cyls[i].modelOwningPointer()));
    }
    p.push_back(getCylinderEndPosition(cyls[cyls.size() - 1].modelOwningPointer()));
    //all the cylinders have the same length = 1.6
    float d = (p[0] - p[1]).norm();
    float dist = (getTargetPosition() - p[0]).norm(); //1.1
    float sum = d * cyls.size();
    // Delete cannot reach
    auto b = p[0]; //1.15
    float difA = (p[p.size() - 1] - getTargetPosition()).norm(); //1.17
    if (difA <= 0.05) {
        return;
    }
    // Delete while for single iteration.
    p[p.size() - 1] = getTargetPosition(); //1.21
    for (int i = p.size() - 2; i >= 0; i--) { //1.22
        float r = (p[i + 1] - p[i]).norm(); //1.24
        float lambda = d / r; //1.25
        p[i] = (1 - lambda) * p[i + 1] + lambda * p[i]; //1.27
    }
    //set arm position
    auto startTranslation = p[0] - getCylinderStartPosition(cyls[0].modelOwningPointer());
    cyls[0]->Translate(startTranslation);

    for (int i = 1; i < p.size(); i++) {//p.size-1 == cyls.size
        Eigen::Vector3f before = (getCylinderEndPosition(cyls[i - 1].modelOwningPointer()) - getCylinderStartPosition(cyls[i - 1].modelOwningPointer())).normalized();
        Eigen::Vector3f after = (p[i] - p[i - 1]).normalized();

        // Set the angle.
        float cosa = std::max(std::min(before.dot(after), 1.f), -1.f);
        auto a = std::acos(cosa);
        Eigen::Vector3f planeNormal = cyls[i - 1]->GetRotation().transpose() * before.cross(after).normalized();
        cyls[i - 1]->Rotate(a, planeNormal);
    }
}

void Game::Snake::Skin()
{
    auto animationPositions = std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond>>{};
    animationPositions.reserve(cyls.size());
    for (auto& cyl : cyls) {
        auto q = Eigen::Quaterniond(cyl->GetTout().rotation().cast<double>()).normalized();
        animationPositions.emplace_back(q);
    }
    animationPositions[0] = Eigen::Quaterniond::Identity();

    auto vQ = std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond>>{};
    std::vector<Eigen::Vector3d> vT{};

    // Used to populate vQ and vT.
    igl::forward_kinematics(C, BE, P, animationPositions, vQ, vT);
    igl::dqs(V, W, vQ, vT, U);
    Eigen::MatrixXd N;
    igl::per_vertex_normals(U, snake->GetMesh()->data[0].faces, N);
    cg3d::Mesh nextPose("snake", U, snake->GetMesh()->data[0].faces, N, snake->GetMesh()->data[0].textureCoords);
    snake->SetMeshList({ std::make_shared<cg3d::Mesh>(nextPose) });
}

Eigen::Vector4f Game::Snake::findClosestVertices(Eigen::Vector3f posV, Eigen::MatrixXd C)
{
    std::vector<float> distances(C.rows());
    for (int i = 0; i < C.rows(); i++)
    {
        Eigen::Vector3f posC_float = C.row(i).cast<float>().eval();
        distances[i] = (posV - posC_float).norm();
    }

    int closestIndex = 0;
    int secondClosestIndex = 1;
    if (distances[1] < distances[0])
    {
        closestIndex = 1;
        secondClosestIndex = 0;
    }
    for (int i = 2; i < C.rows(); i++)
    {
        if (distances[i] < distances[closestIndex])
        {
            secondClosestIndex = closestIndex;
            closestIndex = i;
        }
        else if (distances[i] < distances[secondClosestIndex])
        {
            secondClosestIndex = i;
        }
    }

    if (closestIndex == C.rows() - 1)
    {
        closestIndex = secondClosestIndex;
    }
    else if (secondClosestIndex == C.rows() - 1)
    {
        secondClosestIndex = closestIndex;
    }

    return { static_cast<float>(closestIndex), static_cast<float>(secondClosestIndex), distances[closestIndex], distances[secondClosestIndex] };
}
