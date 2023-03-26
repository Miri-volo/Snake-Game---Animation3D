#pragma once
#include "Collidable.h"
#include "Model.h"
#include "IglMeshLoader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Updater.h"

namespace Game {
    struct CameraSettings {
        const int width;
        const int height;
        const float fov;
        const float near;
        const float far;
    };

	class Snake : public Game::Updatable {
	public:
        explicit Snake() {}
        void init(const Game::CameraSettings& cameraSettings);
        void update(const std::chrono::milliseconds& sinceLastUpdate) override;
		std::shared_ptr<cg3d::Model>& snakeRootModel();
		std::shared_ptr<cg3d::Model>& targetModel();
		std::shared_ptr<cg3d::Camera>& thirdPersonCamera();
        void onUp();
        void onDown();
        void onLeft();
        void onRight();
        bool isSelfColliding();
        bool isCollidingWith(const Game::Collidable& other) const;
	private:
		Game::Collidable sphere;
		std::vector<Game::Collidable> cyls;
		std::vector<std::shared_ptr<cg3d::Model>> axis;
        std::shared_ptr<cg3d::Camera> thirdPerson;
        Eigen::Vector3f speed{ 0.1, 0, 0 };
        std::shared_ptr<cg3d::Model> snake;
        Eigen::MatrixXd C;
        Eigen::MatrixXi BE;
        Eigen::VectorXi P;
        Eigen::MatrixXd W;
        Eigen::MatrixXd V;
        Eigen::MatrixXd U;

		Eigen::Vector3f getCylinderEndPosition(std::shared_ptr<cg3d::Model> cylinder);
		Eigen::Vector3f getCylinderStartPosition(std::shared_ptr<cg3d::Model> cylinder);
		Eigen::Vector3f getArmEndPosition();
		Eigen::Vector3f getArmStartPosition();
		Eigen::Vector3f getTargetPosition();

        void FabrikishMoveToTarget();
        void Skin();

        static Eigen::Vector4f findClosestVertices(Eigen::Vector3f posV, Eigen::MatrixXd C);
    };
}