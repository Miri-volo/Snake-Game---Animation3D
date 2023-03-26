#include "Renderer.h"
#include "Display.h"
#include "SceneSwitcher.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#define Menu StartMenu
#define SCENE BasicScene 
#define firstLevel level1
#define secondLevel level2
#define finish Victory

#define STRINGIFY(X) #X
#define CLASS_NAME_TO_HEADER(X) STRINGIFY(X.h)
#include CLASS_NAME_TO_HEADER(Menu)
#include CLASS_NAME_TO_HEADER(SCENE)
#include CLASS_NAME_TO_HEADER(firstLevel)
#include CLASS_NAME_TO_HEADER(secondLevel)
#include CLASS_NAME_TO_HEADER(finish)
#include "Animator.h";

using namespace cg3d;

int main()
{
    Game::CameraSettings cameraSettings{ 1000, 1000, 45.f, .1f, 120.f };

    Renderer renderer;
    Display display("Demo", cameraSettings.width, cameraSettings.height, &renderer);
    // std::cout.setstate(std::ios_base::failbit); // suppress junk output to console from igl::opengl::glfw::Viewer
    // std::cout.clear(); // re-enable output to console

    // setting up Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(display.window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    Game::SceneSwitcher switcher{ renderer, display };
    switcher.addScene("menu", [&display, &cameraSettings, &switcher]() {
        auto scene = std::make_shared<Menu>(STRINGIFY(Menu), &display);
        scene->Init(cameraSettings, [&switcher](const std::string& level) {switcher.setScene(level); });
        return scene;
        });
    switcher.addScene("first", [&display, &cameraSettings, &switcher]() {
        auto scene = std::make_shared<firstLevel>(STRINGIFY(firstLevel), &display);
        scene->Init(cameraSettings, [&switcher](const std::string& level) {switcher.setScene(level); });
        return scene;
        });

    switcher.addScene("second", [&display, &cameraSettings, &switcher]() {
        auto scene = std::make_shared<secondLevel>(STRINGIFY(secondLevel), &display);
        scene->Init(cameraSettings, [&switcher](const std::string& level) {switcher.setScene(level); });
        return scene;
        });

    switcher.addScene("third", [&display, &cameraSettings, &switcher]() {
        auto scene = std::make_shared<SCENE>(STRINGIFY(SCENE), &display);
        scene->Init(cameraSettings, [&switcher](const std::string& level) {switcher.setScene(level); });
        return scene;
        });
    switcher.addScene("victory", [&display, &cameraSettings, &switcher]() {
        auto scene = std::make_shared<finish>(STRINGIFY(finish), &display);
        scene->Init(cameraSettings, [&switcher](const std::string& level) {switcher.setScene(level); });
        return scene;
        });

    switcher.setScene("menu");
    display.LaunchRendering(true);

    return 0;
}


//#include "BasicScene.h"
//#include "SimpleScene.h"
//
//using namespace cg3d;
//
//int main()
//{
//    const int DISPLAY_WIDTH = 1600;
//    const int DISPLAY_HEIGHT = 800;
//    const float CAMERA_ANGLE = 45.0f;
//    const float NEAR = 0.1f;
//    const float FAR = 120.0f;
//
//    Renderer renderer;
//    Display display("Demo", DISPLAY_WIDTH, DISPLAY_HEIGHT, &renderer);
//    std::cout.setstate(std::ios_base::failbit); // suppress junk output to console from igl::opengl::glfw::Viewer
//    auto scene1 = std::make_shared<SimpleScene>("SimpleScene", &display);
//    auto scene2 = std::make_shared<BasicScene>("BasicScene", &display);
//    std::cout.clear(); // re-enable output to console
//    scene1->Init(CAMERA_ANGLE, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, NEAR, FAR);
//    scene2->Init(CAMERA_ANGLE, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, NEAR, FAR);
//    auto viewport1 = std::make_shared<Viewport>("LeftViewport", 0, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, scene1);
//    auto viewport2 = std::make_shared<Viewport>("RightViewport", DISPLAY_WIDTH / 2, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, scene2);
//    renderer.AddViewport(viewport1);
//    renderer.AddViewport(viewport2);
//    display.LaunchRendering(true);
//
//    return 0;
//}
