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

    int _shadingMode = 0; // 0 for default (solid black), 1 for Gradient based on distance from center
    float colorCenter[3] = { 1.0f, 1.0f, 0.0f }; // Default Yellow color
    float colorEdge[3] = { 0.0f, 0.0f, 1.0f }; // Default Blue color
private:
    Material _cubeMaterial;
    Mesh _cubeMesh;
    uint32_t _cellSize;
    std::vector<Matrix> _transforms;

    // This is just unimportant at this point. I'm adding it cause I like the way it looks
    // Shader uniform locations
    int _locShadingMode;
    int _locGridCenter;
    int _locMaxDistance;
    int _locColorStart;
    int _locColorEnd;
};