// These function implementations aren't anything new. I just offloaded all the manual stuff from main.cpp to this application class to keep main.cpp clean
#include "application.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include <algorithm>
#include "rulesets.hpp"

Application3D::Application3D(const char* title) : _updateInterval(0.2F), _timeAccumulator(0.0f), _isPaused(false),
    _neighbourhood(Neighbourhood3D::MOORE), _uiGridSize(20), _uiDensity(0.2f), _uiSeed(1337), _uiPresetIndex(0), _bMin(13), _bMax(14), _sMin(11), _sMax(19)
{
    InitWindow(900, 900, title);
    SetTargetFPS(60);
    rlImGuiSetup(1);

    _renderer = std::make_unique<GridRenderer3D>(1, BLACK);

    _grid.ResizeGrid(_uiGridSize);
    _grid.RandomSeed(_uiDensity, _uiSeed);

    _activeRuleset = std::make_unique<PresetRuleset3D>(RULE_PRESET3D::LIFE_3D);

    // Camera setup
    _camera = { 0 };
    _camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    _camera.fovy = 45.0f;
    _camera.projection = CAMERA_PERSPECTIVE;
    RecenterCamera();
}

Application3D::~Application3D(){
    rlImGuiShutdown();
    CloseWindow();
}

void Application3D::Run(){
    while(!WindowShouldClose()){
        ProcessInput();
        Update();
        Render();
    }
}

void Application3D::ProcessInput(){
    UpdateCamera(&_camera, CAMERA_ORBITAL);
    // Pause Controls
    if (IsKeyPressed(KEY_SPACE)) _isPaused = !_isPaused;
    if (_isPaused && IsKeyPressed(KEY_N)) {
        _grid.Update(*_activeRuleset, _neighbourhood);
    }
}

void Application3D::Update(){
    if(!_isPaused && _updateInterval > 0.0F){
        _timeAccumulator += GetFrameTime();
        int maxTicksPerFrame = 5;

        while(_timeAccumulator >= _updateInterval && maxTicksPerFrame > 0){
            _grid.Update(*_activeRuleset, _neighbourhood);
            _timeAccumulator -= _updateInterval;
            maxTicksPerFrame--;
        }
    }
}

void Application3D::Render(){
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        BeginMode3D(_camera);
            _renderer->Draw(_grid);
        EndMode3D();

        rlImGuiBegin();
            RenderGUI();
        rlImGuiEnd();
    EndDrawing();
}

void Application3D::RenderGUI(){
    ImGui::Begin("CONTROLS");
    // FPS COUNTERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
    ImGui::Text("FPS: %d", GetFPS());
    ImGui::Separator();

    // Playback COntrols (Pause/play and stuff)
    if(ImGui::Button(_isPaused ? "Resume" : "Pause")) _isPaused = !_isPaused;
    ImGui::SameLine();
    if(ImGui::Button("Advance By 1 Simulation Tick") && _isPaused) _grid.Update(*_activeRuleset, _neighbourhood);
    
    // Tick Interval (Did you know thhat ticks are arachnids? They're more closely related to spiders and scorpions than they are to iinsects :))
    // Another fact about ticks: believed to have originated during the Roman Empire as a shorthand version of the letter "V," which stood for veritas, the Latin word for truth.
    ImGui::SliderFloat("Tick Interval", &_updateInterval, 0.01F, 1.0F, "%.2F Seconds");
    ImGui::Separator();

    const char* presets[] = {
        "Life3D",
        "Amoeba3D",
        "Crystal3D",
        "Bays3D",
        "Custom"
    };
    if(ImGui::Combo("Ruleset", &_uiPresetIndex, presets, 5)){
        if(_uiPresetIndex < 4){
            _activeRuleset = std::make_unique<PresetRuleset3D>(static_cast<RULE_PRESET3D>(_uiPresetIndex));
        }
    }
    // This one is for the custom ruleset
    if(_uiPresetIndex == 4){
        ImGui::Indent();
        ImGui::Text("Custom Ruleset (Birth / Survival)");
        // 0 to 26 cause there's a total of 26 neighbours when following the Moore3D neighbour cound method. (Why? Just pick up a rubik's cube and count all the blocks around the block at the very center of the rubik's cube(Not at the center of a face, By center, I mean the actuual center which isn't visible. Inside the cube))
        ImGui::SliderInt("Birth Min", &_bMin, 0, 26);
        ImGui::SliderInt("Birth Max", &_bMax, 0, 26);
        ImGui::SliderInt("Survival Min", &_sMin, 0, 26);
        ImGui::SliderInt("Survival Max", &_sMax, 0, 26);

        if(ImGui::Button("Apply Custom Rule")){
            // So this is a boundary check to make sure that the minimum variables are smaller than maximum variables
            _bMax = std::max(_bMin, _bMax);
            _sMax = std::max(_sMin, _sMax);

            std::vector<int> birth, surv;
            for(int i = _bMin; i <= _bMax; i++) birth.push_back(i);
            for(int i = _sMin; i <= _sMax; i++) surv.push_back(i);
            // Now I've got the vectors for birth and survivals. Just need to implement a custom ruleset this that can take these vectors and use them to apply its rules
            _activeRuleset = std::make_unique<CustomRuleset3D>(birth, surv);
        }
        ImGui::Unindent();
    }
    ImGui::Separator();

    ImGui::SliderInt("Grid Size", &_uiGridSize, 10, 100);
    if(ImGui::Button("Apply New Dimensions")) {
        _grid.ResizeGrid(_uiGridSize);
        RecenterCamera();
    }

    ImGui::SliderFloat("Seed Density", &_uiDensity, 0.01F, 1.0F);
    ImGui::InputInt("Seed (0 = Random)", &_uiSeed);
    if(ImGui::Button("Generate With Given Seed")) _grid.RandomSeed(_uiDensity, _uiSeed);

    if(ImGui::Button("Generate With Random Seed")){
        _grid.RandomSeed(_uiDensity, 0);
        _uiSeed = 0;
    }
    ImGui::SameLine();
    if(ImGui::Button("Clear Grid")) _grid.Clear();

    ImGui::Separator();
    ImGui::Text("VISUALS");
    const char* shadingModes[] = { "Solid Black", "Distance Gradient" };
    ImGui::Combo("Shading Mode", &_renderer->_shadingMode, shadingModes, 2);
    if(_renderer->_shadingMode == 1){
        ImGui::Indent();
        ImGui::ColorEdit3("Center Color", _renderer->colorCenter);
        ImGui::ColorEdit3("Edge Color", _renderer->colorEdge);
        ImGui::Unindent();
    }
    ImGui::End();
}

void Application3D::RecenterCamera(){
    float gridCenter = (_grid.GetSize() * _renderer->GetCellSize()) * 0.5F;
    _camera.target = (Vector3){ gridCenter, gridCenter, gridCenter };
    _camera.position = (Vector3){ gridCenter + 80.0f, gridCenter + 80.0f, gridCenter + 120.0f };
}