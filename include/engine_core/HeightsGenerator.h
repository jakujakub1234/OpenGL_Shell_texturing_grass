// code inspired by https://www.youtube.com/watch?v=yNYwZMmgTJk&list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&index=15&ab_channel=ThinMatrix

#pragma once

class HeightsGenerator
{
public:
    HeightsGenerator();
    float GenerateHeight(int x, int z);

private:
    const float AMPLITUDE = 110.0f;
    
    int seed;

    float Interpolate(float a, float b, float blend);
    float GetInterpolatedNoise(float x, float z);
    float GetSmoothNoise(int x, int z);
    float GetNoise(int x, int z);
};
