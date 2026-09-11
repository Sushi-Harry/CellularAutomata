#include "rendering_system.hpp"

/*
    ,_______,\--------\
    \_\   | | \ _____\ \
          | |  \ \    \ \
    ,_____|_|   \ \    \ \
    | |_____    _\ \____\ \
    \_______|   \_________/
*/

void GridRenderer2D::Draw(const Grid2D& grid){
    uint32_t size = (uint32_t)grid.GetSize();

    // Clearing the background with WHiTE COLOR. Basic raylib stuff
    ClearBackground(RAYWHITE);

    // So it didn't cross my mind at first but this is a better way to draw than just drawing every single block. Just draw the ones with the alive state (Black Color)
    // Why? Because white (dead state) blocks won't be visible on my choice of background even if I draw them.
    for(uint32_t y = 0; y < size; y++){
        for(uint32_t x = 0; x < size; x++){
            if(grid.GetCell(x, y) == 1){
                DrawRectangle(x * _cellSize, y * _cellSize, _cellSize, _cellSize, BLACK);
            }
        }
    }

    DrawGridLines(size);
}

void GridRenderer2D::DrawGridLines(int gridSize){
    int totalPixels = gridSize * _cellSize; // Total pixels along one direction since gridSize contains a single dimension's size

    // Vertical lines first and then the horizontal ones
    for(int i = 0; i <= gridSize; i++){
        DrawLine(i*_cellSize, 0, i*_cellSize, totalPixels, BLACK);
    }
    for(int i = 0; i <= gridSize; i++){
        DrawLine(0, i*_cellSize, totalPixels, i*_cellSize, BLACK);
    }
}


/*  
    /-------\     \--------\
    \_____   \     \ _____\ \
        __\   \     \ \    \ \
        |__.   \     \ \    \ \
      ______\   \    _\ \____\ \
      \_________/    \_________/
*/

void GridRenderer3D::Draw(const Grid3D& grid){
    uint32_t size = (uint32_t)grid.GetSize();

    ClearBackground(RAYWHITE);
    for(int z = 0; z < size; z++){
        for(uint32_t y = 0; y < size; y++){
            for(uint32_t x = 0; x < size; x++){
                if(grid.GetCell(x, y, z) == 1){
                    // Center offset of 0.5 cause of the way origin (0,0,0) exists and the way that raylib draws its cubes. Too lazy to explaiin it right now cause it would take a lot of typing
                    Vector3 pos = {
                        ( x + 0.5)* _cellSize,
                        ( y + 0.5) * _cellSize,
                        ( z + 0.5) * _cellSize
                    };
                    DrawCube(pos, _cellSize, _cellSize, _cellSize, BLACK);
                }
            }
        }
    }

}