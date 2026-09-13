#pragma once
#include "grid.hpp"
#include "rendering_system.hpp"
#include <iostream>
#include <memory>

class Application3D{
public:
    Application3D(const char* title = "3D Cellular Automata");
    ~Application3D();

    void Run();
private:

    void ProcessInput();
    void Update();
    void Render();
    void RenderGUI();
    void RecenterCamera();

    Grid3D _grid;
    std::unique_ptr<GridRenderer3D> _renderer;
    std::unique_ptr<Rulesets3D> _activeRuleset;

    Camera3D _camera;
    
    float _updateInterval;
    float _timeAccumulator;
    bool _isPaused;

    Neighbourhood3D _neighbourhood;

    // UI VAriables
    int _uiGridSize;
    float _uiDensity;
    int _uiSeed;
    int _uiPresetIndex;

    // Survival Rules Bounds for custom rulesets
    int _bMin, _bMax, _sMin, _sMax;
};