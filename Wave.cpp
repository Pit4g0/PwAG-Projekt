#include "Wave.h"
#include <cmath>
#include <GL/glut.h>
#include <GL/glext.h>


Wave::Wave() {
	vertices.clear();

	for (int z = 0; z <= SIZE; z++)
	{
		for (int x = 0; x <= SIZE; x++)
		{
			float posX = x - SIZE / 2.0f;
			float posZ = z - SIZE / 2.0f;

			vertices.push_back(posX);
			vertices.push_back(0.0f);
			vertices.push_back(posZ);
		}
	}
}

void Wave::Draw()
{
	for (int z = 0; z < SIZE; z++)
	{
		glBegin(GL_TRIANGLE_STRIP);

		for (int x = 0; x <= SIZE; x++)
		{
			int index1 = (z * (SIZE + 1) + x) * 3;
			int index2 = ((z + 1) * (SIZE + 1) + x) * 3;

			glNormal3f(0.0f, 1.0f, 0.0f);

			glVertex3f(
				vertices[index1],
				vertices[index1 + 1],
				vertices[index1 + 2]
			);

			glVertex3f(
				vertices[index2],
				vertices[index2 + 1],
				vertices[index2 + 2]
			);
		}

		glEnd();
	}
}

float Wave::GetHeight(float x, float z, float totalTime)
{
	float timeSpeed = totalTime * SPEED;
	float alttimeSpeed = totalTime * SECONDARY_SPEED;

	float firseSinewave = amplitude * sinf(frequencyX * x + timeSpeed) * cosf(frequencyZ * z + timeSpeed);
	float secondSinewave = secondary_amplitude * sinf(secondary_frequencyX * x + alttimeSpeed) * cosf(secondary_frequencyZ * z + alttimeSpeed);

	return firseSinewave + secondSinewave;
}

void Wave::Update(float totalTime) {
	for (int z = 0; z <= SIZE; z++)
	{
		for (int x = 0; x <= SIZE; x++)
		{
			int index = (z * (SIZE + 1) + x) * 3;

			float posX = vertices[index];
			float posZ = vertices[index + 2];

			vertices[index + 1] = GetHeight(posX, posZ, totalTime);
		}
	}
}