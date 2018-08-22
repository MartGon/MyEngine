#include "RotatableBoxCollider.h"
#include "Utilities.h"
#include "RendererManager.h"
#include "GameObject.h"
#include <cmath>
#include <math.h>

RotatableBoxCollider::RotatableBoxCollider(Vector2<int> v1, Vector2<int> v2, Vector2<int> v3, Vector2<int> v4)
{
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
	this->v4 = v4;

	vertex[0] = &(this->v1);
	vertex[1] = &(this->v2);
	vertex[2] = &(this->v3);
	vertex[3] = &(this->v4);

	A = v2 - v1;
	B = v3 - v1;
}

Vector2<int> RotatableBoxCollider::rotateVertex(Vector2<int> rotationCenter, double angle, Vector2<int> vertex)
{
	Vector2<int> rotationVector = vertex - rotationCenter;
	double rad_angle = Utilities::toRads(angle);

	double x = cos(rad_angle) * rotationVector.x - sin(rad_angle) * rotationVector.y;
	double y = sin(rad_angle) * rotationVector.x + cos(rad_angle) * rotationVector.y;

	Vector2<double> rotatedVector(x, y);

	Vector2<double> doubRotatedPoint = rotatedVector + (Vector2<double>)rotationCenter;

	Vector2<int> intRotPoint = Vector2<int>(std::round(doubRotatedPoint.x), std::round(doubRotatedPoint.y));

	return intRotPoint;
}

void RotatableBoxCollider::rotate(Vector2<int> rotationCenter, double angle)
{
	for(int i = 0; i < 4; i++)
		*(vertex[i]) = rotateVertex(rotationCenter, angle, *vertex[i]);
}

std::string RotatableBoxCollider::vertexValuesToStr()
{
	std::string str("");

	for (int i = 0; i < 4; i++)
		str += std::string("v") + std::to_string(i) + std::string(" ") + vertex[i]->toStr() + std::string("\n");

	return str;
}

void RotatableBoxCollider::draw()
{
	SDL_Renderer *renderer = RendererManager::renderer;
	Vector2<float> scaler = RendererManager::getScaler();
	Vector2<int> cam_pos = RendererManager::getCameraPosition();
	SDL_Point points[4];
	
	for (int i = 0; i < 4; i++)
	{
		points[i].x = (vertex[i]->x + gameObject->transform.position.x - cam_pos.x) * scaler.x;
		points[i].y = (vertex[i]->y + gameObject->transform.position.y - cam_pos.y) * scaler.y;
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLines(renderer, points, 4);
}