#pragma once

#include <vector>
#include <cstdint>

/*
    Grid will have this:
        1. A vector of cells. Need to figure out the
        2. assuming constant grid size of 100x100 for simplicity
        3. size of each cell = 5x5
*/

/*
    Required Functions:
        1. Update the cells

*/

// DEAD STATE = 0
// ALIVE STATE = 1

constexpr int DEFAULT_GRID_SIZE = 100;
constexpr int DEFAULT_CELL_SIZE = 5;

class Grid2D{
public:
    Grid2D();

    void ResizeGrid(int size);
    void SetCellState(int x, int y, uint8_t state);
    uint8_t GetCell(int x, int y) const;

    const std::vector<uint8_t>& GetCells() const { return _cells; }
    const int GetSize() const { return _currentSize; }
private:
    std::vector<uint8_t> _cells; // Primary cell buffer
    std::vector<uint8_t> _nextCells; // Secondary Cell buffer
    uint32_t _currentSize;
};

class Grid3D{
public:
    Grid3D();

    void ResizeGrid(int size);
    void SetCellState(int x, int y, int z, uint8_t state);
    uint8_t GetCell(int x, int y, int z) const;

    const std::vector<uint8_t>& GetCells() const { return _cells; }
    const int GetSize() const { return _currentSize; }
    void Clear();
    void RandomSeed(float density = 0.2F, uint32_t seed = 0);

private:
    std::vector<uint8_t> _cells;
    std::vector<uint8_t> _nextCells;
    uint32_t _currentSize;
};