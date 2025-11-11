
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include "Object.h"
#include "Ray.h"
#include "RayHit.h"
#include "Scene.h"
#include "Mirror.h"
#include "Target.h"
#include "Wave.h"
#include <functional>
#include "PointSource.h"
#include "QuantumDot.h"
#include "DirectionalLight.h"
#include "ConeLight.h"



void CaptureTest()
{
	Scene scene = Scene("CaptureTest");

	scene.AddObject(new Mirror(-10, 10, 10, 10));
	scene.AddObject(new Mirror(-10, 10, -10, -10));
	scene.AddObject(new Mirror(10, 10, 10, -10));
	scene.AddObject(new Target(-10, -10, 10, -10));
	scene.AddObject(new Wave(-10, -5, 10, -5, 100, [](double x, double y) { return 1.1; }));
	scene.AddObject(new Wave(-10, -5.5, 10, -5.5, 100, [](double x, double y) { return 1.2; }));
	scene.AddObject(new Wave(-10, -6, 10, -6, 100, [](double x, double y) { return 1.3; }));
	scene.AddObject(new Wave(-10, -6.5, 10, -6.5, 100, [](double x, double y) { return 1.4; }));
	//scene.AddObject(new QuantumDot(-3, 3, 3, 100));
	//scene.AddObject(new QuantumDot(3, 3, 3, 100));

	//PointSource source = PointSource(0, 0, 1000);
	//DirectionalLight light = DirectionalLight(0.0, 0.0, 5.0, 5.0, 1000);

	scene.AddRaySource(new ConeLight(0.0, 0.0, -1, 0, 0, -1, 1000));

	//scene.AddRays(source.GenerateRays());
	std::cout << "Render" << std::endl;
	scene.Render();
}

int main()
{
	CaptureTest();

	std::cout << "Press ENTER to exit...";
	std::cin.get();
}
