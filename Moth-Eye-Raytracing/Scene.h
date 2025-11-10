#pragma once
#include <vector>
#include "Ray.h"
#include "Object.h"
#include "Segment.h"
#include <iostream>
#include "Frame.h"
#include <fstream>
#include "RaySource.h"
class Scene
{
public:

	std::vector<Object*> Objects;

	std::vector<Ray> Rays;

	std::vector<Frame> Frames;

	std::vector<RaySource*> RaySources;

	std::string FileName;

	Scene(std::string fileName)
	{
		this->Objects = std::vector<Object*>();
		this->Rays = std::vector<Ray>();
		this->Frames = std::vector<Frame>();
		this->FileName = fileName;
	}

	void AddObject(Object* object)
	{
		this->Objects.push_back(object);
	}

	void AddRay(Ray ray)
	{
		this->Rays.push_back(ray);
	}

	void AddRays(std::vector<Ray> rays)
	{
		this->Rays.reserve(this->Rays.size() + rays.size()); // capacity only
		this->Rays.insert(this->Rays.end(), rays.begin(), rays.end());
	}

	void AddFrame(Frame frame)
	{
		this->Frames.push_back(frame);
	}

	void Render()
	{
		std::cout << "Rendering Started" << std::endl;

		int index = 0;

		while (this->Rays.size() > 0)
		{
			Frame frame = Frame(index);

			for (Ray& ray : this->Rays)
				frame.AddRay(ray);

			std::vector<Ray> newRays = std::vector<Ray>();

			newRays.reserve(this->Rays.size() * 2); // Estimate

			for (Ray& ray : this->Rays)
			{
				std::vector<Ray> traveledRays = this->Travel(&ray, &frame);

				if (traveledRays.size() > 0)
					newRays.insert(newRays.end(), traveledRays.begin(), traveledRays.end());
			}

			std::cout << "Rendered Frame " << index << ": " << this->Rays.size() << " Rays, " << frame.DestroyedRays << " Destroyed, " << frame.LostRays << " Lost" << std::endl;

			AddFrame(frame);
			this->Rays = newRays;
			index++;
		}

		Frame frame = Frame(index);

		AddFrame(frame);

		std::cout << "Rendering Complete" << std::endl;

		json j;

		j["Geometry"] = json::array();

		for (Object* object : this->Objects)
			j["Geometry"].push_back(object->ToJSON());

		j["Frames"] = json::array();
		for (Frame& frame : this->Frames)
			j["Frames"].push_back(frame.ToJSON());

		//Save the File 
		std::ofstream file(FileName + ".json");
		file << j.dump(2);  // Pretty Indent of 4 Spaces
		file.close();

		std::cout << "Render Saved" << std::endl;
	}

	std::vector<Ray> Travel(Ray* ray, Frame* frame)
	{
		ray->Bounce();

		if (ray->DestroyRay())
		{
			frame->DestroyedRays += 1;
			return std::vector<Ray>();
		}
			
		double minT = INFINITY;
		Segment* closestSegment = nullptr;
		Object* closestObject = nullptr;

		for (Object* object : this->Objects)
		{
			RayHit hit = object->Intersect(ray);

			if (hit.Hit && hit.Distance < minT)
			{
				minT = hit.Distance;
				closestSegment = hit.SegmentHit;
				closestObject = object;
			}
		}

		if (closestSegment == nullptr || closestObject == nullptr)
		{
			frame->LostRays += 1;
			return std::vector<Ray>();
		}
			
		return closestObject->InteractWithRay(closestSegment, ray);
	}
};