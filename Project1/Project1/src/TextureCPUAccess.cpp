#include "TextureCPUAccess.h"
#include <GL/glew.h>

TextureCPUAccess::TextureCPUAccess(unsigned int tex, int sizeX, int sizeY)
	: tex(tex), sizeX(sizeX), sizeY(sizeY), data(sizeX* sizeY * 3)
{
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &data[0]);
}

glm::fvec3 TextureCPUAccess::getByIndex(int X, int Y) const
{
	int offset = (Y * sizeX + X) * 3;
	return { data[offset], data[offset+1], data[offset+2] };
}

glm::fvec3 TextureCPUAccess::getByUV(glm::fvec2 uv) const
{
	int x = roundf(uv.x * (sizeX - 1));
	int y = roundf(uv.y * (sizeY - 1));
	return getByIndex(x,y);
}