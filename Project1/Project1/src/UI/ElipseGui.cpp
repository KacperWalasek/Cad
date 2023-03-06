#include "ElipseGui.h"

ElipseGui::ElipseGui(Elipse& elipse)
	: elipse(elipse)
{
}

void ElipseGui::Render()
{
    ImGui::Begin("Elipse");

    float changedRX = elipse.rX, changedRY = elipse.rY, changedRZ = elipse.rZ;
    if (ImGui::InputFloat("RadiusX", &changedRX))
        elipse.setRadious(changedRX, elipse.rY, elipse.rZ);
    if (ImGui::InputFloat("RadiusY", &changedRY))
        elipse.setRadious(elipse.rX, changedRY, elipse.rZ);
    if (ImGui::InputFloat("RadiusZ", &changedRZ))
        elipse.setRadious(elipse.rX, elipse.rY, changedRZ);

    float changedM = elipse.m;
    if (ImGui::InputFloat("Specular m", &changedM))
        elipse.m = changedM;

    ImGui::End();
}
