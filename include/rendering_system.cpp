#include "rendering_system.hpp"
#include "raymath.h"
#include "rlgl.h"
/*
    ,_______,\--------\
    \_\   | | \ _____\ \
          | |  \ \    \ \
    ,_____|_|   \ \    \ \
    | |_____    _\ \____\ \
    \_______|   \_________/
*/

void GridRenderer2D::Draw(const Grid2D& grid){
    uint32_t size = (uint32_t)grid.GetSize();

    // Clearing the background with WHiTE COLOR. Basic raylib stuff
    ClearBackground(RAYWHITE);

    // So it didn't cross my mind at first but this is a better way to draw than just drawing every single block. Just draw the ones with the alive state (Black Color)
    // Why? Because white (dead state) blocks won't be visible on my choice of background even if I draw them.
    for(uint32_t y = 0; y < size; y++){
        for(uint32_t x = 0; x < size; x++){
            if(grid.GetCell(x, y) == 1){
                DrawRectangle(x * _cellSize, y * _cellSize, _cellSize, _cellSize, BLACK);
            }
        }
    }

    DrawGridLines(size);
}

void GridRenderer2D::DrawGridLines(int gridSize){
    int totalPixels = gridSize * _cellSize; // Total pixels along one direction since gridSize contains a single dimension's size

    // Vertical lines first and then the horizontal ones
    for(int i = 0; i <= gridSize; i++){
        DrawLine(i*_cellSize, 0, i*_cellSize, totalPixels, BLACK);
    }
    for(int i = 0; i <= gridSize; i++){
        DrawLine(0, i*_cellSize, totalPixels, i*_cellSize, BLACK);
    }
}


/*  
    /-------\     \--------\
    \_____   \     \ _____\ \
        __\   \     \ \    \ \
        |__.   \     \ \    \ \
      ______\   \    _\ \____\ \
      \_________/    \_________/
*/

static const char* INSTANCING_VS = R"(
    #version 330
    in vec3 vertexPosition;
    in vec2 vertexTexCoord;
    in vec4 vertexColor;
    in mat4 instanceTransform;

    uniform mat4 mvp;

    out vec3 fragPosition;
    out vec2 fragTexCoord;
    out vec4 fragColor;

    void main() {
        vec4 worldPosition = instanceTransform * vec4(vertexPosition, 1.0);
        
        // Pass the exact 3D world coordinate to the fragment shader
        fragPosition = worldPosition.xyz; 
        fragTexCoord = vertexTexCoord;
        fragColor = vertexColor;

        gl_Position = mvp * worldPosition;
    }
)";

static const char* INSTANCING_FS = R"(
    #version 330
    in vec3 fragPosition;
    in vec2 fragTexCoord;
    in vec4 fragColor;

    // Standard Solid Color
    uniform vec4 colDiffuse;

    // New Gradient Uniforms
    uniform int u_shadingMode; // 0 = Solid, 1 = Gradient
    uniform vec3 u_gridCenter;
    uniform float u_maxDistance;
    uniform vec3 u_colorStart; // Center color
    uniform vec3 u_colorEnd;   // Edge color

    out vec4 finalColor;

    void main() {
        if (u_shadingMode == 1) {
            // Calculate distance from this pixel to the grid center
            float dist = distance(fragPosition, u_gridCenter);
            
            // Normalize distance to a 0.0 - 1.0 scale
            float t = clamp(dist / u_maxDistance, 0.0, 1.0);
            
            // Mix the two colors based on distance
            vec3 gradColor = mix(u_colorStart, u_colorEnd, t);
            finalColor = vec4(gradColor, 1.0);
        } else {
            finalColor = colDiffuse * fragColor;
        }
    }
)";


GridRenderer3D::GridRenderer3D(int cellSize, Color _activeColor) {
    _cellSize = cellSize;
    _cubeMesh = GenMeshCube(1.0, 1.0, 1.0);

    _cubeMaterial = LoadMaterialDefault();
    _cubeMaterial.maps[MATERIAL_MAP_DIFFUSE].color = _activeColor;

    Shader instanceShader = LoadShaderFromMemory(INSTANCING_VS, INSTANCING_FS);
    instanceShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(instanceShader, "mvp");
    instanceShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(instanceShader, "viewPos");
    _locShadingMode = GetShaderLocation(instanceShader, "u_shadingMode");
    _locGridCenter  = GetShaderLocation(instanceShader, "u_gridCenter");
    _locMaxDistance = GetShaderLocation(instanceShader, "u_maxDistance");
    _locColorStart  = GetShaderLocation(instanceShader, "u_colorStart");
    _locColorEnd    = GetShaderLocation(instanceShader, "u_colorEnd");
    _cubeMaterial.shader = instanceShader;

    _transforms.reserve(50000);
}

GridRenderer3D::~GridRenderer3D(){
    UnloadMesh(_cubeMesh);
    UnloadShader(_cubeMaterial.shader);
}

void GridRenderer3D::Draw(const Grid3D& grid){
    uint32_t size = (uint32_t)grid.GetSize();
    float floatCellSize = static_cast<float>(_cellSize);
    float halfCellSize = floatCellSize * 0.5F;
    _transforms.clear();
    if(_transforms.capacity() < size * size * size){
        _transforms.reserve(size * size * size);
    }

    // You'd think that this would work right away. It ddid not work at all on the first try. {Insert like 5 or 6 relieved emojis here}
    for (uint32_t z = 0; z < size; ++z) {
        for (uint32_t y = 0; y < size; ++y) {
            for (uint32_t x = 0; x < size; ++x) {
                if (grid.GetCell(x, y, z) == 1) {
                    Vector3 pos = {
                        x * floatCellSize + halfCellSize,
                        y * floatCellSize + halfCellSize,
                        z * floatCellSize + halfCellSize
                    };
                    // Scale and translate the base cube to match the correct coords in the 3d space
                    Matrix transform = { 0 };
                    transform.m0 = floatCellSize;
                    transform.m5 = floatCellSize;
                    transform.m10 = floatCellSize;
                    transform.m15 = 1.0F;
                    transform.m12 = pos.x;
                    transform.m13 = pos.y;
                    transform.m14 = pos.z;
                    _transforms.push_back(transform);
                }
            }
        }
    }

    if(!_transforms.empty()){
        float center = (size * floatCellSize) * 0.5f;
        float gridCenter[3] = { center, center, center };
        float maxDist = center * 1.73205F;
        SetShaderValue(_cubeMaterial.shader, _locShadingMode, &_shadingMode, SHADER_UNIFORM_INT);
        SetShaderValue(_cubeMaterial.shader, _locGridCenter, gridCenter, SHADER_UNIFORM_VEC3);
        SetShaderValue(_cubeMaterial.shader, _locMaxDistance, &maxDist, SHADER_UNIFORM_FLOAT);
        SetShaderValue(_cubeMaterial.shader, _locColorStart, colorCenter, SHADER_UNIFORM_VEC3);
        SetShaderValue(_cubeMaterial.shader, _locColorEnd, colorEdge, SHADER_UNIFORM_VEC3);
        DrawMeshInstanced(_cubeMesh, _cubeMaterial, _transforms.data(), static_cast<int>(_transforms.size()));
    }

    DrawGridLines(size);
}

void GridRenderer3D::DrawGridLines(int gridSize){
    float totalSize = gridSize * static_cast<float>(_cellSize);
    Vector3 boxCenter = { totalSize * 0.5f, totalSize * 0.5f, totalSize * 0.5f };
    DrawCubeWires(boxCenter, totalSize, totalSize, totalSize, DARKGRAY);

    rlPushMatrix();
        // Move the drawing origin to the center of the bottom face
        rlTranslatef(totalSize * 0.5f, 0.0f, totalSize * 0.5f);
        
        // Draw grid matching grid dimensions
        DrawGrid(gridSize, static_cast<float>(_cellSize));
    rlPopMatrix();
}

void GridRenderer3D::UpdateTransforms(const Grid3D& grid){
    uint32_t size = (uint32_t)grid.GetSize();
    float floatCellSize = static_cast<float>(_cellSize);
    float halfCellSize = floatCellSize * 0.5F;

    _transforms.clear();
    if (_transforms.capacity() < size * size * size) {
        _transforms.reserve(size * size * size);
    }

    for (uint32_t z = 0; z < size; ++z) {
        for (uint32_t y = 0; y < size; ++y) {
            for (uint32_t x = 0; x < size; ++x) {
                if (grid.GetCell(x, y, z) == 1) {
                    Matrix transform = { 0 };
                    transform.m0  = floatCellSize;
                    transform.m5  = floatCellSize;
                    transform.m10 = floatCellSize;
                    transform.m15 = 1.0f;
                    transform.m12 = x * floatCellSize + halfCellSize;
                    transform.m13 = y * floatCellSize + halfCellSize;
                    transform.m14 = z * floatCellSize + halfCellSize;
                    _transforms.push_back(transform);
                }
            }
        }
    }
}

void GridRenderer3D::Draw(int gridSize) {
    if(!_transforms.empty()){
        float floatCellSize = static_cast<float>(_cellSize);
        float center = (gridSize * floatCellSize) * 0.5f;
        float gridCenter[3] = { center, center, center };
        float maxDist = center * 1.73205F;
        
        SetShaderValue(_cubeMaterial.shader, _locShadingMode, &_shadingMode, SHADER_UNIFORM_INT);
        SetShaderValue(_cubeMaterial.shader, _locGridCenter, gridCenter, SHADER_UNIFORM_VEC3);
        SetShaderValue(_cubeMaterial.shader, _locMaxDistance, &maxDist, SHADER_UNIFORM_FLOAT);
        SetShaderValue(_cubeMaterial.shader, _locColorStart, colorCenter, SHADER_UNIFORM_VEC3);
        SetShaderValue(_cubeMaterial.shader, _locColorEnd, colorEdge, SHADER_UNIFORM_VEC3);
        
        DrawMeshInstanced(_cubeMesh, _cubeMaterial, _transforms.data(), static_cast<int>(_transforms.size()));
    }
    DrawGridLines(gridSize);
}