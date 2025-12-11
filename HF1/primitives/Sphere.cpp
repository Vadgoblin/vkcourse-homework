#include "Sphere.h"
#include <cmath>
#include <vector>

void buildSphere(float                      radius,
                 int                        sectorCount,
                 int                        stackCount,
                 std::vector<float>&        vertices,
                 std::vector<float>&        normals,
                 std::vector<float>&        texCoords,
                 std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359f;

    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();

    float x, y, z, xy;                           // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius; // normal
    float s, t;                                  // texCoords

    float sectorStep = 2 * PI / sectorCount;
    float stackStep  = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;    // from +pi/2 to ‑pi/2
        xy         = radius * cos(stackAngle); // r * cos(u)
        z          = radius * sinf(stackAngle); // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep; // from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // texture coordinate (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    // indices
    // k1: first vertex index of current stack
    // k2: first vertex index of next stack
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1); // beginning of current stack
        int k2 = k1 + sectorCount + 1;  // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector except for first/last stacks
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

Sphere::Sphere(float radius, int sectorCount, int stackCount) : BasePrimitive()
{
    buildSphere(radius,sectorCount,stackCount,m_vertices,m_normals,m_texCoords,m_indices);
}
