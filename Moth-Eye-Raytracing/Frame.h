#pragma once
#include "Ray.h"
#include <vector>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class Frame
{
public:

	std::vector<Ray> Rays;

	int FrameNumber;

	int LostRays;

	int DestroyedRays;

	Frame(int frameNumber) : FrameNumber(frameNumber)
	{
		Rays = std::vector<Ray>();
		LostRays = 0;
		DestroyedRays = 0;
	}

	void AddRay(Ray ray)
	{
		this->Rays.push_back(ray);
	}

	json ToJSON()
	{
		json j;
		j["FrameNumber"] = this->FrameNumber;
		j["RayCount"] = this->Rays.size();
		j["LostRays"] = this->LostRays;
		j["DestroyedRays"] = this->DestroyedRays;
		j["Rays"] = json::array();

		for (Ray& ray : this->Rays)
			j["Rays"].push_back(ray.ToJSON());

		return j;
	}
};