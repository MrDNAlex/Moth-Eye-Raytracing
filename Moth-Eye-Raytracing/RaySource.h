#pragma once
#include "Vec2.h"
#include "Ray.h"
class RaySource
{
public:

	int NumberOfRays;

	RaySource(int numberOfRays)
	{
		this->NumberOfRays = numberOfRays;
	}

	virtual std::vector<Ray> GenerateRays()
	{
		return std::vector<Ray>();
	}
};

