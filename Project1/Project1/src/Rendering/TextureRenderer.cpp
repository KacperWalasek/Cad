#include "TextureRenderer.h"

void TextureRenderer::createTexture(bool depthTest)
{
	GLint format = GL_RGB;
	switch (chanels)
	{
	case 1:
		format = GL_ALPHA;
	case 2:
		format = GL_RG;
	case 3: 
		format = GL_RGB;
	case 4:
		format = GL_RGBA;
	}

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, sizeX, sizeY, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	
	if (depthTest)
	{
		glGenTextures(1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, sizeX, sizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

} 
 
TextureRenderer::TextureRenderer(int sizeX, int sizeY, int chanels, bool depthTest)
	: sizeX(sizeX), sizeY(sizeY), chanels(chanels)
{
	createTexture(depthTest);
}

unsigned int TextureRenderer::getTextureId() const
{
	return tex;
}

void TextureRenderer::Clear(glm::fvec4 color)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
  
void TextureRenderer::Render(IRenderable& object, VariableManager& variableManager)
{
	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	glViewport(0, 0, sizeX, sizeY);
	glDepthFunc(GL_LESS);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	object.Render(false, variableManager);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthFunc(GL_LESS);
	glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

}
