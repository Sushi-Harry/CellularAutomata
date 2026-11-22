#pragma once

#include <vector>
#include <cstdint>


class Rulesets2D;
class Rulesets3D;

// DEAD STATE = 0
// ALIVE STATE = 1

constexpr int DEFAULT_GRID_SIZE = 100;
constexpr int DEFAULT_CELL_SIZE = 5;

// Two ways to calculate neighbour count
// 1. Identical to moore neighbours - 9 neighbours done using precomputed offsets
// 2. Identical to von neumann - 4 neighbours. Useless since 2D is easier to render and compute compared to 3D

class Grid2D{
public:
    Grid2D();

    void ResizeGrid(int size);
    void SetCellState(int x, int y, uint8_t state);
    uint8_t GetCell(int x, int y) const;
    void Clear();

    const std::vector<uint8_t>& GetCells() const { return _cells; }
    const int GetSize() const { return _currentSize; }
    void RandomSeed(float density, uint32_t seed = 0);
    unsigned int GetNeighbourCount(int x, int y) const;
    void Update(const Rulesets2D& ruleset);
private:
    void calcOffsets();

    std::vector<uint8_t> _cells; // Primary cell buffer
    std::vector<uint8_t> _nextCells; // Secondary Cell buffer
    uint32_t _currentSize = DEFAULT_GRID_SIZE;
    unsigned int GetNeighbourCount_FastMoore(int index) const;
    int _precomputedOffsets[9];
};

// This enum is for the 3D neighbourhood type
enum class Neighbourhood3D{
    MOORE,
    VON_NEUMANN
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

    unsigned int GetNeighbourCount(int x, int y, int z, Neighbourhood3D type) const;

    void Update(const Rulesets3D& ruleset, Neighbourhood3D type);
    
private:
    void CalculateOffsets();
    unsigned int GetNeighbourCount_Neumann(int x, int y, int z) const;
    unsigned int GetNeighbourCount_Moore(int x, int y, int z) const;
    inline unsigned int GetNeighboutCount_FastMoore(int index) const;

    std::vector<uint8_t> _cells;
    std::vector<uint8_t> _nextCells;
    uint32_t _currentSize = DEFAULT_GRID_SIZE;
    // std::vector<int> _precompMooreOffsets;
    int _precompMooreOffsets[26];
};