#include "RotatableBoxCollider.h"
#include "Utilities.h"
#include "RendererManager.h"
#include "GameObject.h"
#include <cmath>
#include <math.h>

RotatableBoxCollider::RotatableBoxCollider(Vector2<int> v0, Vector2<int> v1, Vector2<int> v2, Vector2<int> v3)
{
	// Read Only
	roVertex[0] = v0;
	roVertex[1] = v1;
	roVertex[2] = v2;
	roVertex[3] = v3;

	// Logic values
	vertex[0] = v0;
	vertex[1] = v1;
	vertex[2] = v2;
	vertex[3] = v3;

	// Setting dir vectors
	A = v1 - v0;
	B = v0 - v2;
}

Vector2<float> RotatableBoxCollider::rotateVertex(Vector2<int> rotationCenter, double angle, Vector2<float> vertex)
{
	Vector2<float> rotationVector = vertex - Vector2<float>(rotationCenter);
	double rad_angle = Utilities::toRads(angle);

	double x = cos(rad_angle) * rotationVector.x - sin(rad_angle) * rotationVector.y;
	double y = sin(rad_angle) * rotationVector.x + cos(rad_angle) * rotationVector.y;

	Vector2<float> rotatedVector((float)x, (float)y);

	Vector2<float> rotatedPoint = rotatedVector + (Vector2<float>)rotationCenter;;

	return  rotatedPoint;
}

void RotatableBoxCollider::setRotation(Vector2<int> rotationCenter, double angle)
{
	for(int i = 0; i < 4; i++)
		vertex[i] = rotateVertex(rotationCenter, angle, roVertex[i]);
}

// Checkers

bool RotatableBoxCollider::checkCollision(RotatableBoxCollider collider)
{
    std::array<Vector2<float>, 4> o_vertex = collider.vertex;
    std::array<Vector2<float>, 2> sides;
    sides[0] = vertex[1] - vertex[0];
    sides[1] = vertex[2] - vertex[0];

    for (Uint8 i = 0; i < 4; i++)
    {
        for (Uint8 j = 0; j < 2; j++)
        {
            // Origin to other collider vertex vector
            Vector2<float> point = (o_vertex[i] + collider.gameObject->transform.position);
            Vector2<float> axisCenter = (vertex[0] + gameObject->transform.position);
            Vector2<float> op = point - axisCenter;

            // Project op onto side
            float proj = op.project(sides[j]);
            float sideLength = sides[j].getModule();

            // Check if this vertex is not inside the box
            if (proj > sideLength || proj < 0)
                break;

            // If we have checked both sides, and it is in both of them,
            // then the point is inside the box
            if (j)
                return true;
        }
    }

    return false;
}

// Debug

std::string RotatableBoxCollider::vertexValuesToStr()
{
	std::string str("");

	for (int i = 0; i < 4; i++)
		str += std::string("v") + std::to_string(i) + std::string(" ") + vertex[i].toStr() + std::string("\n");

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
		points[i].x = (vertex[i].x + gameObject->transform.position.x - cam_pos.x) * scaler.x;
		points[i].y = (vertex[i].y + gameObject->transform.position.y - cam_pos.y) * scaler.y;
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer, points[0].x, points[0].y, points[1].x, points[1].y);
	SDL_RenderDrawLine(renderer, points[0].x, points[0].y, points[2].x, points[2].y);
	SDL_RenderDrawLine(renderer, points[1].x, points[1].y, points[3].x, points[3].y);
	SDL_RenderDrawLine(renderer, points[2].x, points[2].y, points[3].x, points[3].y);
}