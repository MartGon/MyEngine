#pragma once
#include <math.h>
#include <string>

class Utilities;

template <typename T>
class Vector2
{
public:
	Vector2()
	{
		x = 0;
		y = 0;
	}

	Vector2(T x, T y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2(double angle)
	{
		this->x = cos(Utilities::toRads(angle));
		this->y = sin(Utilities::toRads(angle));
	}

	~Vector2()
	{

	}

	// Coordinates
	T x;
	T y;

	// Methods
	void normalize()
	{
		float mod = getModule();
		x /= mod;
		y /= mod;
	}

	float getModule()
	{
		return sqrt(x * x + y * y);
	}

	double getAngle(bool rads = false)
	{
		double angle = atan(y / x);

		// If x is negative, we need to add 180 degrees in order to get the rigth angle
		if (x < 0)
			angle += 3.14159265;


		return rads ? angle : Utilities::toDegs(angle);
	}

	std::string toStr()
	{
		return std::string("(") + std::to_string(x) + std::string(", ") + std::to_string(y) + std::string(")");
	}

    float dot(Vector2<T> vector)
    {
        return x * vector.x + y * vector.y;
    }

    // This projected onto b
    float project(Vector2<T> b)
    {
        // Projected module = (vector to be projected) · (vector to be porjected onto) / mod(vector to be porjected onto)
        return dot(b) / b.getModule();
    }

	// Operators
	friend Vector2<T> operator+(const Vector2<T> &v1, const Vector2<T> &v2)
	{
		T fX = v1.x + v2.x;
		T fY = v1.y + v2.y;

		return Vector2(fX, fY);
	}

	friend Vector2<T> operator-(const Vector2<T> &v1, const Vector2<T> &v2)
	{
		T fX = v1.x - v2.x;
		T fY = v1.y - v2.y;

		return Vector2(fX, fY);
	}

	template <typename L>
	friend Vector2<L> operator*(const Vector2<L> &vector, const T &value)
	{
		L sX = vector.x * value;
		L sY = vector.y * value;

		return Vector2<L>(sX, sY);
	}

	template <typename L>
	friend Vector2<L> operator/(const Vector2<L> &vector, const T &value)
	{
		L sX = vector.x / value;
		L sY = vector.y / value;

		return Vector2<L>(sX, sY);
	}

	friend Vector2<T> operator*(const Vector2<T> &x1, const Vector2<T> &x2)
	{
		T sX = x1.x * x2.x;
		T sY = x1.y * x2.y;

		return Vector2<T>(sX, sY);
	}

	friend Vector2<T> operator/(const Vector2<T> &x1, const Vector2<T> &x2)
	{
		T sX = x1.x / x2.x;
		T sY = x1.y / x2.y;

		return Vector2<T>(sX, sY);
	}

	template <typename L>
	operator Vector2<L>()
	{
		Vector2<L> v;
		v.x = (L)x;
		v.y = (L)y;

		return v;
	}
};