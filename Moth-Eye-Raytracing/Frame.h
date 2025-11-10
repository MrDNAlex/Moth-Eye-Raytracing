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

	Frame(int frameNumber) : FrameNumber(frameNumber)
	{
		Rays = std::vector<Ray>();
	}

	void AddRay(Ray ray)
	{
		this->Rays.push_back(ray);
	}

	json ToJSON()
	{
		json j;
		j["frame_number"] = this->FrameNumber;
		j["ray_count"] = this->Rays.size();
		j["rays"] = json::array();

		for (Ray& ray : this->Rays)
			j["rays"].push_back(ray.ToJSON());

		return j;
	}
};