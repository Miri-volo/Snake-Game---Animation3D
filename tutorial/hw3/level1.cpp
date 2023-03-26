#include "level1.h"
#include <Eigen/src/Core/Matrix.h>
#include <edges.h>
#include <memory>
#include <per_face_normals.h>
#include <read_triangle_mesh.h>
#include <utility>
#include <vector>
#include "GLFW/glfw3.h"
#include "Mesh.h"
#include "PickVisitor.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include "imgui.h"
#include <algorithm>
#include "igl/per_vertex_normals.h"
#include "igl/per_face_normals.h"
#include "igl/unproject_onto_mesh.h"
#include "igl/edge_flaps.h"
#include "igl/loop.h"
#include "igl/upsample.h"
#include "igl/AABB.h"
#include "igl/parallel_for.h"
#include "igl/shortest_edge_and_midpoint.h"
#include "igl/circulation.h"
#include "igl/edge_midpoints.h"
#include "igl/collapse_edge.h"
#include "igl/edge_collapse_is_valid.h"
#include "igl/write_triangle_mesh.h"
#include <algorithm>
using namespace cg3d;

void level1::Init(const Game::CameraSettings& cameraSettings, std::function<void(const std::string&)> onLevelChange)
{
    changeLevel = onLevelChange;
    auto daylight{ std::make_shared<Material>("daylight", "shaders/cubemapShader") };
    daylight->AddTexture(0, "textures/cubemaps/skybox/skybox_", 3);
    auto background{ Model::Create("background", Mesh::Cube(), daylight) };
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();
    root = Movable::Create("root");
    AddChild(root);

    snake = std::make_shared<Game::Snake>();
    snake->init(cameraSettings);
    updater.addUpdatable(snake);
    root->AddChild(snake->snakeRootModel());
    root->AddChild(snake->targetModel());

    snake->snakeRootModel()->Rotate(0, Axis::X);
    cameras.emplace_back(Camera::Create("top", cameraSettings.fov, float(cameraSettings.width) / cameraSettings.height, cameraSettings.near, cameraSettings.far));
    cameras[0]->Translate(55, Axis::Y);
    cameras[0]->RotateByDegree(-90, Axis::X);
    cameras.emplace_back(snake->thirdPersonCamera());
    camera = cameras[1];
    score = 3;
    auto program{ std::make_shared<cg3d::Program>("shaders/basicShader") };
    auto material1{ std::make_shared<cg3d::Material>("material", program) };
    material1->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{ cg3d::IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj") };
    nextLevelReward = Game::Collidable(cg3d::Model::Create("target", sphereMesh, material1));
    AddChild(nextLevelReward.modelOwningPointer());
    MoveTarget();
}

void level1::MoveTarget() {
    float point = std::min(50, 5 * score + 20);
    float z = 5;
    if (score % 2 == 0) {
        point *= -1;
        z *=-1;
    }
    nextLevelReward->Translate({ point, 0, z });
}

void level1::BuildImGui()
{
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    bool* pOpen = nullptr;

    ImGui::Begin("Menu", pOpen, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(0, 0), ImGuiCond_Always);

    ImGui::Text("Choose camera:");
    for (int i = 0; i < cameras.size(); i++) {
        ImGui::SameLine(0);
        bool selectedCamera = cameras[i] == sceneViewport->camera;
        if (selectedCamera)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button(std::to_string(i + 1).c_str()))
            setCamera(i);
        if (selectedCamera)
            ImGui::PopStyleColor();
    }
    if (snake->isSelfColliding()) {
        ImGui::Text("Press SPACE to release the snake");
    }
    ImGui::Text("Targets left until next level: %d", score);
    if (pickedModel) {
        ImGui::Text("Picked model: %s", pickedModel->name.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Drop"))
            pickedModel = nullptr;
        if (pickedModel) {
            if (ImGui::CollapsingHeader("Draw options", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Show wireframe", &pickedModel->showWireframe);
                if (pickedModel->showWireframe) {
                    ImGui::Text("Wireframe color:");
                    ImGui::SameLine();
                    ImGui::ColorEdit4("Wireframe color", pickedModel->wireframeColor.data(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                }
                ImGui::Checkbox("Show faces", &pickedModel->showFaces);
                ImGui::Checkbox("Show textures", &pickedModel->showTextures);
                if (ImGui::Button("Scale down"))
                    pickedModel->Scale(0.9f);
                ImGui::SameLine();
                if (ImGui::Button("Scale up"))
                    pickedModel->Scale(1.1f);
            }
            if (ImGui::Button("Dump model mesh data")) {
                std::cout << "model name: " << pickedModel->name << std::endl;
                if (pickedModel->meshIndex > 0)
                    std::cout << "mesh index in use: " << pickedModel->meshIndex;
                for (auto& mesh : pickedModel->GetMeshList()) {
                    Eigen::IOFormat simple(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "", "");
                    std::cout << "mesh name: " << mesh->name << std::endl;
                    for (int i = 0; i < mesh->data.size(); i++) {
                        if (mesh->data.size() > 1)
                            std::cout << "mesh #" << i + 1 << ":" << std::endl;
                    }
                }
            }
            if (ImGui::Button("Dump model transformations")) {
                Eigen::IOFormat format(2, 0, ", ", "\n", "[", "]");
                const Eigen::Matrix4f& transform = pickedModel->GetAggregatedTransform();
                std::cout << "Tin:" << std::endl << pickedModel->Tin.matrix().format(format) << std::endl
                    << "Tout:" << std::endl << pickedModel->Tout.matrix().format(format) << std::endl
                    << "Transform:" << std::endl << transform.matrix().format(format) << std::endl
                    << "--- Transform Breakdown ---" << std::endl
                    << "Rotation:" << std::endl << Movable::GetTranslation(transform).matrix().format(format) << std::endl
                    << "Translation:" << std::endl << Movable::GetRotation(transform).matrix().format(format) << std::endl
                    << "Rotation x Translation:" << std::endl << Movable::GetTranslationRotation(transform).matrix().format(format)
                    << std::endl << "Scaling:" << std::endl << Movable::GetScaling(transform).matrix().format(format) << std::endl;
            }
        }
    }
    if (ImGui::Button("Back to menu", ImVec2(0, 0))) {
        changeLevel("menu");
    }
    ImGui::End();
}

void level1::AddViewportCallback(cg3d::Viewport* _viewport)
{
    sceneViewport = _viewport;
    SceneWithImGui::AddViewportCallback(_viewport);
}


void level1::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{

    SceneWithImGui::Update(program, proj, view, model);
    if (!isRunning) {
        return;
    }

    updater.update();
    if (snake->isSelfColliding()) {
        std::cout << "Collided!\n";
        isRunning = false;
    }
    if (score == 0) {
        changeLevel("second");
        isRunning = false;
    }
    if (score != 0 && snake->isCollidingWith(nextLevelReward)) {
        score--;
        MoveTarget();
    }
    updater.update();
}

void level1::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            return;
        case GLFW_KEY_SPACE:
            isRunning = !isRunning;
            return;
        case GLFW_KEY_W:
            snake->onUp();
            return;
        case GLFW_KEY_S:
            snake->onDown();
            return;
        case GLFW_KEY_A:
            snake->onLeft();
            return;
        case GLFW_KEY_D:
            snake->onRight();
            return;
        }
    }
    SceneWithImGui::KeyCallback(viewport, x, y, key, scancode, action, mods);
}

void level1::setCamera(int index)
{
    sceneViewport->camera = cameras[index];
}