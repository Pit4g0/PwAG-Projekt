#pragma once
#include <vector>
class Wave
{
private:
    const float amplitude = 6.0f;
    const float frequencyX = 0.08f;
    const float frequencyZ = 0.01f;

    const float secondary_amplitude = 0.4f;
    const float secondary_frequencyX = 0.25f;
    const float secondary_frequencyZ = 0.5f;

    static const int SIZE = 200;

    static constexpr float SPEED = 2.f;
    static constexpr float SECONDARY_SPEED = -3.f;

    std::vector<float> vertices;


public:
    Wave();

    float GetHeight(float x, float z);
    void Draw();
    float GetHeight(float x, float z, float totalTime);
    void Update(float totalTime);
};

