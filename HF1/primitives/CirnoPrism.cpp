#include "CirnoPrism.h"

void buildCirnoPrism(
               std::vector<float>& vertices,
               std::vector<float>& normals,
               std::vector<float>& texCoords,
               std::vector<unsigned int>& indices)
{
    vertices.clear(); normals.clear(); texCoords.clear(); indices.clear();

    // buttom
    {
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);

        vertices.push_back(0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);

        vertices.push_back(0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(0.5f);

        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(0.5f);


        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);


        texCoords.push_back(0.0f);
        texCoords.push_back(0.0f);

        texCoords.push_back(0.0f);
        texCoords.push_back(0.0f);

        texCoords.push_back(0.0f);
        texCoords.push_back(0.0f);

        texCoords.push_back(0.0f);
        texCoords.push_back(0.0f);
    }

    //face
    {
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);

        vertices.push_back(0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);

        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.0f);

        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(6);


        texCoords.push_back(0.0f);
        texCoords.push_back(1.0f);

        texCoords.push_back(0.25f);
        texCoords.push_back(1.0f);

        texCoords.push_back(0.125f);
        texCoords.push_back(0.0f);
    }

    //side
    {
        vertices.push_back(0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);

        vertices.push_back(0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(0.5f);

        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.0f);

        indices.push_back(7);
        indices.push_back(8);
        indices.push_back(9);


        texCoords.push_back(0.25f);
        texCoords.push_back(1.0f);

        texCoords.push_back(0.5f);
        texCoords.push_back(1.0f);

        texCoords.push_back(0.25+0.125);
        texCoords.push_back(0.0f);
    }

    //back
    {
        vertices.push_back(0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(0.5f);

        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(0.5f);

        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.0f);

        indices.push_back(10);
        indices.push_back(11);
        indices.push_back(12);


        texCoords.push_back(0.5f);
        texCoords.push_back(1.0f);

        texCoords.push_back(0.75f);
        texCoords.push_back(1.0f);

        texCoords.push_back(0.5f + 0.125f);
        texCoords.push_back(0.0f);
    }

    //side
    {
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(0.5f);

        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);
        vertices.push_back(-0.5f);

        vertices.push_back(0.0f);
        vertices.push_back(0.5f);
        vertices.push_back(0.0f);

        indices.push_back(13);
        indices.push_back(14);
        indices.push_back(15);


        texCoords.push_back(0.75f);
        texCoords.push_back(1.0f);

        texCoords.push_back(1.0f);
        texCoords.push_back(1.0f);

        texCoords.push_back(0.75+0.125);
        texCoords.push_back(0.0f);
    }







    normals.push_back(0.0f);
    normals.push_back(0.0f);
    normals.push_back(0.0f);




    //
    // texCoords.push_back(0.0f);
    // texCoords.push_back(1.0f);
    //
    // texCoords.push_back(1.0f);
    // texCoords.push_back(0.0f);
    //
    // texCoords.push_back(1.0f);
    // texCoords.push_back(1.0f);

}

CirnoPrism::CirnoPrism() : BasePrimitive()
{
    buildCirnoPrism(m_vertices,m_normals,m_texCoords,m_indices);
}
