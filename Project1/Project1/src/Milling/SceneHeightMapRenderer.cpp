#include "SceneHeightMapRenderer.h"
#include "../Rendering/TextureRenderer.h"

SceneHeightMapRenderer::SceneHeightMapRenderer(Scene& scene)
{
	for (auto& o : scene.objects)
	{
		auto surf = std::dynamic_pointer_cast<IUVSurface>(o.first);
		if (surf)
			surfaces.push_back(surf);
	}
}

void SceneHeightMapRenderer::Render(bool selected, VariableManager& vm)
{
	for (auto s : surfaces)
	{
		s->setRenderState(SurfaceRenderState::HeightMap);
		auto sRenderable = std::dynamic_pointer_cast<IRenderable>(s);
		sRenderable->Render(selected, vm);
		s->setRenderState(SurfaceRenderState::Wireframe);
	}
}
