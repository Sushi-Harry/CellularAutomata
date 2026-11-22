// These function implementations aren't anything new. I just offloaded all the manual stuff from main.cpp to this application class to keep main.cpp clean
#include "application.hpp"
#include "imgui.h"
#include "rlImGui.h"
#include <algorithm>
#include "rulesets.hpp"
#include <omp.h>



namespace {

constexpr int kWindowSize = 900;
constexpr int kTargetFps = 60;
constexpr int kCellSizePx = 5;
constexpr int kMaxTicksPerFrame = 5;

// Order must match the RULE_PRESET2D values (0, 1, 2). "Custom" must stay last.
const char* const kPresetNames[] = {"Life2D", "HighLife2D", "Seeds2D", "Custom"};
constexpr int kPresetCount = IM_ARRAYSIZE(kPresetNames);
constexpr int kCustomIndex = kPresetCount - 1;

}  // namespace

// ---------------- Lifetime ----------------

Application2D::Application2D(const char* title) {
    InitWindow(kWindowSize, kWindowSize, title);
    SetTargetFPS(kTargetFps);
    rlImGuiSetup(true);

    _renderer = std::make_unique<GridRenderer2D>(kCellSizePx);

    _grid.ResizeGrid(_uiGridSize);
    _grid.RandomSeed(_uiDensity, _uiSeed);

    SelectPreset(_uiPresetIndex);
}

Application2D::~Application2D() {
    _renderer.reset();  // free GPU resources while the window still exists
    rlImGuiShutdown();
    CloseWindow();
}

// ---------------- Main loop ----------------

void Application2D::Run() {
    while (!WindowShouldClose()) {
        ProcessInput();
        Update();
        Render();
    }
}

void Application2D::ProcessInput() {
    // Don't steal keys while the user is typing in a text box (e.g. the seed).
    if (ImGui::GetIO().WantTextInput)
        return;

    if (IsKeyPressed(KEY_SPACE))
        _isPaused = !_isPaused;

    if (_isPaused && IsKeyPressed(KEY_N))
        StepOnce();
}

void Application2D::Update() {
    if (_isPaused)
        return;

    _timeAccumulator += GetFrameTime();

    int ticks = 0;
    while (_timeAccumulator >= _updateInterval && ticks < kMaxTicksPerFrame) {
        StepOnce();
        _timeAccumulator -= _updateInterval;
        ++ticks;
    }

    // Hit the cap with time still owed: discard it so the backlog can't grow forever.
    if (_timeAccumulator >= _updateInterval)
        _timeAccumulator = 0.0f;
}

void Application2D::Render() {
    BeginDrawing();
    ClearBackground(BLACK);  // remove if GridRenderer2D::Draw already clears

    _renderer->Draw(_grid);

    rlImGuiBegin();
    RenderGUI();
    rlImGuiEnd();

    EndDrawing();
}

// ---------------- GUI ----------------

void Application2D::RenderGUI() {
    ImGui::Begin("2D CONTROLS");

    DrawPlaybackControls();
    ImGui::Separator();
    DrawRulesetControls();
    ImGui::Separator();
    DrawGridControls();

    ImGui::End();
}

void Application2D::DrawPlaybackControls() {
    ImGui::Text("FPS: %d", GetFPS());
    ImGui::Separator();

    if (ImGui::Button(_isPaused ? "Resume" : "Pause"))
        _isPaused = !_isPaused;

    ImGui::SameLine();

    ImGui::BeginDisabled(!_isPaused);  // greyed out while running
    if (ImGui::Button("Step 1 Tick (N)"))
        StepOnce();
    ImGui::EndDisabled();

    ImGui::SliderFloat("Tick Interval", &_updateInterval, 0.01f, 1.0f, "%.2f s");
}

void Application2D::DrawRulesetControls() {
    if (ImGui::Combo("Ruleset", &_uiPresetIndex, kPresetNames, kPresetCount))
        SelectPreset(_uiPresetIndex);

    if (_uiPresetIndex != kCustomIndex)
        return;

    ImGui::Indent();
    ImGui::TextUnformatted("Custom Ruleset (Birth / Survival)");

    // DragIntRange2 keeps min <= max by itself and returns true when either changes.
    bool changed = false;
    changed |= ImGui::DragIntRange2("Birth", &_bMin, &_bMax, 0.1f, 0, 8, "Min: %d", "Max: %d");
    changed |= ImGui::DragIntRange2("Survival", &_sMin, &_sMax, 0.1f, 0, 8, "Min: %d", "Max: %d");

    if (changed)
        ApplyCustomRule();

    ImGui::Unindent();
}

void Application2D::DrawGridControls() {
    ImGui::SliderInt("Grid Size", &_uiGridSize, 10, 100);

    if (ImGui::Button("Apply New Dimensions"))
        _grid.ResizeGrid(_uiGridSize);

    ImGui::SliderFloat("Seed Density", &_uiDensity, 0.01f, 1.0f);
    ImGui::InputInt("Seed (0 = Random)", &_uiSeed);

    if (ImGui::Button("Generate With Given Seed"))
        _grid.RandomSeed(_uiDensity, _uiSeed);

    if (ImGui::Button("Generate With Random Seed")) {
        _grid.RandomSeed(_uiDensity, 0);
        _uiSeed = 0;
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear Grid"))
        _grid.Clear();
}

// ---------------- Actions ----------------

void Application2D::StepOnce() {
    _grid.Update(*_activeRuleset);
}

void Application2D::SelectPreset(int index) {
    if (index == kCustomIndex) {
        ApplyCustomRule();
        return;
    }

    // Combo order == enum values, so the index can be cast directly.
    _activeRuleset = std::make_unique<PresetRuleset2D>(static_cast<RULE_PRESET2D>(index));
}

void Application2D::ApplyCustomRule() {
    std::vector<int> birth, survival;

    for (int n = _bMin; n <= _bMax; ++n)
        birth.push_back(n);

    for (int n = _sMin; n <= _sMax; ++n)
        survival.push_back(n);

    _activeRuleset = std::make_unique<CustomRuleset2D>(birth, survival);
}

Application3D::Application3D(const char* title) : _updateInterval(0.2F), _timeAccumulator(0.0f), _isPaused(false),
    _neighbourhood(Neighbourhood3D::MOORE), _uiGridSize(20), _uiDensity(0.2f), _uiSeed(1337), _uiPresetIndex(0), _bMin(13), _bMax(14), _sMin(11), _sMax(19)
{
    InitWindow(900, 900, title);
    SetTargetFPS(60);
    rlImGuiSetup(1);

    _renderer = std::make_unique<GridRenderer3D>(1, BLACK);

    _grid.ResizeGrid(_uiGridSize);
    _grid.RandomSeed(_uiDensity, _uiSeed);
    _renderer->UpdateTransforms(_grid);

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
    if (_isPaused && IsKeyPressed(KEY_N)) {
    _grid.Update(*_activeRuleset, _neighbourhood);
    _renderer->UpdateTransforms(_grid); // Add this
}
}

void Application3D::Update(){
    if(!_isPaused && _updateInterval > 0.0F){
        _timeAccumulator += GetFrameTime();
        int maxTicksPerFrame = 5;

       bool gridChanged = false;
        while(_timeAccumulator >= _updateInterval && maxTicksPerFrame > 0){
            _grid.Update(*_activeRuleset, _neighbourhood);
            _timeAccumulator -= _updateInterval;
            maxTicksPerFrame--;
            gridChanged = true;
        }
        if (gridChanged) {
            _renderer->UpdateTransforms(_grid);
        }
    }
}

void Application3D::Render(){
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        BeginMode3D(_camera);
            _renderer->Draw(_grid.GetSize());
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
    if(ImGui::Button("Generate With Given Seed")) {
        _grid.RandomSeed(_uiDensity, _uiSeed);
        _renderer->UpdateTransforms(_grid);
    }

    if(ImGui::Button("Generate With Random Seed")){
        _grid.RandomSeed(_uiDensity, 0);
        _uiSeed = 0;
    }
    ImGui::SameLine();
    if(ImGui::Button("Clear Grid")) {
        _grid.Clear();
        _renderer->UpdateTransforms(_grid);
    }

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