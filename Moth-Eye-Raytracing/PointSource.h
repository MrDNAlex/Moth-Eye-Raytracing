#pragma once
#include "Ray.h"
#include "Vec2.h"
class PointSource
{
public:

	Vec2 Origin;

	int NumberOfRays;

	PointSource(double x, double y, int numberOfRays) : Origin(x, y)
	{
		this->NumberOfRays = numberOfRays;
	}

	std::vector<Ray> GenerateRays()
	{
		std::vector<Ray> rays = std::vector<Ray>();

		double pi2 = 2 * 3.14159265358979323846;

		for (int i = 0; i < this->NumberOfRays; i++)
		{
			double angle = (pi2 / this->NumberOfRays) * i;
			double dx = cos(angle);
			double dy = sin(angle);

			Ray ray(this->Origin.X, this->Origin.Y, dx, dy);
			rays.push_back(ray);
		}

		return rays;
	}
};