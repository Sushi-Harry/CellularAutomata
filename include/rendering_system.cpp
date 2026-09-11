#include "rendering_system.hpp"

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