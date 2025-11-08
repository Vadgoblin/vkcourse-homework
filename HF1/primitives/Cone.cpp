#include "Cone.h"

void buildCone(float baseRadius, float height, int sectorCount, bool capBase,
               std::vector<float>& vertices,
               std::vector<float>& normals,
               std::vector<float>& texCoords,
               std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359f;
    float halfHeight = height * 0.5f;
    float sectorStep = 2 * PI / sectorCount;

    // --- Apex vertex (top)
    vertices.push_back(0.0f);       // x
    vertices.push_back(+halfHeight); // y
    vertices.push_back(0.0f);       // z

    normals.push_back(0.0f); normals.push_back(1.0f); normals.push_back(0.0f);
    texCoords.push_back(0.5f); texCoords.push_back(0.0f);

    // --- Base circle vertices
    for (int i = 0; i <= sectorCount; ++i) {
        float angle = i * sectorStep;
        float x = baseRadius * cos(angle);
        float y = -halfHeight;      // base at bottom
        float z = baseRadius * sin(angle);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        // Approximate side normal (points outward, not exact for smooth shading)
        float nx = cos(angle);
        float ny = baseRadius / height; // slope factor
        float nz = sin(angle);
        float length = std::sqrt(nx*nx + ny*ny + nz*nz);
        normals.push_back(nx/length);
        normals.push_back(ny/length);
        normals.push_back(nz/length);

        texCoords.push_back((x / baseRadius + 1.0f) * 0.5f);
        texCoords.push_back((z / baseRadius + 1.0f) * 0.5f);
    }

    unsigned int apexIndex = 0;
    unsigned int baseStart = 1;

    // --- Side faces (triangles)
    for (int i = 0; i < sectorCount; ++i) {
        indices.push_back(apexIndex);
        indices.push_back(baseStart + i);
        indices.push_back(baseStart + i + 1);
    }

    if (capBase) {
        // Center of base
        unsigned int centerIndex = static_cast<unsigned int>(vertices.size()/3);
        vertices.push_back(0.0f); vertices.push_back(-halfHeight); vertices.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(-1.0f); normals.push_back(0.0f);
        texCoords.push_back(0.5f); texCoords.push_back(0.5f);

        for (int i = 0; i < sectorCount; ++i) {
            indices.push_back(centerIndex);
            indices.push_back(baseStart + i + 1);
            indices.push_back(baseStart + i);
        }
    }
}

Cone::Cone(const float baseRadius, const float height, const int sectorCount, const bool capBase, const bool wireframe) : BasePrimitive(wireframe)
{
    buildCone(baseRadius, height, sectorCount, capBase,m_vertices,m_normals,m_texCoords,m_indices);
}
