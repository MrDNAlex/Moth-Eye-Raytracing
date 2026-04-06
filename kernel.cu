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
#include "FYDPSims.h"
#include "SolcoreSims.h"

int main(int argc, char* argv[])
{
	RunSimulations(argc, argv);
	std::cout << "Calculation Complete!" << std::endl;
}
