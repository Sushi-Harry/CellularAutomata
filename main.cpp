#include "application.hpp"

/*
    ONLY TEST CODE IN THE MAIN FILE RIGHT NOW. I'll write the application file after all the features have been added and verified to be working
*/

int main(){
    // InitWindow(800, 800, "Cellular Automata 2D Test");
    // SetTargetFPS(60);

    // // Initializing the 3d grid
    // Grid3D grid;
    // int cellSize = 1;
    // grid.ResizeGrid(100);
    // grid.RandomSeed(0.2F, 1337);

    // // Initializing the renderer for the 3d grid
    // GridRenderer3D renderer3d(cellSize);
    
    // //Initializing a ruleset
    // PresetRuleset3D life3d(RULE_PRESET3D::LIFE_3D);

    // // CAMERA SETUP - DO NOT TOUCH IT - THIS IS PERFECT AS IS
    // Camera3D camera = { 0 };
    // float gridCenter = (grid.GetSize() * cellSize) * 0.5F;
    // camera.position = (Vector3){ gridCenter + 80.0f, gridCenter + 80.0f, gridCenter + 120.0f };
    // camera.target = (Vector3){ gridCenter, gridCenter, gridCenter };
    // camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    // camera.fovy = 45.0f;
    // camera.projection = CAMERA_PERSPECTIVE;

    // // Timing Variables
    // float updateInterval = 0.2F; // (0.2 seconds per tick. => 5 ticks per second)
    // float timeAccumulator = 0.0F;
    // bool isPaused = true;

    // while(!WindowShouldClose()){
    //     UpdateCamera(&camera, CAMERA_ORBITAL);

    //     // Speed controls
    //     if (IsKeyPressed(KEY_UP)) updateInterval = std::max(0.02f, updateInterval - 0.05f);
    //     if (IsKeyPressed(KEY_DOWN)) updateInterval += 0.05f;
    //     // Manual step when paused
    //     if (isPaused && IsKeyPressed(KEY_N)) {
    //         grid.Update(life3d, Neighbourhood3D::MOORE);
    //     }
    //     // Pause / Resume control
    //     if (IsKeyPressed(KEY_SPACE)) isPaused = !isPaused;
    //     // Random Grid Generation
    //     if(IsKeyPressed(KEY_R)) grid.RandomSeed(0.05F);

    //     // Simulation code
    //     if (!isPaused) {
    //         timeAccumulator += GetFrameTime();
    //         int maxTicksPerFrame = 5;
    //         while (timeAccumulator >= updateInterval && maxTicksPerFrame > 0) {
    //             grid.Update(life3d, Neighbourhood3D::MOORE);
    //             timeAccumulator -= updateInterval;
    //             maxTicksPerFrame--;
    //         }
    //     }

    //     BeginDrawing();
    //         ClearBackground(RAYWHITE);
    //         BeginMode3D(camera);
    //             renderer3d.Draw(grid);
    //         EndMode3D();

    //         if (isPaused) {
    //             DrawText("Press 'N' to advance 1 step", 10, 110, 18, RED);
    //         }
    //         DrawText(TextFormat("Simulation: %s (SPACE to toggle)", isPaused ? "PAUSED" : "RUNNING"), 10, 35, 18, DARKGREEN);
    //         DrawText(TextFormat("Tick Speed: %.2F sec/tick (UP / DOWN to change)", updateInterval), 10, 60, 18, DARKGREEN);
    //         DrawText("Press R to Randomize the current grid", 10, 85, 18, DARKGREEN);
    //         DrawFPS(10, 10);
    //     EndDrawing();
    // }

    // CloseWindow();

    Application3D newApp;
    newApp.Run();
    return 0;
}