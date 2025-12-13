#include "Sphere.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ai generate, might be based on https://www.songho.ca/opengl/gl_sphere.html
void GenerateSphere(float radius,
                    int sectorCount,
                    int stackCount,
                    std::vector<float>& vertices,
                    std::vector<float>& normals,
                    std::vector<float>& texCoords,
                    std::vector<unsigned int>& indices)
{
    // Clear the vectors to ensure we start fresh
    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();

    float x, y, z, xy;                              // Vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // Vertex normal
    float s, t;                                     // Vertex texCoordinates

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    // 1. Generate vertices, normals, and UVs
    for (int i = 0; i <= stackCount; ++i)
    {
        // From pi/2 to -pi/2
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);             // r * cos(u)
        y = radius * sinf(stackAngle);              // r * sin(u)

        // Loop from 0 to sectorCount (inclusive) to duplicate the seam vertex
        // with different texture coordinates
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // from 0 to 2pi

            // Calculate Vertex Position
            // x = r * cos(u) * cos(v)
            // z = r * cos(u) * sin(v)
            x = xy * cosf(sectorAngle);
            z = xy * sinf(sectorAngle);

            // Add Vertex
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Add Normal (normalized vertex position)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // Add Texture Coordinates (range [0, 1])
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    // 2. Generate Indices
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // Beginning of current stack
        k2 = k1 + sectorCount + 1;      // Beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

Sphere::Sphere(float radius, int sectorCount, int stackCount) : BasePrimitive()
{
    GenerateSphere(radius,sectorCount,stackCount,m_vertices,m_normals,m_texCoords,m_indices);
}
