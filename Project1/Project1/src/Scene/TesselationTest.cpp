#include "TesselationTest.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

TesselationTest::TesselationTest(Camera& camera)
    :shader("Shaders/test.vert", "Shaders/fragmentShader.frag"), camera(camera)
{
    shader.Init();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    std::vector<float> vertices = {
        -10.0f,10.0f,0.0f,
        10.0f,10.0f,0.0f
    };
    std::vector<int> indices = {
        0,1
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shader.loadShaderFile("Shaders/test.tesc", GL_TESS_CONTROL_SHADER);
    shader.loadShaderFile("Shaders/test.tese", GL_TESS_EVALUATION_SHADER);
    shader.use();
    unsigned int colorLoc = glGetUniformLocation(shader.ID, "color");
    glUniform4f(colorLoc, 0.0f, 1.0f, 1.0f, 1.0f);
}

std::string TesselationTest::getName() const
{
	return "Tesselation";
}

void TesselationTest::Render(bool selected)
{
    shader.use();
    glPatchParameteri(GL_PATCH_VERTICES, 2);

    unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
    glm::fmat4x4 centerMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(centerMatrix));

    glBindVertexArray(VAO); 
    glDrawArrays(GL_PATCHES, 0, 2);
}
