#pragma once
#include "Collidable.h"
#include "SceneWithImGui.h"
#include "Snake.h"
#include <memory>
#include <utility>
#include <chrono>

class StartMenu : public cg3d::SceneWithImGui
{
public:
    explicit StartMenu(std::string name, cg3d::Display* display) : SceneWithImGui(name, display) {};
    void Init(const Game::CameraSettings& cameraSettings, std::function<void(const std::string&)> onLevelChange);
    void BuildImGui() override;
    static void LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
private:
    std::function<void(const std::string&)> changeLevel;
    Game::Updater updater;
    std::shared_ptr<Game::Snake> snake;
    std::shared_ptr<Movable> root;
    Game::Collidable nextLevelReward;
    cg3d::Viewport* sceneViewport = nullptr;
    bool isRunning = true;
    std::vector<std::shared_ptr<cg3d::Camera>> cameras{};
};
