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

    // Vertex attributes
    in vec3 vertexPosition;
    in vec2 vertexTexCoord;
    in vec3 vertexNormal;
    in vec4 vertexColor;

    // Instancing attribute (4x4 matrix split across 4 vec4 attributes)
    in mat4 instanceTransform;

    // Input uniforms
    uniform mat4 mvp;
    uniform mat4 matNormal;

    // Output to fragment shader
    out vec3 fragPosition;
    out vec2 fragTexCoord;
    out vec4 fragColor;
    out vec3 fragNormal;

    void main()
    {
        // Compute vertex position using the per-instance matrix
        vec4 worldPosition = instanceTransform * vec4(vertexPosition, 1.0);
        
        fragPosition = vec3(worldPosition);
        fragTexCoord = vertexTexCoord;
        fragColor = vertexColor;
        fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 0.0)));

        // Final object transform
        gl_Position = mvp * worldPosition;
    }
)";

GridRenderer3D::GridRenderer3D(int cellSize, Color _activeColor) {
    _cellSize = cellSize;
    _cubeMesh = GenMeshCube(1.0, 1.0, 1.0);

    _cubeMaterial = LoadMaterialDefault();
    _cubeMaterial.maps[MATERIAL_MAP_DIFFUSE].color = _activeColor;

    Shader instanceShader = LoadShaderFromMemory(INSTANCING_VS, nullptr);
    instanceShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(instanceShader, "mvp");
    instanceShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(instanceShader, "viewPos");
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
                    Matrix transform = MatrixMultiply(
                        MatrixScale(floatCellSize, floatCellSize, floatCellSize),
                        MatrixTranslate(pos.x, pos.y, pos.z)
                    );
                    _transforms.push_back(transform);
                }
            }
        }
    }

    if(!_transforms.empty()){
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