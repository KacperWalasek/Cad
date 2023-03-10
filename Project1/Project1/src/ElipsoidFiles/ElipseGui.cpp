#include "ElipseGui.h"

ElipseGui::ElipseGui(Elipse& elipse, AdaptiveRendering& rendering)
	: elipse(elipse), rendering(rendering)
{
}

void ElipseGui::Render()
{
    ImGui::Begin("Elipse");

    float changedRX = elipse.rX, changedRY = elipse.rY, changedRZ = elipse.rZ;
    if (ImGui::InputFloat("RadiusX", &changedRX))
    {
        elipse.setRadious(changedRX, elipse.rY, elipse.rZ);
        elipse.updateTransforamtions();
        rendering.hardReset = true;
    }
    if (ImGui::InputFloat("RadiusY", &changedRY))
    {
        elipse.setRadious(elipse.rX, changedRY, elipse.rZ);
        elipse.updateTransforamtions();
        rendering.hardReset = true;
    }
    if (ImGui::InputFloat("RadiusZ", &changedRZ))
    {
        elipse.setRadious(elipse.rX, elipse.rY, changedRZ);
        elipse.updateTransforamtions();
        rendering.hardReset = true;
    }

    float changedM = elipse.m;
    if (ImGui::InputFloat("Specular m", &changedM))
    {
        elipse.m = changedM;
        elipse.updateTransforamtions();
        rendering.hardReset = true;
    }

    ImGui::End();
}
