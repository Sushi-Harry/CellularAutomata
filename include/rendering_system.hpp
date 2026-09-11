#pragma once

#include "raylib.h"
#include "grid.hpp"

class GridRenderer2D{
public:
    GridRenderer2D(int cellSize = DEFAULT_CELL_SIZE) : _cellSize(cellSize) {}

    void Draw(const Grid2D& grid);
    void DrawGridLines(int gridSize);

    void SetCellSize(int size) { _cellSize = size; }
    int GetCellSize() const { return _cellSize; }

private:
    uint32_t _cellSize;
};

class GridRenderer3D{
public:
    GridRenderer3D(int cellSize = DEFAULT_CELL_SIZE, Color _activeColor = BLACK);
    ~GridRenderer3D();

    void Draw(const Grid3D& grid);
    void DrawGridLines(int gridSize);

    void SetCellSize(int size) { _cellSize = size; }
    int GetCellSize() const { return _cellSize; }

private:
    Material _cubeMaterial;
    Mesh _cubeMesh;
    uint32_t _cellSize;
    std::vector<Matrix> _transforms;
};