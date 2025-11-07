#include "Cylinder.h"
#include <cmath>
#include <vector>

void buildCylinder(float baseRadius,
                   float topRadius,
                   float height,
                   int sectorCount,
                   int stackCount,
                   std::vector<float>& vertices,
                   std::vector<float>& normals,
                   std::vector<float>& texCoords,
                   std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359f;
    float sectorStep = 2.0f * PI / sectorCount;
    float stackStep = height / stackCount;
    float radiusStep = (topRadius - baseRadius) / stackCount;

    // Vertices, normals, texCoords
    for(int i = 0; i <= stackCount; ++i)
    {
        float curRadius = baseRadius + i * radiusStep;
        float z = -0.5f * height + i * stackStep;    // centered about origin
        for(int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * sectorStep;
            float x = curRadius * cosf(sectorAngle);
            float y = curRadius * sinf(sectorAngle);

            // position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normal (for side surface)
            float nx = cosf(sectorAngle);
            float ny = sinf(sectorAngle);
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(0.0f);

            // texture coord
            float s = (float)j / (float)sectorCount;
            float t = (float)i / (float)stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    // Indices for side surfaces
    for(int i = 0; i < stackCount; ++i)
    {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // triangle 1
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            // triangle 2
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }
}


Cylinder::Cylinder(float baseRadius,
                   float topRadius,
                   float height,
                   int   sectorCount,
                   int   stackCount,
                   bool  wireframe) : BasePrimitive(wireframe)
{
    buildCylinder(baseRadius, topRadius, height, sectorCount, stackCount,m_vertices,m_normals,m_texCoords,m_indices);
}
