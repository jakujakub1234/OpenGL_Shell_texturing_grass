// code inspired by https://www.youtube.com/watch?v=yNYwZMmgTJk&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=15&ab_channel=ThinMatrix

#include "../../include/engine_core/Terrain.h"
#include "../../include/engine_core/HeightsGenerator.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

Terrain::Terrain(int grid_x, int grid_z)
{
    this->x = grid_x;
    this->z = grid_z;
}

float Terrain::GetHeightOfTerrain(float world_x, float world_z)
{
    float terrain_x = world_x - this->x;
    float terrain_z = world_z - this->z;

    float grid_square_size = SIZE / ((float)VERTEX_COUNT - 1.0f);

    int grid_x = floor(terrain_x / grid_square_size);
    int grid_z = floor(terrain_z / grid_square_size);

    if (grid_x > VERTEX_COUNT -1 || grid_z > VERTEX_COUNT -1 || grid_x < 0 || grid_z < 0) {
        return 0.0f;
    }

    float x_coord = fmod(terrain_x, grid_square_size) / grid_square_size;
    float z_coord = fmod(terrain_z, grid_square_size) / grid_square_size;

    float result;

    if (x_coord < 1.0f - z_coord) {
        result = BarryCentric(glm::vec3(0, heights[grid_x][grid_z], 0), glm::vec3(1,
							heights[grid_x + 1][grid_z], 0), glm::vec3(0,
							heights[grid_x][grid_z + 1], 1), glm::vec2(x_coord, z_coord));
    } else {
        result = BarryCentric(glm::vec3(1, heights[grid_x + 1][grid_z], 0), glm::vec3(1,
							heights[grid_x + 1][grid_z + 1], 1), glm::vec3(0,
							heights[grid_x][grid_z + 1], 1), glm::vec2(x_coord, z_coord));
    }

    return result;
}

void Terrain::GenerateAndBindTerrain(unsigned int &terrain_VAO, unsigned int &terrain_VBO, unsigned int &terrain_EBO)
{
    int count = VERTEX_COUNT * VERTEX_COUNT;
    float vertices[count * 3];
    float normals[count * 3];
    float textureCoords[count*2];
    int indices[6*(VERTEX_COUNT-1)*(VERTEX_COUNT-1)];

    int vertexPointer = 0;
    
    for (int i = 0; i < VERTEX_COUNT; i++) {
        for (int j = 0; j < VERTEX_COUNT; j++) {
            float height = GetHeight(j, i);
            heights[j][i] = height;

            vertices[vertexPointer*3] = (float)j/((float)VERTEX_COUNT - 1) * SIZE;
            vertices[vertexPointer*3+1] = height;
            vertices[vertexPointer*3+2] = (float)i/((float)VERTEX_COUNT - 1) * SIZE;

            glm::vec3 normal = CalculateNormal(j, i);
            normals[vertexPointer*3] = normal.x;
            normals[vertexPointer*3+1] = normal.y;
            normals[vertexPointer*3+2] = normal.z;
            
            textureCoords[vertexPointer*2] = (float)j/((float)VERTEX_COUNT - 1);
            textureCoords[vertexPointer*2+1] = (float)i/((float)VERTEX_COUNT - 1);
            vertexPointer++;
        }
    }

    int pointer = 0;
    for (int gz = 0; gz < VERTEX_COUNT - 1; gz++) {
        for (int gx = 0; gx < VERTEX_COUNT - 1; gx++) {
            int topLeft = (gz*VERTEX_COUNT)+gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz+1)*VERTEX_COUNT)+gx;
            int bottomRight = bottomLeft + 1;
            indices[pointer++] = topLeft;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = topRight;
            indices[pointer++] = topRight;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = bottomRight;
        }
    }

    float terrain_data[8*count];
    int terrain_data_index = 0;

    for (int i = 0; i < count; ++i) {
        for (int j = i*3; j < i*3+3; ++j) {
            terrain_data[terrain_data_index++] = vertices[j];
        }

        for (int j = i*3; j < i*3+3; ++j) {
            terrain_data[terrain_data_index++] = normals[j];
        }

        for (int j = i*2; j < i*2+2; ++j) {
            terrain_data[terrain_data_index++] = textureCoords[j];
        }
    }

    glGenVertexArrays(1, &terrain_VAO);
    glGenBuffers(1, &terrain_VBO);
    glGenBuffers(1, &terrain_EBO);

    glBindVertexArray(terrain_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, terrain_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(terrain_data), terrain_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float Terrain::GetHeight(int x, int z) {
    return heights_generator.GenerateHeight(x, z);
}

// quasi Perlin noise
glm::vec3 Terrain::CalculateNormal(int x, int z) {
    float height_l = GetHeight(x-1, z);
    float height_r = GetHeight(x+1, z);
    float height_d = GetHeight(x, z-1);
    float height_u = GetHeight(x, z+1);

    glm::vec3 normal = glm::vec3(height_l - height_r, 2.0f, height_d - height_u);
    normal = glm::normalize(normal);

    return normal;
}

float Terrain::BarryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
{
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3 = 1.0f - l1 - l2;

    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}
