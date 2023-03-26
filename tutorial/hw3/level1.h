#pragma once
#include "Collidable.h"
#include "SceneWithImGui.h"
#include "Snake.h"
#include "Updater.h"
#include "Animator.h"
#include <memory>
#include <utility>
#include <chrono>

class level1 : public cg3d::SceneWithImGui
{
public:
    explicit level1(std::string name, cg3d::Display* display) : SceneWithImGui(name, display) {};
    void Init(const Game::CameraSettings& cameraSettings, std::function<void(const std::string&)> onLevelChange);
    void BuildImGui() override;
    void AddViewportCallback(cg3d::Viewport* _viewport) override;
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) override;
    void MoveTarget();
private:
    std::function<void(const std::string&)> changeLevel;
    Game::Updater updater;
    std::shared_ptr<Game::Snake> snake;
    std::shared_ptr<Movable> root;
    Game::Collidable nextLevelReward;
    cg3d::Viewport* sceneViewport = nullptr;
    bool isRunning = true;
    int score;
    std::vector<std::shared_ptr<cg3d::Camera>> cameras{};
    void setCamera(int index);
};
