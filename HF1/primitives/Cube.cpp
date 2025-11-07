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



Cube::Cube(const float size, const bool wireframe): BasePrimitive(wireframe)
{
    buildCube(size,m_vertices,m_normals,m_texCoords,m_indices);
}
