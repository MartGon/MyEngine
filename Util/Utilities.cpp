#include "Utilities.h"

const double Utilities::PI = 3.14159265;

Utilities::Utilities()
{
}

Utilities::~Utilities()
{
}

int Utilities::getNumberOfDigits(int number)
{
	if (!number)
		return 1;

	int n = 0;
	float fNumber = number;

	while (fNumber >= 1)
	{
		fNumber /= 10;
		n++;
	}

	return n;
}

// Lo devuelve en el otro sentido
// Para el número 123
// Lo devuelve [0] = 3, [1] = 2, [2] = 3

std::vector<int> Utilities::getDigitsFromNumber(int number)
{
	int digitsNumber = getNumberOfDigits(number);
	std::vector<int> retVector;

	for (int i = 0; i < digitsNumber; i++)
	{
		int digit = number % 10;
		retVector.push_back(digit);
		number /= 10;
	}

	return retVector;
}

double Utilities::toRads(double deg_angle)
{
	return deg_angle * Utilities::PI / 180;
}

double Utilities::toDegs(double rad_angle)
{
	return rad_angle *  180 / Utilities::PI;
}


Vector2<float> Utilities::rotatePointFromCenter(Vector2<float> rotation_center, double angle, Vector2<float> point)
{
    Vector2<float> rotationVector = point - rotation_center;
    double rad_angle = Utilities::toRads(angle);

    double x = cos(rad_angle) * rotationVector.x - sin(rad_angle) * rotationVector.y;
    double y = sin(rad_angle) * rotationVector.x + cos(rad_angle) * rotationVector.y;

    Vector2<float> rotatedVector((float)x, (float)y);

    Vector2<float> rotatedPoint = rotatedVector + rotation_center;

    return  rotatedPoint;
}
