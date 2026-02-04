#pragma once
#include <vector>
#include <filesystem>
#include <string>
#include <iostream>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define access _access
#else
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
// F_OK is used with access() on Linux to check if file exists
#endif


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

bool DirectoryExists(const std::string& path)
{
#ifdef _WIN32
	return _access(path.c_str(), 0) == 0;
#else
	return access(path.c_str(), 0) == 0;
#endif
}

void CreateFolder(const std::string& path)
{
	if (DirectoryExists(path))
		return;

#ifdef _WIN32
	if (_mkdir(path.c_str()) != 0)
#else
	if (mkdir(path.c_str(), 0777) != 0)
#endif
	{
		std::cout << "Failed to create directory.\n";
	}
}
