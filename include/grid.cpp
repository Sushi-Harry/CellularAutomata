#include "grid.hpp"
#include <iostream>
#include <algorithm> // For some reason I keep forgetting that std::min() function blongs to this header
#include <random>
#include "rulesets.hpp"
#include <omp.h>

/*  
    WRITING THIS AFTER THE NOTE ABOVE THE 3D LOGO.
    YES I WILLINGLY WASTED TIME MAKING THIS BIG 2D LOGO PURELY CAUSE IT'S PLEASING TO MY EYES

    ,_______,\--------\
    \_\   | | \ _____\ \
          | |  \ \    \ \
    ,_____|_|   \ \    \ \
    | |_____    _\ \____\ \
    \_______|   \_________/
*/

Grid2D::Grid2D() : _currentSize(DEFAULT_GRID_SIZE){
    _cells.assign(DEFAULT_GRID_SIZE*DEFAULT_GRID_SIZE, 0);
    _nextCells.assign(DEFAULT_GRID_SIZE*DEFAULT_GRID_SIZE, 0);

}

void Grid2D::SetCellState(int x, int y, uint8_t state){
    _cells[x + y * _currentSize] = state;
}

uint8_t Grid2D::GetCell(int x, int y) const {
    return _cells[x + y * _currentSize];
}

void Grid2D::ResizeGrid(int size){
    // Allocating enough memory for the new grid with "size" being the length
    std::vector<uint8_t> newCells(size*size, 0);

    // Copying existing cells into the new grid
    // Now here's what we need to do before starting all the copying and pasting data from previous grid to the newCells (new grid)
    /*
        Case1: The new grid is larger than the previous grid. (Easy stuff. No need to handle states that are alive since all the alive states are guaranteed to find a place in the new grid)
        Case2: The new grid is smaller (In this case, the alive states might get cut off which will alter the simulation going forward. But we'll look at that some other time)
    */

    uint32_t copyDimensions = std::min(_currentSize, (uint32_t)size); 
    for(int i = 0; i < copyDimensions; i++){
        for(int j = 0; j < copyDimensions; j++){
            newCells[i + size*j] = _cells[i + _currentSize*j];
        }
    }

    _currentSize = size;
    _cells = std::move(newCells);
    _nextCells.assign(_currentSize*_currentSize, 0);
}

// Here's a little note I'm leaving just in case anyone expects this is AI.
// I TYPED OUT THIS HEADING BY HAND AND WASTED OVER 5 MINUTES ON JUST WRITING THIS BIG "3D" HEADING

/*  
    /-------\     \--------\
    \_____   \     \ _____\ \
        __\   \     \ \    \ \
        |__.   \     \ \    \ \
      ______\   \    _\ \____\ \
      \_________/    \_________/
*/
Grid3D::Grid3D(){
    _cells.assign(DEFAULT_GRID_SIZE * DEFAULT_GRID_SIZE * DEFAULT_GRID_SIZE, 0);
    _nextCells.assign(DEFAULT_GRID_SIZE * DEFAULT_GRID_SIZE * DEFAULT_GRID_SIZE, 0);
    CalculateOffsets();
}

// This function is a part of my attempt to optimize performance for large size grid. Instead of calculating 27 offsets for every single cell in every single tick, I'll do it only when the grid is initialized or resized
void Grid3D::CalculateOffsets(){
    int s = _currentSize;
    int s2 = s * s;
    int idx = 0;
    for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0 && dz == 0) continue;
                _precompMooreOffsets[idx++] = dx + (dy * s) + (dz * s2);
            }
        }
    }
}

void Grid3D::SetCellState(int x, int y, int z, uint8_t state){
    _cells[x + _currentSize * y + _currentSize * _currentSize * z] = state;
}

uint8_t Grid3D::GetCell(int x, int y, int z) const {
    return _cells[x + _currentSize*y + _currentSize*_currentSize*z];
}

void Grid3D::ResizeGrid(int size){
    std::vector<uint8_t> newCells(size*size*size, 0);

    uint32_t copyDimensions = std::min(_currentSize, (uint32_t)size); 
    for(int i = 0; i < copyDimensions; i++){
        for(int j = 0; j < copyDimensions; j++){
            for(int k = 0; k < copyDimensions; k++){
                newCells[i + size*j + size*size*k] = _cells[i + _currentSize*j + _currentSize*_currentSize*k];
            }
        }
    }

    _currentSize = size;
    _cells = std::move(newCells);
    _nextCells.assign(_currentSize*_currentSize*_currentSize, 0);
    CalculateOffsets();
}

void Grid3D::Clear(){
    std::fill(_cells.begin(), _cells.end(), 0);
    std::fill(_nextCells.begin(), _nextCells.end(), 0);
    // Just fill the values with 0 so that every cell is dead
}

void Grid3D::RandomSeed(float density, uint32_t seed){
    Clear();

    std::mt19937 generator(seed != 0 ? seed : std::random_device{}());
    std::bernoulli_distribution dist(density);

    uint32_t total_cells = _currentSize * _currentSize * _currentSize;

    for(uint32_t i = 0; i < total_cells; i++){
        if(dist(generator)){
            _cells[i] = 1;
        }
    }
}
// Again, this is for just a 6 neighbour setup (Von Neumann)
// This is faster cause duh, but Moore's 26 neighbour system is kinda needed for most of the different rulesets I've defined in the preset enum
unsigned int Grid3D::GetNeighbourCount_Neumann(int x, int y, int z) const {
    int count = 0;

    int countOffsets[6][3] = {
        {-1, 0, 0}, {1, 0, 0},
        {0, -1, 0}, {0, 1, 0},
        {0, 0, -1}, {0, 0, 1}
    };

    for(int i = 0; i < 6; i++){
        int nx = (x + countOffsets[i][0] + _currentSize) % _currentSize;
        int ny = (y + countOffsets[i][1] + _currentSize) % _currentSize;
        int nz = (z + countOffsets[i][2] + _currentSize) % _currentSize;
        count += GetCell(nx, ny, nz);
    }
    return count;
}

// This is the more complicated Moore 3D neighbours looping (-1, 1)
unsigned int Grid3D::GetNeighbourCount_Moore(int x, int y, int z) const {
    int count = 0;
    // Will just loop through all the surrounding cubes
    for(int dz = -1; dz <= 1; dz++){
        for(int dy = -1; dy <= 1; dy++){
            for(int dx = -1; dx <= 1; dx++){
                if (dx == 0 && dy == 0 && dz == 0) continue;
                int nx = (x + dx + _currentSize) % _currentSize;
                int ny = (y + dy + _currentSize) % _currentSize;
                int nz = (z + dz + _currentSize) % _currentSize;

                count += GetCell(nx, ny, nz);
            }
        }
    }
    return count;
}

unsigned int Grid3D::GetNeighbourCount(int x, int y, int z, Neighbourhood3D type) const{
    if(type == Neighbourhood3D::MOORE)
        return GetNeighbourCount_Moore(x, y, z);
    
    return GetNeighbourCount_Neumann(x, y, z);
}

void Grid3D::Update(const Rulesets3D& ruleset, Neighbourhood3D type){
    int size = static_cast<int>(_currentSize);

    // This line is for using OpenMP for multithreading
    #pragma omp parallel for schedule(static)
    // Tells the compiler "Ayo, my man could you spawn some worker threads to get this done real quick?" and then optionally daps up the compiler for the love of the game.
    for(int z = 0; z < size; z++){
        for(int y = 0; y < size; y++){
            for(int x = 0; x < size; x++){
                int index = x + size * y + size * size * z;
                unsigned int neighbours = 0;
                
                // If the cell is at the boundary, just use the slower function since I know its safe to use in that case
                if(x == 0 || x == size - 1 || y == 0 || y == size - 1 || z == 0 || z == size - 1){
                    neighbours = GetNeighbourCount(x, y, z, Neighbourhood3D::MOORE);
                }else{
                    neighbours = GetNeighboutCount_FastMoore(index);
                }
                uint8_t curr_state = _cells[index];
                _nextCells[index] = ruleset.EvaluateState(curr_state, neighbours);
            }
        }
    }
    std::swap(_cells, _nextCells);
}

inline unsigned int Grid3D::GetNeighboutCount_FastMoore(int index) const {
    const uint8_t* c = _cells.data() + index;
    const int* o = _precompMooreOffsets;

    return(
        c[o[0]] + c[o[1]] + c[o[2]] + c[o[3]] + c[o[4]] + c[o[5]] + c[o[6]] + 
        c[o[7]] + c[o[8]] + c[o[9]] + c[o[10]] + c[o[11]] + c[o[12]] + c[o[13]] + 
        c[o[14]] + c[o[15]] + c[o[16]] + c[o[17]] + c[o[18]] + c[o[19]] + c[o[20]] + 
        c[o[21]] + c[o[22]] + c[o[23]] + c[o[24]] + c[o[25]]
    );
}