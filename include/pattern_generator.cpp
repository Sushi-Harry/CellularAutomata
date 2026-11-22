#include "pattern_generator.hpp"

void PatternGenerator::GenerateRandom2D(Grid2D& grid, float density, uint32_t seed) 
{
    grid.RandomSeed(density, seed);
}