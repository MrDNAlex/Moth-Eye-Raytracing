
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

std::vector<double> linspace(double start, double end, int num) {
	std::vector<double> result;
	if (num <= 0) return result;
	if (num == 1) {
		result.push_back(start);
		return result;
	}

	double step = (end - start) / (num - 1);
	for (int i = 0; i < num; ++i)
		result.push_back(start + step * i);

	return result;
}

void ConeWaveUnitCell(int QDs)
{
	std::string name = "Cone" + std::to_string(QDs) + "WaveUnitCell";

	Scene scene = Scene(name);

	//Constants
	double qdRadius = 10.0;
	double mothEyeHeight = 250.0;
	double pi = 3.14159265358979323846;

	double A = mothEyeHeight / 2.0;
	double B = 2.0 * pi / mothEyeHeight;
	double C = mothEyeHeight / 4.0;
	double D = mothEyeHeight / 2.0;

	double startX = 0.0;
	double endX = 250.0;
	double startY = 0.0;
	double endY = 0.0;
	double depth = -200.0;

	scene.AddObject(new Wave(startX, startY, endX, endY, 1000, [](double x, double y) { return 1.1; }, A, B, C, D));
	scene.AddObject(new Mirror(startX, startY, startX, depth));
	scene.AddObject(new Mirror(endX, startY, endX, depth));
	scene.AddObject(new Target(startX, depth, endX, depth));

	std::vector<double> qdPositionsX = linspace(startX, endX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = startY;

		double bx = endX;
		double by = endY;

		scene.AddRaySource(new ConeLight(ox, oy, ax, ay, bx, by, 10));
	}

	std::cout << "Render" << std::endl;
	scene.Render();
}


void QDInternalWaveUnitCell(int QDs)
{
	std::string name = "QDInternal" + std::to_string(QDs) + "WaveUnitCell";

	Scene scene = Scene(name);
	
	//Constants
	double qdRadius = 10.0;
	double mothEyeHeight = 250.0;
	double pi = 3.14159265358979323846;

	double A = mothEyeHeight / 2.0;
	double B = 2.0 * pi / mothEyeHeight;
	double C = mothEyeHeight / 4.0;
	double D = mothEyeHeight / 2.0;

	double startX = 0.0;
	double endX = 250.0;
	double startY = 0.0;
	double endY = 0.0;
	double depth = -200.0;

	scene.AddObject(new Wave(startX, startY, endX, endY, 1000, [](double x, double y) { return 1.1; }, A, B, C, D));
	scene.AddObject(new Mirror(startX, startY, startX, depth));
	scene.AddObject(new Mirror(endX, startY, endX, depth));
	scene.AddObject(new Target(startX, depth, endX, depth));

	std::vector<double> qdPositionsX = linspace(startX, endX, QDs+2);

	for (int i = 1; i < qdPositionsX.size()-1; i++)
	{
		scene.AddObject(new QuantumDot(qdPositionsX[i], -100, qdRadius, 1000));
		scene.AddRaySource(new PointSource(qdPositionsX[i], -100, 1000));
	}

	std::cout << "Render" << std::endl;
	scene.Render();
}


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
	//CaptureTest();
	QDInternalWaveUnitCell(1);
	QDInternalWaveUnitCell(2);
	QDInternalWaveUnitCell(3);
	QDInternalWaveUnitCell(4);

	ConeWaveUnitCell(1);
	ConeWaveUnitCell(2);
	ConeWaveUnitCell(3);
	ConeWaveUnitCell(4);

	std::cout << "Press ENTER to exit...";
	std::cin.get();
}
