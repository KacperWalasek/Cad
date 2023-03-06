#include "AdaptiveRendering.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

AdaptiveRendering::AdaptiveRendering(std::function<RGB(float x, float y)> calculatePixel, int maxSizeX, float  viewportRatio, int  initSizeX, int stepX)
    : calculatePixel(calculatePixel), maxSizeX(maxSizeX), viewportRatio(viewportRatio), initSizeX(initSizeX), stepX(stepX), currentTexture(nullptr), 
        currentSizeX(initSizeX), currentSizeY(initSizeX/viewportRatio), reset(true), aimedSizeX(initSizeX), aimedSizeY(initSizeX/viewportRatio)
{

}
AdaptiveRendering::~AdaptiveRendering()
{
    delete currentTexture;
}
RGB* AdaptiveRendering::createTexture(int sizeX, int sizeY)
{
    RGB* tmpImage = new RGB[sizeX * sizeY];

    for (int i = 0; i < sizeY; i++) {
        for (int j = 0; j < sizeX; j++) {
            float x = (j - 0.5 * (float)sizeX) * maxSizeX / sizeX;
            float y = (i - 0.5 * (float)sizeY) * maxSizeX / sizeX;
            tmpImage[i * sizeX + j] = calculatePixel(x,y);
        }
    }
    return tmpImage;
}


void AdaptiveRendering::Render()
{
    glBindTexture(GL_TEXTURE_2D, texName);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void AdaptiveRendering::Init()
{
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
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

bool AdaptiveRendering::Checkout()
{
	if(!reset && currentSizeX==maxSizeX)
		return false;
    
    if (currentTexture == nullptr)
    {
        aimedSizeX = initSizeX;
        aimedSizeY = initSizeX / viewportRatio;
        currentTexture = createTexture(aimedSizeX, aimedSizeY);
        job = std::async(std::launch::async, [this]() {return createTexture(aimedSizeX, aimedSizeY); });
        return false;
    }
    if (hardReset)
        reset = reset;
    if (hardReset || job.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        if (!hardReset)
        {
            delete currentTexture;
            currentTexture = job.get();
            currentSizeX = aimedSizeX;
            currentSizeY = aimedSizeY;

            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glGenTextures(1, &texName);

            glBindTexture(GL_TEXTURE_2D, texName);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentSizeX,
                currentSizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                currentTexture);
        }

        if (reset)
        {
            aimedSizeX = initSizeX;
            aimedSizeY = initSizeX / viewportRatio;
            reset = false;
            hardReset = false;
        }
        else
        {
            aimedSizeX = fmin(maxSizeX, currentSizeX * stepX);
            aimedSizeY = aimedSizeX / viewportRatio;

        }
        job = std::async(std::launch::async, [this]() {return createTexture(aimedSizeX, aimedSizeY); });

        return true;
    }
	
}

void AdaptiveRendering::Reset()
{
    reset = true;
}
