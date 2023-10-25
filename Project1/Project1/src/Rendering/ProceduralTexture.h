#pragma once
#include <vector>
#include <GL/glew.h>
 
class ProceduralTexture {
	std::vector<GLbyte> pixels;
	int sizeU, sizeV;
	unsigned int tex, fb;
public:
	~ProceduralTexture()
	{ 
		glDeleteTextures(1, &tex);
		glDeleteFramebuffers(1, &fb);
		
	}
	ProceduralTexture(int sizeU, int sizeV)
		: sizeU(sizeU), sizeV(sizeV), pixels(sizeU*sizeV*3)
	{
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeU, sizeV, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	std::vector<GLbyte>& getFlat() { return pixels; }
	
	std::tuple<GLbyte, GLbyte, GLbyte> getPixel(int u, int v) 
	{
		int offset = (v * sizeU + u) * 3;
		return { pixels[offset], pixels[offset + 1], pixels[offset + 2] };
	}

	void setPixel(int u, int v, std::tuple<GLbyte, GLbyte, GLbyte> pixel)
	{
		int offset = (v * sizeU + u) * 3;
		auto [x, y, z] = pixel;
		pixels[offset] = x;
		pixels[offset+1] = y;
		pixels[offset+2] = z;
	}

	void flush()
	{
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sizeU, sizeV, GL_RGB, GL_UNSIGNED_BYTE, &pixels[0]);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	unsigned int getTextureId() const { return tex; }
};