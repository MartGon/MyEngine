#pragma once
#include <vector>

class Utilities
{
public:
	Utilities();
	~Utilities();

	static int getNumberOfDigits(int number);
	static std::vector<int> getDigitsFromNumber(int number);

	static double toRads(double deg_angle);
	static double toDegs(double rad_angle);

	static const double PI;
};

