// code inspired by https://www.youtube.com/watch?v=yNYwZMmgTJk&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=15&ab_channel=ThinMatrix

#include "../../include/engine_core/HeightsGenerator.h"
#include <cstdlib>
#include <time.h>
#include <cmath>

HeightsGenerator::HeightsGenerator()
{
    srand((unsigned) time(NULL));

    seed = rand() % 100000;
}

float HeightsGenerator::GenerateHeight(int x, int z)
{
    float result = GetInterpolatedNoise(x / 4.0f, z / 4.0f) * AMPLITUDE;
    result += GetInterpolatedNoise(x / 2.0f, z / 2.0f) * AMPLITUDE / 3.0f;
    result += GetInterpolatedNoise(x, z) * AMPLITUDE / 9.0f;

    return result;
}

float HeightsGenerator::Interpolate(float a, float b, float blend)
{
    double theta = blend * M_PI;
    float f = (float)(1.0f - cos(theta)) * 0.5f;

    return a * (1.0f - f) + b * f;
}

float HeightsGenerator::GetInterpolatedNoise(float x, float z)
{
    int int_x = (int) x;
    int int_z = (int) z;

    float fract_x = x - int_x;
    float fract_z = z - int_z;

    float v1 = GetSmoothNoise(int_x, int_z);
    float v2 = GetSmoothNoise(int_x + 1, int_z);
    float v3 = GetSmoothNoise(int_x, int_z + 1);
    float v4 = GetSmoothNoise(int_x + 1, int_z + 1);

    float i1 = Interpolate(v1, v2, fract_x);
    float i2 = Interpolate(v3, v4, fract_x);

    return Interpolate(i1, i2, fract_z);
}

float HeightsGenerator::GetSmoothNoise(int x, int z)
{
    float corners = (GetNoise(x-1, z-1) + GetNoise(x-1, z+1) + GetNoise(x+1, z-1) + GetNoise(x+1, z+1)) / 16.0f;
    float sides = (GetNoise(x-1, z) + GetNoise(x+1, z) + GetNoise(x, z-1) + GetNoise(x, z+1)) / 8.0f;
    float center = GetNoise(x, z) / 4.0f;

    return corners + sides + center;
}

float HeightsGenerator::GetNoise(int x, int z)
{
    srand(x*52345 + z*13054 + seed);
    
    return ((float)(rand()) / (float)(RAND_MAX)) * 2.0f - 1.0f;
}
