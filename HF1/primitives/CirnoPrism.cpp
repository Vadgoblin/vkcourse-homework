#include "CirnoPrism.h"

void buildCirnoPrism(
               std::vector<float>& vertices,
               std::vector<float>& normals,
               std::vector<float>& texCoords,
               std::vector<unsigned int>& indices)
{
    vertices.clear(); normals.clear(); texCoords.clear(); indices.clear();

    // Pre-calculated values for the pyramid slope
    // The slope is 2:1 (Height 1.0, Half-Width 0.5).
    // Vector length = sqrt(1*1 + 0.5*0.5) = 1.118
    // Y-component = 0.5 / 1.118 = 0.4472
    // X/Z-component = 1.0 / 1.118 = 0.8944
    float nY = 0.447214f;
    float nXZ = 0.894427f;

    // bottom (Normal points straight down: 0, -1, 0)
    {
        vertices.push_back(-0.5f); vertices.push_back(-0.5f); vertices.push_back(-0.5f);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);

        vertices.push_back(0.5f); vertices.push_back(-0.5f); vertices.push_back(-0.5f);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);

        vertices.push_back(0.5f); vertices.push_back(-0.5f); vertices.push_back(0.5f);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);

        vertices.push_back(-0.5f); vertices.push_back(-0.5f); vertices.push_back(0.5f);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);

        indices.push_back(0); indices.push_back(1); indices.push_back(2);
        indices.push_back(0); indices.push_back(2); indices.push_back(3);

        texCoords.push_back(0.0f); texCoords.push_back(0.0f);
        texCoords.push_back(1.0f); texCoords.push_back(0.0f); // Fixed texture scaling logic likely needed here
        texCoords.push_back(1.0f); texCoords.push_back(1.0f);
        texCoords.push_back(0.0f); texCoords.push_back(1.0f);
    }

    // face (Negative Z side -> Normal points Back-Up: 0, +, -)
    {
        vertices.push_back(-0.5f); vertices.push_back(-0.5f); vertices.push_back(-0.5f);
        normals.push_back(0.0f); normals.push_back(nY); normals.push_back(-nXZ);

        vertices.push_back(0.5f); vertices.push_back(-0.5f); vertices.push_back(-0.5f);
        normals.push_back(0.0f); normals.push_back(nY); normals.push_back(-nXZ);

        vertices.push_back(0.0f); vertices.push_back(0.5f); vertices.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(nY); normals.push_back(-nXZ);

        indices.push_back(4); indices.push_back(5); indices.push_back(6);

        texCoords.push_back(0.0f); texCoords.push_back(1.0f);
        texCoords.push_back(0.25f); texCoords.push_back(1.0f);
        texCoords.push_back(0.125f); texCoords.push_back(0.0f);
    }

    // side (Positive X side -> Normal points Right-Up: +, +, 0)
    {
        vertices.push_back(0.5f); vertices.push_back(-0.5f); vertices.push_back(-0.5f);
        normals.push_back(nXZ); normals.push_back(nY); normals.push_back(0.0f);

        vertices.push_back(0.5f); vertices.push_back(-0.5f); vertices.push_back(0.5f);
        normals.push_back(nXZ); normals.push_back(nY); normals.push_back(0.0f);

        vertices.push_back(0.0f); vertices.push_back(0.5f); vertices.push_back(0.0f);
        normals.push_back(nXZ); normals.push_back(nY); normals.push_back(0.0f);

        indices.push_back(7); indices.push_back(8); indices.push_back(9);

        texCoords.push_back(0.25f); texCoords.push_back(1.0f);
        texCoords.push_back(0.5f); texCoords.push_back(1.0f);
        texCoords.push_back(0.25f+0.125f); texCoords.push_back(0.0f);
    }

    // back (Positive Z side -> Normal points Forward-Up: 0, +, +)
    {
        vertices.push_back(0.5f); vertices.push_back(-0.5f); vertices.push_back(0.5f);
        normals.push_back(0.0f); normals.push_back(nY); normals.push_back(nXZ);

        vertices.push_back(-0.5f); vertices.push_back(-0.5f); vertices.push_back(0.5f);
        normals.push_back(0.0f); normals.push_back(nY); normals.push_back(nXZ);

        vertices.push_back(0.0f); vertices.push_back(0.5f); vertices.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(nY); normals.push_back(nXZ);

        indices.push_back(10); indices.push_back(11); indices.push_back(12);

        texCoords.push_back(0.5f); texCoords.push_back(1.0f);
        texCoords.push_back(0.75f); texCoords.push_back(1.0f);
        texCoords.push_back(0.5f + 0.125f); texCoords.push_back(0.0f);
    }

    // side (Negative X side -> Normal points Left-Up: -, +, 0)
    {
        vertices.push_back(-0.5f); vertices.push_back(-0.5f); vertices.push_back(0.5f);
        normals.push_back(-nXZ); normals.push_back(nY); normals.push_back(0.0f);

        vertices.push_back(-0.5f); vertices.push_back(-0.5f); vertices.push_back(-0.5f);
        normals.push_back(-nXZ); normals.push_back(nY); normals.push_back(0.0f);

        vertices.push_back(0.0f); vertices.push_back(0.5f); vertices.push_back(0.0f);
        normals.push_back(-nXZ); normals.push_back(nY); normals.push_back(0.0f);

        indices.push_back(13); indices.push_back(14); indices.push_back(15);

        texCoords.push_back(0.75f); texCoords.push_back(1.0f);
        texCoords.push_back(1.0f); texCoords.push_back(1.0f);
        texCoords.push_back(0.75f+0.125f); texCoords.push_back(0.0f);
    }
}

CirnoPrism::CirnoPrism() : BasePrimitive()
{
    buildCirnoPrism(m_vertices,m_normals,m_texCoords,m_indices);
}
