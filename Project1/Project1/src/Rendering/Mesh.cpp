#include "Mesh.h"
#include <GL/glew.h>
#include <algorithm>
#include <iterator>

Mesh Mesh::CombineMeshes(const Mesh& mesh1, const Mesh& mesh2)
{
    Mesh mesh;
    mesh.vertices = mesh1.vertices;
    mesh.vertices.insert(mesh.vertices.end(), mesh2.vertices.begin(), mesh2.vertices.end());

    mesh.indices = mesh1.indices;
    std::transform(mesh2.indices.begin(), mesh2.indices.end(), std::back_insert_iterator(mesh.indices),
        [&mesh1](const int& index) {return index + mesh1.vertices.size()/3; }
        );
    return mesh;
}

Mesh::Mesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void Mesh::Update()
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (vertices.size() != 0)
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    if (indices.size() != 0)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::Render()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}
