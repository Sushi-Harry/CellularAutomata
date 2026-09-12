#include "grid.hpp"
#include <iostream>
#include <algorithm> // For some reason I keep forgetting that std::min() function blongs to this header
#include <random>

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