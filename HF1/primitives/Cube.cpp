#include "Cube.h"
#include <vector>

void buildCube(float size,
               std::vector<float>& vertices,
               std::vector<float>& normals,
               std::vector<float>& texCoords,
               std::vector<unsigned int>& indices)
{
    float half = size / 2.0f;

    // 8 cube corners
    float cubeVertices[8][3] = {
        {-half, -half, -half}, {half, -half, -half},
        {half,  half, -half}, {-half,  half, -half},
        {-half, -half,  half}, {half, -half,  half},
        {half,  half,  half}, {-half,  half,  half}
    };

    // cube normals for each face
    float cubeNormals[6][3] = {
        { 0,  0, -1}, // back
        { 0,  0,  1}, // front
        { 0, -1,  0}, // bottom
        { 0,  1,  0}, // top
        {-1,  0,  0}, // left
        { 1,  0,  0}  // right
    };

    // texture coordinates
    float cubeTexCoords[4][2] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
    };

    // indices for each face (two triangles per face)
    unsigned int faceIndices[6][4] = {
        {0, 1, 2, 3}, // back
        {4, 5, 6, 7}, // front
        {0, 1, 5, 4}, // bottom
        {3, 2, 6, 7}, // top
        {0, 3, 7, 4}, // left
        {1, 2, 6, 5}  // right
    };

    // Generate vertices, normals, texCoords
    for (int f = 0; f < 6; ++f)
    {
        for (int v = 0; v < 4; ++v)
        {
            int vi = faceIndices[f][v];
            vertices.push_back(cubeVertices[vi][0]);
            vertices.push_back(cubeVertices[vi][1]);
            vertices.push_back(cubeVertices[vi][2]);

            normals.push_back(cubeNormals[f][0]);
            normals.push_back(cubeNormals[f][1]);
            normals.push_back(cubeNormals[f][2]);

            texCoords.push_back(cubeTexCoords[v][0]);
            texCoords.push_back(cubeTexCoords[v][1]);
        }

        // Add indices (two triangles per face)
        unsigned int startIndex = f * 4;
        indices.push_back(startIndex);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);

        indices.push_back(startIndex);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
    }
}

void GenerateCubeAtlas(std::vector<float>& vertices,
                       std::vector<float>& normals,
                       std::vector<float>& texCoords,
                       std::vector<unsigned int>& indices)
{
    vertices.clear(); normals.clear(); texCoords.clear(); indices.clear();

    // 1. Define the Layout of your Texture Atlas
    // Assume a 3 Columns x 2 Rows grid
    float colStep = 1.0f / 3.0f;
    float rowStep = 1.0f / 2.0f;

    // Define UV ranges for each face
    // Format: { uMin, uMax, vMin, vMax }
    // CHANGE THESE indices [0] through [5] to match where your images are in the grid!
    // Current Order: Front, Back, Top, Bottom, Right, Left
    struct UV { float u0, u1, v0, v1; };
    UV facesUV[6];

    // Example Mapping:
    facesUV[0] = { 0.0f,     colStep,   0.0f,    rowStep }; // Front  (Col 0, Row 0)
    facesUV[1] = { colStep,  colStep*2, 0.0f,    rowStep }; // Back   (Col 1, Row 0)
    facesUV[2] = { colStep*2, 1.0f,     0.0f,    rowStep }; // Top    (Col 2, Row 0)
    facesUV[3] = { 0.0f,     colStep,   rowStep, 1.0f    }; // Bottom (Col 0, Row 1)
    facesUV[4] = { colStep,  colStep*2, rowStep, 1.0f    }; // Right  (Col 1, Row 1)
    facesUV[5] = { colStep*2, 1.0f,     rowStep, 1.0f    }; // Left   (Col 2, Row 1)

    // Helper to push a vertex
    auto pushVert = [&](float x, float y, float z, float nx, float ny, float nz, float u, float v) {
        vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
        normals.push_back(nx); normals.push_back(ny); normals.push_back(nz);
        texCoords.push_back(u); texCoords.push_back(v);
    };

    // 2. Generate Geometry
    // We strictly use CCW winding (BottomLeft -> BottomRight -> TopRight -> TopLeft)
    // so Vulkan sees the face.

    // --- FACE 0: FRONT (Z+) ---
    // Normal (0, 0, 1)
    UV uv = facesUV[0];
    pushVert(-1, -1,  1,  0, 0, 1,  uv.u0, uv.v1); // Bottom-Left
    pushVert( 1, -1,  1,  0, 0, 1,  uv.u1, uv.v1); // Bottom-Right
    pushVert( 1,  1,  1,  0, 0, 1,  uv.u1, uv.v0); // Top-Right
    pushVert(-1,  1,  1,  0, 0, 1,  uv.u0, uv.v0); // Top-Left

    // --- FACE 1: BACK (Z-) ---
    // Normal (0, 0, -1) - Look FROM the back, so X is inverted in view
    uv = facesUV[1];
    pushVert( 1, -1, -1,  0, 0, -1, uv.u0, uv.v1); // Bottom-Left (physically right)
    pushVert(-1, -1, -1,  0, 0, -1, uv.u1, uv.v1); // Bottom-Right (physically left)
    pushVert(-1,  1, -1,  0, 0, -1, uv.u1, uv.v0); // Top-Right
    pushVert( 1,  1, -1,  0, 0, -1, uv.u0, uv.v0); // Top-Left

    // --- FACE 2: TOP (Y+) ---
    // Normal (0, 1, 0)
    uv = facesUV[2];
    pushVert(-1,  1,  1,  0, 1, 0,  uv.u0, uv.v1); // Bottom-Left (Front-Left)
    pushVert( 1,  1,  1,  0, 1, 0,  uv.u1, uv.v1); // Bottom-Right (Front-Right)
    pushVert( 1,  1, -1,  0, 1, 0,  uv.u1, uv.v0); // Top-Right (Back-Right)
    pushVert(-1,  1, -1,  0, 1, 0,  uv.u0, uv.v0); // Top-Left (Back-Left)

    // --- FACE 3: BOTTOM (Y-) ---
    // Normal (0, -1, 0)
    uv = facesUV[3];
    pushVert(-1, -1, -1,  0, -1, 0, uv.u0, uv.v1); // Bottom-Left (Back-Left)
    pushVert( 1, -1, -1,  0, -1, 0, uv.u1, uv.v1); // Bottom-Right (Back-Right)
    pushVert( 1, -1,  1,  0, -1, 0, uv.u1, uv.v0); // Top-Right (Front-Right)
    pushVert(-1, -1,  1,  0, -1, 0, uv.u0, uv.v0); // Top-Left (Front-Left)

    // --- FACE 4: RIGHT (X+) ---
    // Normal (1, 0, 0)
    uv = facesUV[4];
    pushVert( 1, -1,  1,  1, 0, 0,  uv.u0, uv.v1); // Bottom-Left (Front)
    pushVert( 1, -1, -1,  1, 0, 0,  uv.u1, uv.v1); // Bottom-Right (Back)
    pushVert( 1,  1, -1,  1, 0, 0,  uv.u1, uv.v0); // Top-Right
    pushVert( 1,  1,  1,  1, 0, 0,  uv.u0, uv.v0); // Top-Left

    // --- FACE 5: LEFT (X-) ---
    // Normal (-1, 0, 0)
    uv = facesUV[5];
    pushVert(-1, -1, -1, -1, 0, 0,  uv.u0, uv.v1); // Bottom-Left (Back)
    pushVert(-1, -1,  1, -1, 0, 0,  uv.u1, uv.v1); // Bottom-Right (Front)
    pushVert(-1,  1,  1, -1, 0, 0,  uv.u1, uv.v0); // Top-Right
    pushVert(-1,  1, -1, -1, 0, 0,  uv.u0, uv.v0); // Top-Left

    // 3. Generate Indices
    // Every face follows the same pattern: 0, 1, 2,  2, 3, 0
    for (int i = 0; i < 6; i++) {
        unsigned int start = i * 4;
        indices.push_back(start + 0);
        indices.push_back(start + 1);
        indices.push_back(start + 2);

        indices.push_back(start + 2);
        indices.push_back(start + 3);
        indices.push_back(start + 0);
    }
}


Cube::Cube(bool atlas): BasePrimitive()
{
    if (atlas) GenerateCubeAtlas(m_vertices,m_normals,m_texCoords,m_indices);
    else buildCube(1,m_vertices,m_normals,m_texCoords,m_indices);
}
