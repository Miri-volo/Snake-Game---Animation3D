#include "Victory.h"
#include "imgui.h"
#include <filesystem>
#include <string>
#include "StartMenu.h"
#include "Sound.h"
using namespace cg3d;

void Victory::Init(const Game::CameraSettings& cameraSettings, std::function<void(const std::string&)> onLevelChange)
{   
    changeLevel = onLevelChange;
    camera = Camera::Create("top", cameraSettings.fov, float(cameraSettings.width) / cameraSettings.height, cameraSettings.near, cameraSettings.far);
}

void Victory::BuildImGui()
{
    int my_image_width = 0;
    int my_image_height = 0;
    GLuint my_image_texture = 0;
    StartMenu::LoadTextureFromFile("../tutorial/hw3/happysnake.jpg", &my_image_texture, &my_image_width, &my_image_height); //https://fox59.com/news/indianapolis-zoo-getting-deadly-snake-africas-black-mamba/
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    bool* pOpen = nullptr;
    ImGui::Begin("Victory", pOpen, flags);
    ImGui::SetWindowSize(ImVec2(800, 800), ImGuiCond_Always);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("You Won!").x * 1.8) / 2);
    ImGui::SetWindowFontScale(1.8);
    ImGui::Text("You Won!");
    ImGui::SetCursorPosX(150);
    ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
    ImGui::Spacing();
    if (ImGui::Button("Back to menu", ImVec2(-1, 0))) {
        changeLevel("menu");
    }
    ImGui::End();
}