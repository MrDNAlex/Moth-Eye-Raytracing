
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

double MothEyeRefractiveIndex(double height)
{
	double A = 1.41225607;
	double B = 0.00122535861;
	double C = 0.0000549792342;
	double D = 0.000000982578598;
	double E = 0.00000000532593744;
	double F = 0.00000000000920959868;

	double h = height;
	double h2 = h * h;
	double h3 = h2 * h;
	double h4 = h3 * h;
	double h5 = h4 * h;

	return A - B * h + C * h2 - D * h3 + E * h4 - F * h5;
}

Scene CreateWaveguideBlock(std::string name, int waveguideLayers, double startX = -10000.0, double endX = 10000.0, bool useMothEyeIndex = false)
{
	Scene scene = Scene(name);

	double qdRadius = 10.0;
	double mothEyeHeight = 250.0;
	double pi = 3.14159265358979323846;

	double waveguideTopLeftX = startX;
	double waveguideTopLeftY = 250.0;
	double waveguideTopRightX = endX;
	double waveguideTopRightY = 250.0;
	double waveguideBottomLeftX = startX;
	double waveguideBottomLeftY = 0.0;
	double waveguideBottomRightX = endX;
	double waveguideBottomRightY = 0.0;

	double targetY = -200.0;

	scene.AddObject(new Mirror(waveguideTopLeftX, waveguideTopLeftY, waveguideBottomLeftX, targetY));
	scene.AddObject(new Mirror(waveguideTopRightX, waveguideTopRightY, waveguideBottomRightX, targetY));
	scene.AddObject(new Target(waveguideBottomLeftX, targetY, waveguideBottomRightX, targetY));

	std::vector<double> waveguideRefractiveIndex = linspace(1.0, 1.41, waveguideLayers);
	std::vector<double> waveguidePosition = linspace(waveguideTopLeftY, waveguideBottomLeftY, waveguideLayers);

	for (int i = 0; i < waveguideLayers; i++)
	{
		double wy = waveguidePosition[i];
		double n = waveguideRefractiveIndex[i];

		if (useMothEyeIndex)
			n = MothEyeRefractiveIndex(mothEyeHeight - wy);

		Object* obj = new Object();

		obj->AddSegment(startX, wy, endX, wy, n);

		scene.AddObject(obj);
	}

	return scene;
}

//
// Moth Eye Wave Section
//
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

	scene.AddObject(new Wave(startX, startY, endX, endY, 1000, [](double x, double y) { return 1.0; }, A, B, C, D));
	scene.AddObject(new Mirror(startX, startY, startX, depth));
	scene.AddObject(new Mirror(endX, startY, endX, depth));
	scene.AddObject(new Target(startX, depth, endX, depth));

	std::vector<double> qdPositionsX = linspace(startX, endX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		scene.AddObject(new QuantumDot(qdPositionsX[i], -100, qdRadius, 1000));
		scene.AddRaySource(new PointSource(qdPositionsX[i], -100, 1000, 1.41));
	}

	std::cout << "Render" << std::endl;
	scene.Render();
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

	scene.AddObject(new Wave(startX, startY, endX, endY, 1000, [](double x, double y) { return 1.0; }, A, B, C, D));
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

		scene.AddRaySource(new ConeLight(ox, oy, ax, ay, bx, by, 10, 1.41));
	}

	std::cout << "Render" << std::endl;
	scene.Render();
}

//
// Moth Eye Waveguide Section
//

void MaxCaptureAngleWaveguide(int waveguideLayers, int numberOfAngles, double endAngle, bool useMothEyeIndex = false)
{
	double startX = -10000.0;
	double endX = 10000.0;
	double emitterLength = 1000;

	double startAngle = 0.0;

	std::vector<double> angles = linspace(startAngle, endAngle, numberOfAngles);

	json js;

	js["Angle"] = json::array();
	js["Power"] = json::array();

	for (int i = 0; i < angles.size(); i++)
	{
		double pi = 3.14159265358979323846;
		double angle = angles[i];
		double radians = angle * pi / 180.0;

		double xStart = -(cos(radians) * emitterLength) + 9900;
		double yStart = sin(radians) * emitterLength + 300;

		std::string name = "MaxCaptureAngleWaveguide" + std::to_string(waveguideLayers) + "Angle" + std::to_string(angle).substr(6);

		Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

		scene.AddObject(new Mirror(startX, 1400.0, startX, 0));
		scene.AddObject(new Mirror(endX, 1400.0, endX, 0));

		scene.AddRaySource(new DirectionalLight(xStart, yStart, 9900, 300, 1));

		scene.Render(false, false);

		for (int j = 0; j < scene.Objects.size(); j++)
		{
			if (scene.Objects[j]->Type == "Target")
			{
				Target* target = static_cast<Target*>(scene.Objects[j]);

				std::cout << "Angle : " << angle << " Captured Power : " << target->CapturedPower << std::endl;

				js["Power"].push_back(target->CapturedPower / (double)scene.Stats.StartRays);
				js["Angle"].push_back(angle);
			}
		}
	}

	std::string name = "MaxCaptureAngleWaveguide" + std::to_string(waveguideLayers);

	if (useMothEyeIndex)
		name += "_MothEye";

	std::ofstream file(name + ".json");
	file << js.dump(2);
	file.close();
}

void RunMaxCaptureAngleWaveguide()
{
	std::vector<double> waveGuides = { 2, 3, 4, 5 , 6, 7, 8, 9, 10 };
	std::vector<double> waveGuides1 = linspace(10, 250, 24);

	waveGuides.insert(waveGuides.end(), waveGuides1.begin(), waveGuides1.end());

	double maxAngle = 90.0;

	for (int i = 0; i < waveGuides.size(); i++)
	{
		int layers = (int)waveGuides[i];
		MaxCaptureAngleWaveguide(layers, 1000, maxAngle, false);
		MaxCaptureAngleWaveguide(layers, 1000, maxAngle, true);
	}
}

void ConeWaveguide(int QDs, int waveguideLayers, int raysPerCone = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -10000.0;
	double endX = 10000.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;
	double qdRadius = 5.0;
	double mothEyeHeight = 250.0;

	std::string name = "Cone" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "Large";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = 0;

		double bx = endX;
		double by = 0;

		scene.AddRaySource(new ConeLight(ox, oy, ax, ay, bx, by, raysPerCone, 1.41));
	}

	std::cout << "Render" << std::endl;
	scene.Render(true, true, false);
}

void ConeWaveguideUnitCell(int QDs, int waveguideLayers, int raysPerCone = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -125.0;
	double endX = 125.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;
	double qdRadius = 5.0;
	double mothEyeHeight = 250.0;

	std::string name = "Cone" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "UnitCell";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = 0;

		double bx = endX;
		double by = 0;

		scene.AddRaySource(new ConeLight(ox, oy, ax, ay, bx, by, raysPerCone, 1.41));
	}

	std::cout << "Render" << std::endl;
	scene.Render(true, true, false);
}

void QDWaveguideUnitCell(int QDs, int waveguideLayers, int raysPerQD = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -125.0;
	double endX = 125.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;
	double qdRadius = 5.0;
	double mothEyeHeight = 250.0;
	int QDResolution = 10000;

	std::string name = "QD" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "UnitCell";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = 0;

		double bx = endX;
		double by = 0;

		scene.AddObject(new QuantumDot(ox, oy, qdRadius, QDResolution));
		scene.AddRaySource(new PointSource(ox, oy, raysPerQD, 1.41));
	}

	std::cout << "Render" << std::endl;
	scene.Render(true, true, false);
}

void QDWaveguide(int QDs, int waveguideLayers, int raysPerQD = 1000, bool useMothEyeIndex = false)
{
	//Constants
	double startX = -10000.0;
	double endX = 10000.0;
	double QDStartX = -125.0;
	double QDEndX = 125.0;
	double qdRadius = 5.0;
	double mothEyeHeight = 250.0;
	int QDResolution = 10000;

	std::string name = "QD" + std::to_string(QDs) + "Waveguide" + std::to_string(waveguideLayers) + "Large";

	if (useMothEyeIndex)
		name += "_MothEye";

	Scene scene = CreateWaveguideBlock(name, waveguideLayers, startX, endX, useMothEyeIndex);

	std::vector<double> qdPositionsX = linspace(QDStartX, QDEndX, QDs + 2);

	for (int i = 1; i < qdPositionsX.size() - 1; i++)
	{
		double ox = qdPositionsX[i];
		double oy = -100;

		double ax = startX;
		double ay = 0;

		double bx = endX;
		double by = 0;

		scene.AddObject(new QuantumDot(ox, oy, qdRadius, QDResolution));
		scene.AddRaySource(new PointSource(ox, oy, raysPerQD, 1.41));
	}

	std::cout << "Render" << std::endl;
	scene.Render(true, true, false);
}

void RunQDInternalReflection()
{
	int rays = 1000;

	std::vector<double> waveGuides = { 2, 3, 4, 5 , 6, 7, 8, 9, 10 };
	std::vector<double> waveGuides1 = linspace(10, 250, 24);

	waveGuides.insert(waveGuides.end(), waveGuides1.begin(), waveGuides1.end());

	for (int QDs = 1; QDs <= 20; QDs++)
	{
		for (int i = 0; i < waveGuides.size(); i++)
		{
			int layers = (int)waveGuides[i];
			QDWaveguide(QDs, layers, rays, false);
			QDWaveguide(QDs, layers, rays, true);
			QDWaveguideUnitCell(QDs, layers, rays, false);
			QDWaveguideUnitCell(QDs, layers, rays, true);
			ConeWaveguide(QDs, layers, rays, false);
			ConeWaveguide(QDs, layers, rays, true);
			ConeWaveguideUnitCell(QDs, layers, rays, false);
			ConeWaveguideUnitCell(QDs, layers, rays, true);
		}
	}
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

	scene.AddRaySource(new ConeLight(0.0, 0.0, -1, 0, 0, -1, 1000));

	//scene.AddRays(source.GenerateRays());
	std::cout << "Render" << std::endl;
	scene.Render();
}

int main()
{
	//CaptureTest();
	//QDInternalWaveUnitCell(1);
	//QDInternalWaveUnitCell(2);
	//QDInternalWaveUnitCell(3);
	//QDInternalWaveUnitCell(4);

	// Functions to Run

	//RunMaxCaptureAngleWaveguide()
	RunQDInternalReflection();

	std::cout << "Press ENTER to exit...";
	std::cin.get();
}
