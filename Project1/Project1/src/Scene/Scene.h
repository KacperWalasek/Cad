#pragma once
#include <memory>
#include <vector>
#include "ISceneElement.h"
#include "../interfaces/IGui.h"
#include "Cursor.h"

class Scene : public IGui
{
public:
	Scene(std::vector<std::shared_ptr<ISceneElement>> objects, Camera& camera);
	std::shared_ptr<ISceneElement> selected;
	std::vector<std::shared_ptr<ISceneElement>> objects;
	std::shared_ptr<Cursor> cursor;

	void Add(std::shared_ptr<ISceneElement> obj);

	virtual void RenderGui() override;

};