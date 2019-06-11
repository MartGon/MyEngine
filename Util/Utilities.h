#pragma once
#include <vector>
#include "Vector2.h"

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

	// Utilities
    static Vector2<float> rotatePointFromCenter(Vector2<float> rotation_center, double angle, Vector2<float> point);

	template <typename T>
	static std::vector<T> removeFromVector(T pointer, std::vector<T>& vector)
	{
		if(std::find(vector.begin(),vector.end(), pointer) != vector.end())
			vector.erase(std::remove(vector.begin(), vector.end(), pointer), vector.end());
		return vector;
	}
};

