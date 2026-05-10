#include <cmath>
#pragma once

namespace Functions
{
	inline double lab6Function(double x)
	{
		return x * x + 5 * cos(x) - 3;
	}

	inline double lab6DFunction(double x)
	{
		return 2 * x - 5 * sin(x);
	}

	inline double lab8Function(double x)
	{
		return x * x + 5 * cos(x);
	}
}
