#include "GregoryPatch.h"
#include "SurfaceC0.h"
#include "../Curves/BezierCurve.h"

Indexer GregoryPatch::indexer;
std::array<std::pair<int, int>, 4> GregoryPatch::cornerIndices = { {
    {0,0},
    {3,0},
    {3,3},
    {0,3}
} };

glm::fvec3 GregoryPatch::derivative(float t, Patch& p, std::pair<int, int> c1, std::pair<int, int> c2)
{
    float nt = 1 - t;

    std::array<glm::fvec3, 3> d1;

    for (int i = 0; i < 3; i++)
        d1[i] = 3.0f * (pointOnEdge(i+1,p,c1,c2) - pointOnEdge(i, p, c1, c2));

    d1[2] = d1[2] * t + d1[1] * nt;
    d1[1] = d1[1] * t + d1[0] * nt;
    return d1[2] * t + d1[1] * nt;
}

std::array<std::array<std::pair<int, int>, 4>, 3> GregoryPatch::orderCorners()
{
    /*
           0     3
           |     |
           |  p1 |
           1 _ _ 2
    3 _ _ 2       1 _ _ 0
          |       |
       p2 |       |  p3
    0 _ _ 1       2 _ _ 3
    */

    auto& [p1, p2, p3] = hole.patches;
    auto& [c1, c2, c3] = hole.corners;
    
    std::array<std::array<std::pair<int, int>, 4>, 3> ordered;
    
    ordered[0][1] = cornerIndices[c1];
    if (p1.corners[(c1 + 1) % 4] == p3.corners[c3])
    {
        ordered[0][0] = cornerIndices[(c1 + 3) % 4];
        ordered[0][2] = cornerIndices[(c1 + 1) % 4];
        ordered[0][3] = cornerIndices[(c1 + 2) % 4];
    }
    else
    {
        ordered[0][0] = cornerIndices[(c1 + 1) % 4];
        ordered[0][2] = cornerIndices[(c1 + 3) % 4];
        ordered[0][3] = cornerIndices[(c1 + 2) % 4];
    }

    ordered[1][2] = cornerIndices[c2];
    if (p2.corners[(c2 + 1) % 4] == p3.corners[(c3 + 1) % 4] ||
        p2.corners[(c2 + 1) % 4] == p3.corners[(c3 + 3) % 4])
    {
        ordered[1][3] = cornerIndices[(c2 + 3) % 4];
        ordered[1][1] = cornerIndices[(c2 + 1) % 4];
        ordered[1][0] = cornerIndices[(c2 + 2) % 4];
    }
    else
    {
        ordered[1][3] = cornerIndices[(c2 + 1) % 4];
        ordered[1][1] = cornerIndices[(c2 + 3) % 4];
        ordered[1][0] = cornerIndices[(c2 + 2) % 4];
    }

    ordered[2][1] = cornerIndices[c3];
    if (p3.corners[(c3 + 1) % 4] == p2.corners[(c2 + 1) % 4] ||
        p3.corners[(c3 + 1) % 4] == p2.corners[(c2 + 3) % 4])
    {
        ordered[2][0] = cornerIndices[(c3 + 3) % 4];
        ordered[2][2] = cornerIndices[(c3 + 1) % 4];
        ordered[2][3] = cornerIndices[(c3 + 2) % 4];
    }
    else
    {
        ordered[2][0] = cornerIndices[(c3 + 1) % 4];
        ordered[2][2] = cornerIndices[(c3 + 3) % 4];
        ordered[2][3] = cornerIndices[(c3 + 2) % 4];
    }

    return ordered;
}

glm::fvec3 GregoryPatch::edgeDeCasteljeu(float t, Patch& p, std::pair<int, int> c1, std::pair<int, int> c2)
{
   std::array <glm::fvec3, 4 > edge;
    for (int j = 0; j < 4; j++)
    {
        float w2 = j / 3.0f;
        float w1 = 1 - w2;

        edge[j] = p.points[(int)roundf(c1.first * w1 + c2.first * w2)][(int)roundf(c1.second * w1 + c2.second * w2)]
            ->getTransform().location;
    }
    return BezierCurve::deCastljeu(t, edge);
}


glm::fvec3 GregoryPatch::pointOnEdge(int i, Patch& p, std::pair<int, int> c1, std::pair<int, int> c2)
{
    float w2 = i / 3.0f;
    float w1 = 1 - w2;
    int x = (int)roundf(c1.first * w1 + c2.first * w2);
    int y = (int)roundf(c1.second * w1 + c2.second * w2);
    return p.points[x][y]->getTransform().location;
}

void GregoryPatch::updateMeshes()
{
    std::array<Patch, 3>&  p = hole.patches;
    auto corners = orderCorners();

    std::array<glm::fvec3, 3> middles;
    std::array<std::array<glm::fvec3, 4>, 3> secondRows;
    std::array<glm::fvec3, 3> middlesSecondRow;
    std::array<glm::fvec3, 3> p1;
    std::array<glm::fvec3, 3> p2;
    std::array<glm::fvec3, 3> q;

    // per Bernstain patch
    for (int i = 0; i < 3; i++)
    {
        auto& [i0, i1, i2, i3] = corners[i];
        middles[i] = edgeDeCasteljeu(0.5f, p[i], i1, i2);

        for (int j = 0; j < 4; j++)
        {
            float w0 = (1 - j / 3.0f) * 1.0f / 3.0f;
            float w1 = (1 - j / 3.0f) * 2.0f / 3.0f;
            float w2 = j / 3.0f * 2.0f / 3.0f;
            float w3 = j / 3.0f * 1.0f / 3.0f;

            int x = (int)round(w0 * i0.first + w1 * i1.first + w2 * i2.first + w3 * i3.first);
            int y = (int)round(w0 * i0.second + w1 * i1.second + w2 * i2.second + w3 * i3.second);

            secondRows[i][j] = p[i].points[x][y]->getTransform().location;
        }
        middlesSecondRow[i] = BezierCurve::deCastljeu(0.5f, secondRows[i]);
        glm::fvec3 middleDer = 3.0f * (middles[i] - middlesSecondRow[i]);
        p2[i] = middles[i] + middleDer / 3.0f;
        q[i] = (3.0f * p2[i] - middles[i]) / 2.0f;
    }

    glm::fvec3 center = (q[0] + q[1] + q[2]) / 3.0f;
    for (int i = 0; i < 3; i++)
        p1[i] = (2.0f * q[i] + center) / 3.0f;

    // per Gregory patch
    for (int i = 0; i < 3; i++)
    {
        int j = (i + 1) % 3;
        auto& [i0, i1, i2, i3] = corners[i];
        auto& [j0, j1, j2, j3] = corners[j];

        subPatches[i][0] = pointOnEdge(0, p[i], i1, i2);
        subPatches[i][3] = middles[i];
        subPatches[i][16] = middles[j];
        subPatches[i][19] = center;

        subPatches[i][1] = (pointOnEdge(1, p[i], i1, i2) + pointOnEdge(0, p[i], i1, i2)) / 2.0f;
        subPatches[i][2] = middles[i] - derivative(0.5f, p[i], i1, i2) / 6.0f;

        subPatches[i][4] = (pointOnEdge(1, p[j], j2, j1) + pointOnEdge(0, p[j], j2, j1)) / 2.0f;
        subPatches[i][10] = middles[j] + derivative(0.5f, p[j], j1, j2) / 6.0f;

        subPatches[i][9] = p2[i];
        subPatches[i][15] = p1[i];

        subPatches[i][17] = p2[j];
        subPatches[i][18] = p1[j];

        subPatches[i][5] = subPatches[i][1] + subPatches[i][1] - (secondRows[i][0] + derivative(0, p[i], i1, i2) / 6.0f);
        subPatches[i][7] = subPatches[i][2] + subPatches[i][2] - (middlesSecondRow[i] - derivative(0.5, p[i], i1, i2) / 6.0f);

        subPatches[i][6] = subPatches[i][4] + subPatches[i][4] - (secondRows[j][3] - derivative(1, p[j], j1, j2) / 6.0f); 
        subPatches[i][12] = subPatches[i][10] + subPatches[i][10] - (middlesSecondRow[j] + derivative(0.5, p[j], j1, j2) / 6.0f); 

        subPatches[i][13] = subPatches[i][18] + ((subPatches[i][10] - subPatches[i][16]) + 2.0f * (subPatches[i][15] - subPatches[i][19])) / 3.0f;
        subPatches[i][14] = subPatches[i][15] + ((subPatches[i][2] - subPatches[i][3]) + 2.0f * (subPatches[i][18] - subPatches[i][19])) / 3.0f;

        subPatches[i][11] = subPatches[i][12];
        subPatches[i][8] = subPatches[i][7];
    }


    std::vector<float> vert;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 20; j++)
            vert.insert(vert.end(), {
                subPatches[i][j].x, subPatches[i][j].y, subPatches[i][j].z
            });

    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), &vert[0], GL_STATIC_DRAW);
}

void GregoryPatch::createBuffers()
{
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);
    glGenVertexArrays(1, &chainVAO);
    glGenBuffers(1, &chainEBO);

    std::vector<int> inds;
    for (int i = 0; i < 60; i++)
        inds.push_back(i);

    std::vector<int> chainInds;
    for (int i = 0; i < 3; i++)
    {
        int offset = 20 * i;
        chainInds.insert(chainInds.end(), {
            offset + 0, offset + 1,
            offset + 1, offset + 2,
            offset + 2, offset + 3,

            offset + 0, offset + 4,
            offset + 4, offset + 10,
            offset + 10, offset + 16,

            offset + 16, offset + 17,
            offset + 17, offset + 18,
            offset + 18, offset + 19,

            offset + 3, offset + 9,
            offset + 9, offset + 15,
            offset + 15, offset + 19,

            offset + 1, offset + 5,
            offset + 2, offset + 7,

            offset + 4, offset + 6,
            offset + 10, offset + 12,

            offset + 9, offset + 8,
            offset + 15, offset + 14,
            offset + 17, offset + 11,
            offset + 18, offset + 13,

            });
    }

    chainIndsSize = chainInds.size();
    meshIndsSize = inds.size();

    glBindVertexArray(meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(int), &inds[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(chainVAO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, chainInds.size() * sizeof(int), &chainInds[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GregoryPatch::GregoryPatch(Hole hole)
    : hole(hole), name("Gregory-" + std::to_string(indexer.getNewIndex())),
    chainShader("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag"),
    meshShader("Shaders/Surface/surfaceC0.vert", "Shaders/fragmentShader.frag"),
    shouldReload(false), showChain(false)
{
    division[0] = 4;
    division[1] = 4;

    chainShader.Init();
    meshShader.Init();
    meshShader.loadShaderFile("Shaders/Surface/gregory.tesc", GL_TESS_CONTROL_SHADER);
    meshShader.loadShaderFile("Shaders/Surface/gregory.tese", GL_TESS_EVALUATION_SHADER);

    createBuffers();
    updateMeshes();
    
}

std::string GregoryPatch::getName() const
{
    return name;
}

void GregoryPatch::SelectAll(Scene& scene) const
{
}

bool GregoryPatch::CanChildBeDeleted() const
{
    return false;
}

bool GregoryPatch::RenderGui()
{
    ImGui::Begin("Gregory");
    if (ImGui::InputInt2("Division", division))
    {
        division[0] = glm::clamp(division[0], 2, 1000);
        division[1] = glm::clamp(division[1], 2, 1000);
        shouldReload = true;
    }
    ImGui::Checkbox("Show Chain", &showChain);
    ImGui::End();
    return false;
}

void GregoryPatch::Render(bool selected, VariableManager& vm)
{
    if (shouldReload)
    {
        updateMeshes();
        shouldReload = false;
    }

    meshShader.use();
    
    glBindVertexArray(meshVAO);
    glPatchParameteri(GL_PATCH_VERTICES, 20);

    vm.SetVariable("divisionU", division[0]);
    vm.SetVariable("divisionV", division[1]);

    vm.SetVariable("reverse", false);
    vm.Apply(meshShader.ID);
    glDrawElements(GL_PATCHES, meshIndsSize, GL_UNSIGNED_INT, 0);

    vm.SetVariable("reverse", true);
    vm.Apply(meshShader.ID);
    glDrawElements(GL_PATCHES, meshIndsSize, GL_UNSIGNED_INT, 0);

    if (showChain)
    {
        chainShader.use();
        vm.SetVariable("color", glm::fvec4(1.0f, 0.0f, 1.0f, 1.0f));
        vm.Apply(chainShader.ID);

        glLineWidth(2);
        glBindVertexArray(chainVAO);
        glDrawElements(GL_LINES, chainIndsSize, GL_UNSIGNED_INT, 0);
        glLineWidth(1);
    }
}

void GregoryPatch::onAdd(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void GregoryPatch::onRemove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
    if (elem.get() == this)
        for (auto& p : hole.patches)
        {
            auto surface = std::dynamic_pointer_cast<SurfaceC0>(p.surface);
            if (!surface)
                continue;

            std::erase_if(surface->po, [this](const std::weak_ptr<IOwner>& o) {
                return o.lock().get() == this;
                });
        }
}

void GregoryPatch::onSelect(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
}

void GregoryPatch::onMove(Scene& scene, std::shared_ptr<ISceneElement> elem)
{
    for (auto& p : hole.patches)
        for (auto& row : p.points)
            for (auto& point : row)
                if (point == elem)
                    shouldReload = true;
}

void GregoryPatch::onCollapse(Scene& scene, std::vector<std::shared_ptr<Point>>& collapsed, std::shared_ptr<Point> result)
{
    for(auto point : collapsed)
        for(auto& patch : hole.patches)
        {
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (patch.points[i][j] == point)
                    {
                        shouldReload = true;
                        patch.points[i][j] = result;
                    }
        }
}
