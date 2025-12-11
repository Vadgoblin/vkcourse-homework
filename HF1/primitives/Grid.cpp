#include "Grid.h"

#include "context.h"
#include <vector>

namespace {
#include "shaders/grid.frag_include.h"
#include "shaders/grid.vert_include.h"
}


void buildGrid(float width, float depth, int rows, int cols,
               std::vector<float>& vertices,
               std::vector<float>& normals,
               std::vector<float>& texCoords,
               std::vector<unsigned int>& indices)
{
    rows = std::max(2, rows);
    cols = std::max(2, cols);

    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;
    float dx = width / (cols - 1);
    float dz = depth / (rows - 1);

    // Generate vertices, normals, and texCoords
    for (int i = 0; i < rows; ++i)
    {
        float z = i * dz - halfDepth;
        for (int j = 0; j < cols; ++j)
        {
            float x = j * dx - halfWidth;

            // Vertex position
            vertices.push_back(x);
            vertices.push_back(0.0f); // y-coordinate for flat grid
            vertices.push_back(z);

            // Normal pointing up
            normals.push_back(0.0f);
            normals.push_back(1.0f);
            normals.push_back(0.0f);

            // Texture coordinates
            texCoords.push_back((float)j / (cols - 1));
            texCoords.push_back((float)i / (rows - 1));
        }
    }

    // Generate indices (two triangles per quad)
    for (int i = 0; i < rows - 1; ++i)
    {
        for (int j = 0; j < cols - 1; ++j)
        {
            unsigned int start = i * cols + j;

            // Triangle 1
            indices.push_back(start);
            indices.push_back(start + cols);
            indices.push_back(start + 1);

            // Triangle 2
            indices.push_back(start + 1);
            indices.push_back(start + cols);
            indices.push_back(start + cols + 1);
        }
    }
}


Grid::Grid(float width, float depth, int rows, int cols) : BasePrimitive()
{
    buildGrid(width, depth, rows, cols,m_vertices, m_normals, m_texCoords, m_indices);

    m_shaderVertData   = SPV_grid_vert;
    m_shaderVertSize   = sizeof(SPV_grid_vert);
    m_shaderFragData   = SPV_grid_frag;
    m_shaderFragSize   = sizeof(SPV_grid_frag);
}