#include "StartMenu.h"
#include "imgui.h"
#include <filesystem>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#define GL_CLAMP_TO_EDGE 0x812F
#include "stb_image.h"
#include "Sound.h"
using namespace cg3d;

void StartMenu::Init(const Game::CameraSettings& cameraSettings, std::function<void(const std::string&)> onLevelChange)
{
    Sound::MenuMusic();
    changeLevel = onLevelChange;
    camera = Camera::Create("top", cameraSettings.fov, float(cameraSettings.width) / cameraSettings.height, cameraSettings.near, cameraSettings.far);
    
}

void StartMenu::BuildImGui()
{
	int my_image_width = 0;
	int my_image_height = 0;
    GLuint snakepic = 0;
    GLuint keys = 0;
	LoadTextureFromFile("../tutorial/hw3/snake.jpg", &snakepic, &my_image_width, &my_image_height); //https://wallpaperaccess.com/hd-snake#google_vignette
    int flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
	bool* pOpen = nullptr;
	ImGui::Begin("Snake Game", pOpen, flags);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Welcome to Snake Game - final 3DAnimation project!").x*1.8)/2);
    ImGui::SetWindowFontScale(1.8);
    ImGui::Text("Welcome to Snake Game - final 3DAnimation project!");
    ImGui::SetCursorPosX(200);
    ImGui::Image((void*)(intptr_t)snakepic, ImVec2(my_image_width, my_image_height));
    if (ImGui::CollapsingHeader("Tutorial", 0)) {
        LoadTextureFromFile("../tutorial/hw3/keys.jpg", &keys, &my_image_width, &my_image_height); //https://wallpaperaccess.com/hd-snake#google_vignette
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Use keys to move:").x) / 2);
        ImGui::Text("Use keys to move:");
        ImGui::SetCursorPosX(300);
        ImGui::Image((void*)(intptr_t)keys, ImVec2(my_image_width, my_image_height));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Press SPACE to pause the game or release the snake").x) / 2);
        ImGui::Text("Press SPACE to pause the game or release the snake");
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Press ESC to exit game").x) / 2);
        ImGui::Text("Press ESC to exit game");
    }
    ImGui::Spacing();
    ImGui::SetWindowFontScale(1.8);
	if (ImGui::Button("Start Level 1", ImVec2(0, 0))) {
        Sound::Stop();
        Sound::GameMusic();
		changeLevel("first");
	}
    ImGui::SameLine();
    if (ImGui::Button("Start Level 2", ImVec2(0, 0))) {
        Sound::Stop();
        Sound::GameMusic();
        changeLevel("second");
    }
    ImGui::SameLine();
    if (ImGui::Button("Start Level 3", ImVec2(0, 0))) {
        Sound::Stop();
        Sound::GameMusic();
        changeLevel("third");
    }
    ImGui::SetWindowFontScale(1);
    ImGui::Text("By Lior Kummer & Miri Volozhinsky");
	ImGui::End();
}


// from https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
void StartMenu::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;
}
