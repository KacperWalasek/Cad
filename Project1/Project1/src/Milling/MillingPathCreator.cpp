#include "MillingPathCreator.h"
#include "../Rendering/TextureRenderer.h"
#include "SceneHeightMapRenderer.h"
#include "../Debuger/Debuger.h"
#include <numbers>

const glm::fvec4 MillingPathCreator::rect = { -25, 25,10,-40 };
const glm::fvec2 MillingPathCreator::zRange = { 0,-10 };
unsigned int MillingPathCreator::renderTextureHeightMap(Scene& scene)
{
	SceneHeightMapRenderer sceneRenderer(scene);
	TextureRenderer tr(textureSize, textureSize, 3, true);

	float rangeLen = zRange.y - zRange.x;

	// setup uniforms
	VariableManager vm;
	vm.AddVariable("projMtx", glm::ortho<float>(rect.x,rect.y,rect.z, rect.w, 0.001f, fabsf(rangeLen)*2));
	vm.AddVariable("viewMtx", glm::translate(glm::rotate<float>(glm::identity<glm::fmat4x4>(), std::numbers::pi, { 0,1,0 }), { 0,0,-rangeLen }));
	vm.AddVariable("modelMtx", glm::identity<glm::fmat4x4>());
	vm.AddVariable("color", glm::fvec4(0,1,1,1));
	vm.AddVariable("divisionV", 4);
	vm.AddVariable("divisionU", 4);
	vm.AddVariable("heightRange", zRange);

	// render
	tr.Clear({ 0,0,0,0 });
	tr.Render(sceneRenderer, vm);
	
	return tr.getTextureId();
}

MillingPath MillingPathCreator::CreateRoughtingPath(Scene& scene)
{
	Debuger::ShowTexture(renderTextureHeightMap(scene));
	
	// wygeneruj mape wysokosci
	// zsampluj mape wyzokosci sciezkami dwukrotnie
	return MillingPath();
	
}
