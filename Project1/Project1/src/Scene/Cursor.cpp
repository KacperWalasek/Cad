#include "Cursor.h"

Cursor::Cursor(Camera& camera)
	:size(10.0f), camera(camera)
{
	mesh.vertices = {
		0.0f,0.0f,0.0f,
		size,0.0f,0.0f,
		0.0f,size,0.0f,
		0.0f,0.0f,size
	};
	mesh.indices = {
		0,1, 0,2, 0,3
	};
	mesh.Update();

	UpdateFromWorld();
}

void Cursor::Render(bool selected, VariableManager& vm)
{
	mesh.Render();
}

void Cursor::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{

	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        switch (action)
        {
        case GLFW_PRESS:
			double posX, posY;
			int sizeX, sizeY;
			glfwGetCursorPos(window, &posX, &posY);
			glfwGetWindowSize(window, &sizeX, &sizeY);

			screenPosition = { posX / (sizeX / 2) - 1.0f, -posY / (sizeY / 2) + 1.0f };
			UpdateFromScreen();
			
			break;
        }
    }

}

void Cursor::UpdateFromScreen()
{
	glm::fvec4 zero(0, 0, 0, 1);
	glm::fvec4 cursorProj = camera.GetProjectionMatrix() * camera.GetViewMatrix() * transform.GetMatrix() * zero;
	cursorProj /= cursorProj.w;

	glm::fvec4 newCursorView(screenPosition.x, screenPosition.y, cursorProj.z, 1.0f);
	glm::fvec4 newCursorWorld = camera.transform.GetMatrix() * camera.GetInverseProjectionMatrix() * newCursorView;
	newCursorWorld /= newCursorWorld.w;
	transform.location = { newCursorWorld.x, newCursorWorld.y, newCursorWorld.z, 0 };
}

void Cursor::UpdateFromWorld()
{
	glm::fvec4 zero(0, 0, 0, 1);
	glm::fvec4 cursorProj = camera.GetProjectionMatrix() * camera.GetViewMatrix() * transform.GetMatrix() * zero;
	cursorProj /= cursorProj.w;

	screenPosition = { cursorProj.x,cursorProj.y };
}

void Cursor::Update(GLFWwindow* window)
{
	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		UpdateFromWorld();
}
bool Cursor::RenderGui(std::vector<std::shared_ptr<ISceneTracker>>& trackers)
{
	float worldPositionTab[3] = { transform.location[0], transform.location[1], transform.location[2] };
	float screenPositionTab[2] = { screenPosition.x, screenPosition.y };
	ImGui::Begin("Cursor");
	if (ImGui::InputFloat3("World Position", worldPositionTab))
	{
		transform.location = { worldPositionTab[0],worldPositionTab[1],worldPositionTab[2],0 };
		UpdateFromWorld();
	}
	if (ImGui::InputFloat2("Screen Position", screenPositionTab))
	{
		screenPosition = { screenPositionTab[0],screenPositionTab[1] };
		UpdateFromScreen();
	}
	ImGui::End();
	return false;
}
