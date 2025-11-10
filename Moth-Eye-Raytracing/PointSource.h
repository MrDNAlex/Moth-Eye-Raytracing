#pragma once
#include "Vec2.h"
#include "RaySource.h"
#include "Ray.h"
class PointSource : public RaySource
{
public:

	Vec2 Origin;

	PointSource(double x, double y, int numberOfRays) : RaySource(numberOfRays), Origin(x, y)
	{
	}

	std::vector<Ray> GenerateRays() override
	{
		std::vector<Ray> rays = std::vector<Ray>();
		rays.reserve(this->NumberOfRays);

		double pi2 = 2 * 3.14159265358979323846;

		for (int i = 0; i < this->NumberOfRays; i++)
		{
			double angle = pi2*(double(i) / double(this->NumberOfRays));
			double dx = cos(angle);
			double dy = sin(angle);

			rays.push_back(Ray(this->Origin.X, this->Origin.Y, dx, dy));
		}

		return rays;
	}
};