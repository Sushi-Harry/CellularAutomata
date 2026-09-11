#include "raylib.h"
#include "include/grid.hpp"
#include "include/rendering_system.hpp"

/*
    ONLY TEST CODE IN THE MAIN FILE RIGHT NOW. I'll write the application file after all the features have been added and verified to be working
*/

int main(){
    InitWindow(800, 800, "Cellular Automata 2D Test");
    SetTargetFPS(60);

    Grid3D grid;
    int cellSize = 1;
    grid.ResizeGrid(20);

    GridRenderer3D renderer3d(cellSize);

    Camera3D camera = { 0 };
    float gridCenter = (grid.GetSize() * cellSize) * 0.5F;
    camera.position = (Vector3){ gridCenter + 80.0f, gridCenter + 80.0f, gridCenter + 120.0f };
    camera.target = (Vector3){ gridCenter, gridCenter, gridCenter };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    grid.SetCellState(10, 10, 10, 1);
    grid.SetCellState(10, 11, 10, 1);
    grid.SetCellState(10, 12, 10, 1);
    grid.SetCellState(11, 10, 10, 1);
    grid.SetCellState(15, 11, 10, 1);
    grid.SetCellState(19, 11, 10, 1);

    while(!WindowShouldClose()){
        UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                renderer3d.Draw(grid);
            EndMode3D();

            DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}