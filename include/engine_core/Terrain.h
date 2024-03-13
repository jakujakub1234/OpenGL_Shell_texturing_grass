// code inspired by https://www.youtube.com/watch?v=yNYwZMmgTJk&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=15&ab_channel=ThinMatrix

#pragma once

#include "../../include/engine_core/HeightsGenerator.h"

#include <glm/glm.hpp>

class Terrain
{
public:
    Terrain(int grid_x, int grid_z);
    
    void GenerateAndBindTerrain(unsigned int &terrain_VAO, unsigned int &terrain_VBO, unsigned int &terrain_EBO);
    float GetHeightOfTerrain(float world_x, float world_z);

private:
    const float SIZE = 800.0f;
    const int VERTEX_COUNT = 64;

    HeightsGenerator heights_generator;

    float x;
    float z;

    float heights[64][64];

    float GetHeight(int x, int z);
    glm::vec3 CalculateNormal(int x, int z);
    float BarryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
};
