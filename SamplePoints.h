#pragma once
#include <vector>
#include "Framework/T5Maths.h"

class SamplePoints
{
public:
	std::vector<T5Vec3> samplePoints;
	std::vector<int> samplePointsColours;

	static SamplePoints* Instance;
};

