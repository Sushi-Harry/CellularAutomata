#include "raylib.h"
#include "include/grid.hpp"
#include "include/rendering_system.hpp"

/*
    ONLY TEST CODE IN THE MAIN FILE RIGHT NOW. I'll write the application file after all the features have been added and verified to be working
*/

int main(){
    int cellSize = 50;
    Grid2D grid;
    GridRenderer2D renderer2d(cellSize);
    grid.ResizeGrid(10);
    int screenWidth = grid.GetSize() * cellSize;
    int screenHeight = grid.GetSize() * cellSize;

    InitWindow(screenWidth, screenHeight, "Cellular Automata 2D Test");
    SetTargetFPS(60);

    grid.SetCellState(1, 1, 1);
    grid.SetCellState(1, 2, 1);
    grid.SetCellState(5, 5, 1);

    while(!WindowShouldClose()){
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            int cellX = mousePos.x / cellSize;
            int cellY = mousePos.y / cellSize;

            if (cellX >= 0 && cellX < static_cast<int>(grid.GetSize()) &&
                cellY >= 0 && cellY < static_cast<int>(grid.GetSize())) {
                grid.SetCellState(cellX, cellY, (uint8_t)(!(bool)grid.GetCell(cellX, cellY)));
            }
        }

        BeginDrawing();
            renderer2d.Draw(grid);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}