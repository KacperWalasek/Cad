#include "ObjectFactory.h"
#include "../Geometry/Point.h"

std::vector<Patch> ObjectFactory::getSelectedPatches()
{
	std::vector<Patch> patches;
	for (auto& [el, selected] : scene.objects)
	{
		if (!selected)
			continue;
		
		auto surface = std::dynamic_pointer_cast<SurfaceC0>(el);
		if (!surface)
			continue;
		auto surPatches = surfaceToPatches(surface);
		patches.insert(patches.end(), surPatches.begin(), surPatches.end());
	}

	return patches;
}

bool ObjectFactory::checkEdge(std::vector<Patch>& patches, Patch& p, int c1, int c2)
{
	int counter = 0;
	for (auto& [el, s] : scene.objects)
	{
		auto surface = std::dynamic_pointer_cast<SurfaceC0>(el);
		if (!surface)
			continue;
		std::vector<Patch> surfacePatches = surfaceToPatches(surface);
		for (auto& sp : surfacePatches)
		{
				for (int spc = 0; spc < 4; spc++)
				{
					if (p.corners[c1] == sp.corners[spc] && p.corners[c2] == sp.corners[(spc + 1) % 4] ||
						p.corners[c2] == sp.corners[spc] && p.corners[c1] == sp.corners[(spc + 1) % 4])
						counter++;
					if (counter > 1)
						return false;
				}
		}
	}
	
	return true;
}

bool ObjectFactory::isEdgeBorder(std::map<int, std::vector<std::pair<int, int>>>& border, int p, int c1, int c2)
{
	for (auto& [b1, b2] : border[p])
		if ((b1 == c1 && b2 == c2) || (b2 == c1 && b1 == c2))
			return true;
	return false;
}

std::vector<Hole> ObjectFactory::findHoles(std::vector<Patch>& patches)
{
	std::map<int, std::vector<std::pair<int,int>>> borderEdges;
	for (int i = 0; i<patches.size(); i++)
	{
		std::vector<std::pair<int, int>> edges;
		for(int j =0; j<4; j++)
			if (checkEdge(patches, patches[i], j, (j + 1) % 4))
				edges.push_back({ j, (j + 1) % 4 });
		
		borderEdges.insert({ i,edges });
	}

	std::vector<Hole> holes;

	for (int i = 0; i < patches.size(); i++)
		for (int j = 0; j < i; j++)
			for (int k = 0; k < j; k++)
			{
				for (int c1 = 0; c1 < 4; c1++)
				{
					int c1Prev = (c1 - 1) < 0 ? 3 : c1 - 1;
					int c1Next = (c1 + 1) % 4;
					for (int c2 = 0; c2 < 4; c2++)
					{
						int c2Prev = (c2 - 1) < 0 ? 3 : c2 - 1;
						int c2Next = (c2 + 1) % 4;
						bool ij = patches[i].corners[c1] == patches[j].corners[c2];

						for (int c3 = 0; c3 < 4; c3++)
						{
							int c3Prev = (c3 - 1) < 0 ? 3 : c3 - 1;
							int c3Next = (c3 + 1) % 4;

							bool iNextK = patches[i].corners[c1Next] == patches[k].corners[c3] && isEdgeBorder(borderEdges, i, c1, c1Next);
							bool iPrevK = patches[i].corners[c1Prev] == patches[k].corners[c3] && isEdgeBorder(borderEdges, i, c1, c1Prev);
							bool ik = iNextK || iPrevK;

							bool jNextKAny = isEdgeBorder(borderEdges, j, c2, c2Next) &&
								(patches[j].corners[c2Next] == patches[k].corners[c3Next] && isEdgeBorder(borderEdges, j, c3, c3Next) ||
									patches[j].corners[c2Next] == patches[k].corners[c3Prev] && isEdgeBorder(borderEdges, j, c3, c3Prev));

							bool jPrevKAny = isEdgeBorder(borderEdges, j, c2, c2Prev) &&
								(patches[j].corners[c2Prev] == patches[k].corners[c3Next] && isEdgeBorder(borderEdges, j, c3, c3Next) ||
									patches[j].corners[c2Prev] == patches[k].corners[c3Prev] && isEdgeBorder(borderEdges, j, c3, c3Prev));
							bool jk = jPrevKAny || jNextKAny;

							Hole hole({ patches[i],patches[j],patches[k] }, { c1,c2,c3 });
							if (ij && ik && jk && !isHoleBlinded(hole))
								holes.push_back(hole);
						}
					}
				}
			}
	return holes;
}

void ObjectFactory::CreateFillPatch()
{
	std::vector<Patch> patches = getSelectedPatches();
	std::vector<Hole> holes = findHoles(patches);
	for (const Hole& hole : holes)
	{
		auto g = std::make_shared<GregoryPatch>(hole);
		scene.Add(g);
		for (int i = 0; i < 3; i++)
		{
			auto surface = std::dynamic_pointer_cast<SurfaceC0>(g->hole.patches[i].surface);
			if (!surface)
				continue;
			surface->po.push_back(g);
		}
	}
}

std::vector<Patch> ObjectFactory::surfaceToPatches(std::shared_ptr<SurfaceC0> surface)
{
	std::vector<Patch> patches;

	for (int i = 0; i < surface->countY; i++)
		for (int j = 0; j < surface->countX; j++)
		{
			Patch p;
			p.surface = surface;
			int rowSize = surface->cylinder ? surface->countX * 3 : (surface->countX * 3 + 1);
			int offset = 3 * i * rowSize + 3 * j;
			for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					if (3 * j + x == rowSize && surface->cylinder)
						p.points[y][x] = surface->points[3 * i * rowSize + y * rowSize];
					else
						p.points[y][x] = surface->points[offset + y * rowSize + x];
				}
			p.corners[0] = p.points[0][0];
			p.corners[1] = p.points[3][0];
			p.corners[2] = p.points[3][3];
			p.corners[3] = p.points[0][3];

			patches.push_back(p);
		}

	return patches;
}

bool ObjectFactory::isHoleBlinded(Hole& hole)
{
	for (auto& [el, s] : scene.objects)
	{
		auto gregory = std::dynamic_pointer_cast<GregoryPatch>(el);
		if (!gregory)
			continue;

		int matchCount = 0;
		for (int i = 0; i < 3; i++)
			if (gregory->hole.patches[i].corners[gregory->hole.corners[i]] == hole.patches[i].corners[hole.corners[i]])
				matchCount++;
		if(matchCount==3)
			return true;

	}
	return false;
}

ObjectFactory::ObjectFactory(Scene& scene)
	: scene(scene)
{
}

void ObjectFactory::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
		switch (key)
		{
		case GLFW_KEY_P:
			scene.Add(std::make_shared<Point>());
		case GLFW_KEY_F:
			CreateFillPatch();
		default:
			break;
		}
}
