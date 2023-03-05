#include "Renderer.h"
#include <iostream>
#include <glfw/glfw3.h>
void Renderer::CreateTexture(float lastToutchTime)
{
    if (elipse.image == nullptr || imageFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        if (elipse.image)
        {
            delete elipse.image;
            elipse.image = imageFuture.get();
            if (glfwGetTime() - lastToutchTime<1)
                imageFuture = std::async(std::launch::async, [this]() {return elipse.createTexture(160, 90); });
            else
                imageFuture = std::async(std::launch::async, [this]() {return elipse.createTexture(1600, 900); });

        }
        else
        {
            elipse.image = elipse.createTexture(16, 9);
            imageFuture = std::async(std::launch::async, [this]() {return elipse.createTexture(16,9); });
            return;
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &texName);

        glBindTexture(GL_TEXTURE_2D, texName);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
            GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, elipse.currentSizeX,
            elipse.currentSizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
            elipse.image);
    }
}

Renderer::Renderer(Window& window)
    :shader("Shaders/vertexShader.vert","Shaders/fragmentShader.frag"), window(window)
{
}


void Renderer::Init()
{
    GLenum err = glewInit();
    shader.Init();

    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    elipse.setRadious(10, 10, 20);

}

void Renderer::Update(float lastToutchTime)
{
    if (window.curentMouseVectorX != 0 || window.curentMouseVectorY != 0)
    {
        if (window.shouldApply)
        {
            elipse.applyTemporaryTransformation();
            window.shouldApply = false;
        }
        if (window.isCrtlPressed)
        {
            float len = sqrt(window.curentMouseVectorX * window.curentMouseVectorX + window.curentMouseVectorY * window.curentMouseVectorY);
            elipse.temporaryTransformation.scale = { {100 / (1 + len),100 / (1 + len),100 / (1 + len),0} };
        }
        else if (window.isShiftPressed)
            elipse.temporaryTransformation.location = { {(float)-window.curentMouseVectorX,(float)window.curentMouseVectorY,0,0} };
        else
            elipse.temporaryTransformation.rotation = { {(float)window.curentMouseVectorY / 1000,-(float)window.curentMouseVectorX / 1000,0,0} };

        elipse.updateTransforamtions();
    }
    CreateTexture(lastToutchTime);
    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID, "texture"), 0);
}

void Renderer::Render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our first triangle
    glBindTexture(GL_TEXTURE_2D, texName);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0); // no need to unbind it every time 

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    elipse.renderUI();
    ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
