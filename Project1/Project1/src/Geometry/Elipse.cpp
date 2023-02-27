#include "Elipse.h"

Elipse::Elipse()
{
}

void Elipse::setRadious(float x, float y, float z)
{
	a = x;
	b = y;
	c = z;
}

unsigned int Elipse::createTexture()
{
    int i, j;
    for (i = 0; i < TexSizeY; i++) {
        for (j = 0; j < TexSizeX; j++) {
            float x = j  - 0.5*(float)TexSizeX;
            float y = i  - 0.5* (float)TexSizeY;
            float z2 = (1 - a * x * x - b * y * y) / c;
            if (z2 > 0)
            {
                float z = 255;//sqrt(z2)*500;
                image[i][j][0] = (GLubyte)z;
            }
            else
                image[i][j][0] = (GLubyte)50;

            image[i][j][1] = (GLubyte)0;
            image[i][j][2] = (GLubyte)0;
            image[i][j][3] = (GLubyte)255;
        }
    }
    unsigned int texName;
    glGenTextures(1, &texName);
	return texName;
}
