#pragma once
#include <functional>
#include <cmath>
#include "NormalPerturbance.h"
#include "Scene.h"
#include "Mirror.h"
#include "Utilities.h"
#include "DirectionalLight.h"
#include "ConstantWavelengthGenerator.h"

double SellmeierMicron(double wavelength)
{
	double B1 = 1.0093;
	double C1 = 0.013185;

	return sqrt(1 + (B1 * wavelength * wavelength) / (wavelength * wavelength - C1));
}

double SellmeierPDMS(double wavelength_nm)
{
	double wavelengthSqr = wavelength_nm * wavelength_nm;
	double B1 = 1.0093;
	double C1 = 13185;

	double nPDMS = sqrt(1 + (B1 * wavelengthSqr) / (wavelengthSqr - C1));

	return nPDMS;
}

double SellmeierAir(double wavelength_nm)
{
	double wavelength_um = wavelength_nm / 1000.0;

	double B1 = 0.05792105;
	double B2 = 0.00167917;
	double C1 = 238.0185;
	double C2 = 57.362;

	double inverseLambda = (1 / (wavelength_um * wavelength_um));

	return 1 + B1 / (C1 - inverseLambda) + B2 / (C2 - inverseLambda);
}

double FillFactor(double heightFraction)
{
	//Constants
	double sqrt3 = sqrt(3);
	double pi = 3.14159265358979323846;
	double radius = 1;

	//Hexagon Values
	double hexagonSideLength = radius * (2 / sqrt3);
	double hexagonArea = 1.5 * sqrt3 * hexagonSideLength * hexagonSideLength;

	//Circle Values
	double circleArea = pi * sqrt(2 * radius * heightFraction - heightFraction * heightFraction);

	return circleArea / hexagonArea;
}

std::function<double(double)> CreateEffectiveRefractiveIndexFunction(double heightFraction)
{
	double fillFactor = FillFactor(heightFraction);
	double q = 2.0 / 3.0;

	return [fillFactor, q](double wavelength)
		{
			return pow(fillFactor * pow(SellmeierPDMS(wavelength), q) + (1 - fillFactor) * pow(SellmeierAir(wavelength * 0.001), q), 1 / q);
		};
}

double GetRefractiveIndexPDMS(double x, double y, double wavelength)
{
	double heightFrac = (250.0 - y) / 250.0;

	double nAir = 1.0;
	double nPDMS = SellmeierPDMS(wavelength);

	return nAir + (nPDMS - nAir) * heightFrac;
}

Scene CreateUnitCellWaveguideBlock(std::string name, int waveguideLayers, PerturbanceGenerator* pertubance, double startX = -125, double endX = 125)
{
	Scene scene = Scene(name);

	double mothEyeHeight = 250.0;

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

	std::vector<double> waveguidePosition = linspace(waveguideTopLeftY, waveguideBottomLeftY, waveguideLayers);

	for (int i = 0; i < waveguideLayers; i++)
	{
		double wy = waveguidePosition[i];
		double heightFraction = (mothEyeHeight - wy) / mothEyeHeight;

		Object* obj = new Object();

		obj->AddSegment(startX, wy, endX, wy, CreateEffectiveRefractiveIndexFunction(heightFraction), pertubance);

		scene.AddObject(obj);
	}

	return scene;
}

Object* CreateWave(double x1, double y1, double x2, double y2, int resolution, std::function<double(double)> refractiveIndex, PerturbanceGenerator* generator, double A = 1.0, double B = 1.0, double C = 1.0, double D = 1.0)
{
	Object* obj = new Object();


	obj->Type = "Wave";
	std::vector<double> x = linspace(x1, x2, resolution);
	std::vector<double> yShift = linspace(y1, y2, resolution);

	for (int i = 0; i < resolution; i++)
	{
		double phase = B * (x[i] - C);
		yShift[i] += A * sin(phase) + D;
	}

	for (int i = 0; i < resolution - 1; i++)
	{
		obj->AddSegment(x[i], yShift[i], x[i + 1], yShift[i + 1], refractiveIndex, generator);
	}

	return obj;
}

Scene CreateUnitCellWaveWaveguideBlock(std::string name, int waveguideLayers, PerturbanceGenerator* pertubance, double startX = -125, double endX = 125)
{
	Scene scene = Scene(name);

	int waveResolution = 500;

	double mothEyeHeight = 250.0;

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

	std::vector<double> waveguidePosition = linspace(waveguideTopLeftY, waveguideBottomLeftY, waveguideLayers);

	GaussianDistribution gauss = GaussianDistribution(1.2 * (endX - startX), 0.5 * (endX - startX));

	for (int i = 0; i < waveguideLayers; i++)
	{
		double wy = waveguidePosition[i];
		double heightFraction = (mothEyeHeight - wy) / mothEyeHeight;

		Object* wave = CreateWave(startX, wy, endX, wy, waveResolution, CreateEffectiveRefractiveIndexFunction(heightFraction), pertubance, 1.0, 2.0 * 3.14159265358979323846 / gauss.GetRandomValue(), 0.0, 0.0);

		scene.AddObject(wave);
	}

	return scene;
}

std::string RunWavelengthSweep(std::string path, int numOfLayers, double wavelength, int numOfRays, int avgIndex, double angle)
{
	double startX = -125.0;
	double endX = 125.0;
	double sourceHeight = 300.0;

	std::string name = "WavelengthSweep_Wavelength_" + std::to_string(wavelength) + "AVG_" + std::to_string(avgIndex);

	Scene scene = CreateUnitCellWaveguideBlock(name, numOfLayers, new ConstantPerturbance(0), startX, endX);

	scene.AddObject(new Mirror(startX, 500.0, startX, 0));
	scene.AddObject(new Mirror(endX, 500.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, numOfRays, new ConstantWavelengthGenerator(wavelength), new ConstantPerturbance(0)));

	scene.Render(true, false, false, false, false, path);

	return path + "/" + name;
}

std::string RunWavelengthSweepWithPerturbance(std::string path, int numOfLayers, double wavelength, int numOfRays, int avgIndex, double angle, double perturbanceDeviation)
{
	double startX = -125.0;
	double endX = 125.0;
	double sourceHeight = 300.0;

	std::string name = "WavelengthSweep_Perturbance_" + std::to_string(perturbanceDeviation) + "_Wavelength_" + std::to_string(wavelength) + "_AVG_" + std::to_string(avgIndex);

	PerturbanceGenerator* pertubance = nullptr;

	if (perturbanceDeviation > 0)
		pertubance = new NormalPerturbance(0, perturbanceDeviation);
	else
		pertubance = new ConstantPerturbance(0);

	Scene scene = CreateUnitCellWaveguideBlock(name, numOfLayers, pertubance, startX, endX);

	scene.AddObject(new Mirror(startX, 500.0, startX, 0));
	scene.AddObject(new Mirror(endX, 500.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, numOfRays, new ConstantWavelengthGenerator(wavelength), new ConstantPerturbance(0)));

	scene.Render(true, false, false, false, false, path);

	return path + "/" + name;
}

std::string RunWavelengthSweepWithWaveWithPerturbance(std::string path, int numOfLayers, double wavelength, int numOfRays, int avgIndex, double angle, double perturbanceDeviation)
{
	double startX = -125.0;
	double endX = 125.0;
	double sourceHeight = 300.0;

	std::string name = "WavelengthSweep_Perturbance_" + std::to_string(perturbanceDeviation) + "_Wavelength_" + std::to_string(wavelength) + "_AVG_" + std::to_string(avgIndex);

	PerturbanceGenerator* pertubance = nullptr;

	if (perturbanceDeviation > 0)
		pertubance = new NormalPerturbance(0, perturbanceDeviation);
	else
		pertubance = new ConstantPerturbance(0);

	Scene scene = CreateUnitCellWaveWaveguideBlock(name, numOfLayers, pertubance, startX, endX);

	scene.AddObject(new Mirror(startX, 500.0, startX, 0));
	scene.AddObject(new Mirror(endX, 500.0, endX, 0));

	double pi = 3.14159265358979323846;
	double radians = angle * pi / 180.0;
	double emitterLength = (endX - startX) * 0.95;

	double xStart = -(cos(radians) * emitterLength) + endX * 0.95;
	double yStart = sin(radians) * emitterLength + sourceHeight;

	scene.AddRaySource(new DirectionalLight(xStart, yStart, endX * 0.95, sourceHeight, numOfRays, new ConstantWavelengthGenerator(wavelength), new ConstantPerturbance(0)));

	scene.Render(true, false, false, false, false, path);

	return path + "/" + name;
}

void RunSimulationCategory1()
{
	std::string filePath = "Simulations/Simulation1_WavelengthSweep";

	CreateFolder("Simulations");
	CreateFolder(filePath);

	filePath = filePath + "/";

	int numOfRays = 10;
	int maxAngle = 60;

	int angleStep = 5;
	int wavelengthStep = 1;
	int startWavelength = 300;
	int endWavelength = 325; //1000
	int startLayers = 50;
	int endLayers = 100; //250
	int layerStepSize = 50;

	int totalRuns = (((endWavelength - startWavelength) / wavelengthStep) + 1) * (((endLayers - startLayers) / layerStepSize) + 1) * 21;

	int simIndex = 0;

	json j;

	for (int a = 0; a <= maxAngle; a += angleStep)
	{
		json j_angle;

		std::cout << "Starting Angle: " << a << " degrees" << std::endl;

		std::string angleName = "Angle_" + std::to_string(a);

		std::string angleFilePath = filePath + angleName;

		CreateFolder(angleFilePath);

		for (int l = startLayers; l <= endLayers; l += layerStepSize)
		{
			json j_layers;

			std::string layerName = "Layers_" + std::to_string(l);

			std::string layerFilePath = angleFilePath + "/" + layerName;

			CreateFolder(layerFilePath);

			std::vector<std::string> filePaths = std::vector<std::string>();

			for (int y = startWavelength; y <= endWavelength; y += wavelengthStep)
			{
				auto start = std::chrono::high_resolution_clock::now();

				filePaths.push_back(RunWavelengthSweep(layerFilePath, l, y, numOfRays, 0, a));

				simIndex++;

				double percentComplete = ((double)simIndex / (double)totalRuns) * 100.0;

				auto end = std::chrono::high_resolution_clock::now();

				double timeTakenMS = std::chrono::duration<double, std::milli>(end - start).count();

				std::cout << "Completed Wavelength " << y << " : " << percentComplete << " % " << " (" << timeTakenMS << " ms)" << std::endl;
			}

			j_angle[layerName] = filePaths;
		}

		j[angleName] = j_angle;
	}

	std::ofstream file(filePath + "FilePaths.json");
	file << j.dump(2);
	file.close();
}

void RunSimulationCategory2(int angle)
{
	std::string filePath = "Simulations/Simulation2_WavelengthSweepWithPerturbance";

	CreateFolder("Simulations");
	CreateFolder(filePath);

	filePath = filePath + "/";

	int avg = 3; //10
	int numOfRays = 10; //100
	int maxPerturbanceDev = 10; //10

	int angleStep = 5;
	int wavelengthStep = 1;
	int startWavelength = 300;
	int endWavelength = 325; //1000
	int startLayers = 50;
	int endLayers = 100; //250
	int layerStepSize = 50;

	int totalRuns = ((endWavelength - startWavelength) / wavelengthStep + 1) * (((maxPerturbanceDev)/2.0)+1) * avg * (((endLayers - startLayers)/layerStepSize)+1);

	std::cout << "Total Runs : " << totalRuns << std::endl;
	int simIndex = 0;

	json j_angle;

	std::cout << "Starting Angle: " << angle << " degrees" << std::endl;

	std::string angleName = "Angle_" + std::to_string(angle);

	std::string angleFilePath = filePath + angleName;

	CreateFolder(angleFilePath);

	for (int p = 0; p <= maxPerturbanceDev; p += 2)
	{
		json j_perturbance;

		std::string perturbanceName = "PerturbanceDev_" + std::to_string(p);

		std::string perturbanceFilePath = angleFilePath + "/" + perturbanceName;

		CreateFolder(perturbanceFilePath);

		for (int l = startLayers; l <= endLayers; l += layerStepSize)
		{
			json j_layers;

			std::string layerName = "Layers_" + std::to_string(l);

			std::string layerFilePath = perturbanceFilePath + "/" + layerName;

			CreateFolder(layerFilePath);

			for (int y = startWavelength; y <= endWavelength; y += wavelengthStep)
			{
				json j_wavelength;

				std::string wavelengthName = "Wavelength_" + std::to_string(y);

				std::string wavelengthFilePath = layerFilePath + "/" + wavelengthName;

				CreateFolder(wavelengthFilePath);

				std::vector<std::string> filePaths = std::vector<std::string>();

				auto start = std::chrono::high_resolution_clock::now();

				for (int i = 0; i < avg; i++)
				{
					filePaths.push_back(RunWavelengthSweepWithPerturbance(wavelengthFilePath, l, y, numOfRays, i, angle, p));

					simIndex++;
				}

				double percentComplete = ((double)simIndex / (double)totalRuns) * 100.0;

				auto end = std::chrono::high_resolution_clock::now();

				double timeTakenMS = std::chrono::duration<double, std::milli>(end - start).count();

				std::cout << "Completed Wavelength " << y << " : " << percentComplete << "%" << " (" << timeTakenMS << " ms)" << std::endl;

				j_layers[wavelengthName] = filePaths;
			}

			std::cout << "Completed Layers " << l << std::endl;

			j_perturbance[layerName] = j_layers;
		}

		std::cout << "Completed Perturbance " << p << std::endl;

		j_angle[perturbanceName] = j_perturbance;
	}

	std::ofstream file(filePath + "FilePaths_Angle_" + std::to_string(angle) + ".json");
	file << j_angle.dump(2);
	file.close();
}

void RunSimulationCategory3(int angle)
{
	std::string filePath = "Simulations/Simulation3_WavelengthSweepWithWaveWithPerturbance";

	CreateFolder("Simulations");
	CreateFolder(filePath);

	filePath = filePath + "/";

	int avg = 3; //10
	int numOfRays = 10; //100
	int maxPerturbanceDev = 10; //10

	int angleStep = 5;
	int wavelengthStep = 1;
	int startWavelength = 300;
	int endWavelength = 325; //1000
	int startLayers = 50;
	int endLayers = 100; //250
	int layerStepSize = 50;

	int totalRuns = ((endWavelength - startWavelength) / wavelengthStep + 1) * (((maxPerturbanceDev) / 2.0) + 1) * avg * (((endLayers - startLayers) / layerStepSize) + 1);

	std::cout << "Total Runs : " << totalRuns << std::endl;
	int simIndex = 0;

	json j_angle;

	std::cout << "Starting Angle: " << angle << " degrees" << std::endl;

	std::string angleName = "Angle_" + std::to_string(angle);

	std::string angleFilePath = filePath + angleName;

	CreateFolder(angleFilePath);

	for (int p = 0; p <= maxPerturbanceDev; p += 2)
	{
		json j_perturbance;

		std::string perturbanceName = "PerturbanceDev_" + std::to_string(p);

		std::string perturbanceFilePath = angleFilePath + "/" + perturbanceName;

		CreateFolder(perturbanceFilePath);

		for (int l = startLayers; l <= endLayers; l += layerStepSize)
		{
			json j_layers;

			std::string layerName = "Layers_" + std::to_string(l);

			std::string layerFilePath = perturbanceFilePath + "/" + layerName;

			CreateFolder(layerFilePath);

			for (int y = startWavelength; y <= endWavelength; y += wavelengthStep)
			{
				json j_wavelength;

				std::string wavelengthName = "Wavelength_" + std::to_string(y);

				std::string wavelengthFilePath = layerFilePath + "/" + wavelengthName;

				CreateFolder(wavelengthFilePath);

				std::vector<std::string> filePaths = std::vector<std::string>();

				auto start = std::chrono::high_resolution_clock::now();

				for (int i = 0; i < avg; i++)
				{
					filePaths.push_back(RunWavelengthSweepWithWaveWithPerturbance(wavelengthFilePath, l, y, numOfRays, i, angle, p));

					simIndex++;
				}

				double percentComplete = ((double)simIndex / (double)totalRuns) * 100.0;

				auto end = std::chrono::high_resolution_clock::now();

				double timeTakenMS = std::chrono::duration<double, std::milli>(end - start).count();

				std::cout << "Completed Wavelength " << y << " : " << percentComplete << "%" << " (" << timeTakenMS << " ms)" << std::endl;

				j_layers[wavelengthName] = filePaths;
			}

			std::cout << "Completed Layers " << l << std::endl;

			j_perturbance[layerName] = j_layers;
		}

		std::cout << "Completed Perturbance " << p << std::endl;

		j_angle[perturbanceName] = j_perturbance;
	}

	std::ofstream file(filePath + "FilePaths_Angle_" + std::to_string(angle) + ".json");
	file << j_angle.dump(2);
	file.close();
}

void RunSimulations(int argc, char* argv[])
{
	std::cout << "Select a simulation to run:\n";
	std::cout << "1. Simulation 1 - Wavelength Sweep\n";
	std::cout << "2. Simulation 2 - Wavelength Sweep with Perturbance\n";
	std::cout << "3. Simulation 3 - Wavelength Sweep with Perturbance and Wavy Structure\n";
	//std::cout << "4. Simulation 4 - Wavy Moth Eye Layers + Segment Normal Perturbance + AM15G Spectrum\n";
	std::cout << "Enter a number (1-4): ";

	int choice;
	
	if (argc >= 2)
		choice = std::stoi(argv[1]);
	else
		std::cin >> choice;

	switch (choice)
	{
	case 1:
		RunSimulationCategory1();
		break;

	case 2:
		if (argc >= 3)
			RunSimulationCategory2(std::stoi(argv[2]));
		else
			RunSimulationCategory2(50);

		break;

	case 3:

		if (argc >= 3)
			RunSimulationCategory3(std::stoi(argv[2]));
		else
			RunSimulationCategory3(0);
		break;

		//case 4:
			//RunSimulationCategory4();
		//	break;

	default:
		std::cout << "Invalid choice.\n";
		break;
	}
}

