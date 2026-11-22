#pragma once
#include "grid.hpp"
#include "rendering_system.hpp"
#include <iostream>
#include <memory>


class Application2D {
public:
    explicit Application2D(const char* title = "2D Cellular Automata");
    ~Application2D();

    // This object owns the window, so copying it would close the window twice.
    Application2D(const Application2D&) = delete;
    Application2D& operator=(const Application2D&) = delete;

    void Run();

private:
    // Main loop stages
    void ProcessInput();
    void Update();
    void Render();

    // GUI sections
    void RenderGUI();
    void DrawPlaybackControls();
    void DrawRulesetControls();
    void DrawGridControls();

    // Actions shared by keyboard and GUI
    void StepOnce();
    void SelectPreset(int index);
    void ApplyCustomRule();

    Grid2D _grid;
    std::unique_ptr<GridRenderer2D> _renderer;
    std::unique_ptr<Rulesets2D> _activeRuleset;

    // Simulation timing
    float _updateInterval = 0.2f;
    float _timeAccumulator = 0.0f;
    bool _isPaused = false;

    // UI state
    int _uiGridSize = 100;
    float _uiDensity = 0.2f;
    int _uiSeed = 1337;
    int _uiPresetIndex = 0;

    // Custom rule ranges (defaults = Life: B3 / S2-3)
    int _bMin = 3, _bMax = 3;
    int _sMin = 2, _sMax = 3;
};




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