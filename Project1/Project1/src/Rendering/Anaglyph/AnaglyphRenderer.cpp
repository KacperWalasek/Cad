#include "AnaglyphRenderer.h"

void AnaglyphRenderer::UpdateCameras()
{
	leftCamera.dist = planeDistace;
	leftCamera.translationX = -eyeDistance / 2;

	rightCamera.dist = planeDistace;
	rightCamera.translationX = eyeDistance / 2;
}

AnaglyphRenderer::AnaglyphRenderer(Camera& camera)
	: eyeDistance(0.5f), 
	planeDistace(50),
	enabled(false),
	leftCamera(camera, -1.0f), 
	rightCamera(camera, 1.0f),
	screenShader("Shaders/ScreenTexture/screenTexture.vert", "Shaders/ScreenTexture/texture.frag")

{
	UpdateCameras();
}

void AnaglyphRenderer::Init()
{
	screenShader.Init();

	glGenFramebuffers(1, &fbLeft);
	glBindFramebuffer(GL_FRAMEBUFFER, fbLeft);

	glGenTextures(1, &texLeft);
	glBindTexture(GL_TEXTURE_2D, texLeft);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texLeft, 0);

	glGenFramebuffers(1, &fbRight);
	glBindFramebuffer(GL_FRAMEBUFFER, fbRight);

	glGenTextures(1, &texRight);
	glBindTexture(GL_TEXTURE_2D, texRight);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texRight, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	std::vector<float> vertices = {
		-1,1,0,
		1,1,0,
		1,-1,0,
		-1,-1,0
	};
	std::vector<int> inds = {
		0,1,2,
		0,2,3
	};


	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);
	glGenBuffers(1, &screenEBO);

	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AnaglyphRenderer::Render(Renderer& renderer, Scene& scene, std::vector<std::shared_ptr<IGui>>& guis, Camera& camera)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbLeft);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.variableManager.SetVariable("projMtx", leftCamera.GetProjectionMtx());
	renderer.variableManager.SetVariable("viewMtx", leftCamera.GetViewMtx());

	renderer.RenderScene(camera,scene); 

	glBindFramebuffer(GL_FRAMEBUFFER, fbRight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer.variableManager.SetVariable("projMtx", rightCamera.GetProjectionMtx());
	renderer.variableManager.SetVariable("viewMtx", rightCamera.GetViewMtx());

	renderer.RenderScene(camera, scene);

	screenShader.use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texLeft);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texRight);   
	glUniform1i(glGetUniformLocation(screenShader.ID, "screenTextureLeft"), 0);
	glUniform1i(glGetUniformLocation(screenShader.ID, "screenTextureRight"), 1);

	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(screenVAO);
	glDisable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	renderer.RenderGui(scene, guis);
	
}

bool AnaglyphRenderer::RenderGui()
{
	ImGui::Begin("Anaglyph");
	ImGui::Checkbox("Enabled", &enabled);
	if(ImGui::InputFloat("Eye distance", &eyeDistance))
		UpdateCameras();
	if (ImGui::InputFloat("Plane distance", &planeDistace))
	{
		if (planeDistace <= 0)
			planeDistace = 1.0f;
		UpdateCameras();
	}
	ImGui::End();
	return false;
}
