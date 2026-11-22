#include "application.hpp"
#include "imgui.h"
#include "rlImGui.h"

/*
    ONLY TEST CODE IN THE MAIN FILE RIGHT NOW. I'll write the application file after all the features have been added and verified to be working
*/

enum class AppMode {
    None,
    MODE_3D,
    MODE_2D
};

AppMode showStartupLauncher(){
    InitWindow(400, 200, "Startup Launcher");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    AppMode chosenMode = AppMode::None;
    int selectedIndex = 0;
    const char* options[] = { "Option 1: 3D Mode", "Option 2: 2D Mode" };

    while (!WindowShouldClose() && chosenMode == AppMode::None) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        rlImGuiBegin();

        // Make the ImGui window fill the entire Raylib launcher window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)GetScreenWidth(), (float)GetScreenHeight()));
        ImGui::Begin("Select:", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImGui::Spacing();
        ImGui::Combo("##ModeSelect", &selectedIndex, options, IM_ARRAYSIZE(options));
        ImGui::Spacing();
        if (ImGui::Button("Confirm & Launch", ImVec2(-1, 35))) {
            chosenMode = (selectedIndex == 0) ? AppMode::MODE_3D : AppMode::MODE_2D;
        }
        ImGui::End();
        rlImGuiEnd();
        EndDrawing();
    }
    rlImGuiShutdown();
    CloseWindow();

    return chosenMode;
}

int main(){
    AppMode selected = showStartupLauncher();
    if (selected == AppMode::None) {
        return 0;
    }

    if(selected == AppMode::MODE_3D){
        Application3D newApp;
        newApp.Run();
    }else{
        Application2D newApp;
        newApp.Run();
    }

    return 0;
}