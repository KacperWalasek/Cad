#pragma once
#include <memory>
#include <vector>
#include "../interfaces/ISceneElement.h"
#include "../interfaces/IGui.h"
#include "../interfaces/ISceneTracker.h"
#include "Cursor.h"
#include <list>
#include "Center.h"

class Scene : public IGui
{
public:
	Scene(std::vector<std::pair<std::shared_ptr<ISceneElement>, bool>> objects, Camera& camera);

	std::vector<std::pair<std::shared_ptr<ISceneElement>,bool>> objects;
	std::shared_ptr<ISceneElement> lastSelected;
	std::shared_ptr<Cursor> cursor;
	Center center;

	bool showPoints;
	std::vector<std::shared_ptr<ISceneTracker>> trackers;

	void Add(std::shared_ptr<ISceneElement> obj, bool cursorPosition = true);
	void Remove(std::shared_ptr<ISceneElement> obj);
	void Select(std::pair<std::shared_ptr<ISceneElement>, bool>& obj);
	void Select(std::shared_ptr<ISceneElement> obj);

	virtual bool RenderGui() override;

};