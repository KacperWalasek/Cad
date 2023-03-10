#include "AdaptiveGui.h"

AdaptiveGui::AdaptiveGui(AdaptiveRendering& rendering)
    :rendering(rendering)
{
}

void AdaptiveGui::Render()
{
    ImGui::Begin("Addaptive Rendering");

    int changedInitX = rendering.initSizeX, changedStepX = rendering.stepX;
    if (ImGui::InputInt("Initial size X", &changedInitX))
        rendering.initSizeX = changedInitX;
    if (ImGui::InputInt("Step X", &changedStepX))
        rendering.stepX = changedStepX;

    ImGui::End();
}
