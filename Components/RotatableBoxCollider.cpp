#include "RotatableBoxCollider.h"
#include "Utilities.h"
#include "RendererManager.h"
#include "GameObject.h"
#include "SceneManager.h"
#include <cmath>
#include <math.h>

RotatableBoxCollider::RotatableBoxCollider(Vector2<int> v0, Vector2<int> v1, Vector2<int> v2, Vector2<int> v3) : Collider()
{
	// Read Only
	roVertex[0] = v0;
	roVertex[1] = v1;
	roVertex[2] = v2;
	roVertex[3] = v3;

	// Logic values
	vertex = roVertex;

}

RotatableBoxCollider::RotatableBoxCollider(RotatableBoxCollider pre, RotatableBoxCollider post) {
	roVertex = { pre.roVertex[0],pre.roVertex[1],post.roVertex[2],post.roVertex[3] };
	vertex = roVertex;
}

RotatableBoxCollider::RotatableBoxCollider(BoxCollider *collider) : 
	RotatableBoxCollider(Vector2<int>(0, 0), Vector2<int>(0, collider->getDimensions().y),
		Vector2<int>(collider->getDimensions().x, 0), collider->getDimensions())
{
	offset = collider->offset;
    debug = collider->debug;
}

RotatableBoxCollider::~RotatableBoxCollider()
{
	destroy();
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
		vertex[i] = rotateVertex(rotationCenter, angle + rotation_offset, roVertex[i]);
}

Vector2<int> RotatableBoxCollider::getColliderCenter()
{
    Vector2<int> center;

    center.x = roVertex[3].x / 2;
    center.y = roVertex[3].y / 2;

    return center;
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
            Vector2<float> point = (o_vertex[i] + collider.gameObject->getAbsolutePosition() + collider.offset * collider.gameObject->transform.scale);
            Vector2<float> axisCenter = (vertex[0] + gameObject->getAbsolutePosition() + offset * gameObject->transform.scale);
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

	// Check for center
	Vector2<float> center = { collider.vertex[3].x / 2, collider.vertex[3].y / 2 };

	for (Uint8 j = 0; j < 2; j++)
	{
		// Origin to other collider vertex vector
		Vector2<float> point = (center + collider.gameObject->getAbsolutePosition() + collider.offset * collider.gameObject->transform.scale);
		Vector2<float> axisCenter = (vertex[0] + gameObject->getAbsolutePosition() + offset * gameObject->transform.scale);
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

    return false;
}

// TODO - Needs overhaul
bool RotatableBoxCollider::isCollidingWith(Collider* collider)
{
	if (RotatableBoxCollider* c2 = dynamic_cast<RotatableBoxCollider*>(collider))
		return checkCollision(*c2);
	else if (BoxCollider* col = dynamic_cast<BoxCollider*>(collider))
	{
		RotatableBoxCollider* c2 = new RotatableBoxCollider(col);
		c2->gameObject = col->gameObject;
		bool result = checkCollision(*c2);
		delete c2;

		return result;
	}

	return false;
}

// Upper

void RotatableBoxCollider::start()
{
    if (Vector2<int> *rotation = gameObject->transform.rotationCenter)
        setRotation(*rotation, gameObject->transform.zRotation);
    else
        setRotation(getColliderCenter(), gameObject->transform.zRotation);
}

void RotatableBoxCollider::update()
{
    // TODO - Take box center as rotation center when rotation == nullptr
    if (gameObject)
    {
        if (Vector2<int> *rotation = gameObject->transform.rotationCenter)
            setRotation(*rotation, gameObject->transform.zRotation);
        else
            setRotation(getColliderCenter(), gameObject->transform.zRotation);
    }
}

// Debug

std::string RotatableBoxCollider::vertexValuesToStr()
{
	std::string str("");

	for (int i = 0; i < 4; i++)
		str += std::string("v") + std::to_string(i) + std::string(" ") + vertex[i].toStr() + std::string("\n");

	return str;
}

void RotatableBoxCollider::drawCollisionBoundaries()
{
	SDL_Renderer *renderer = RendererManager::renderer;
	Vector2<float> scaler = RendererManager::getScaler();
	Vector2<int> cam_pos = RendererManager::getCameraPosition();
	SDL_Point points[4];
	
	for (int i = 0; i < 4; i++)
	{
		points[i].x = (std::round(vertex[i].x) + gameObject->getAbsolutePosition().x + offset.x - cam_pos.x) * scaler.x;
		points[i].y = (std::round(vertex[i].y) + gameObject->getAbsolutePosition().y + offset.y - cam_pos.y) * scaler.y;
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer, points[0].x, points[0].y, points[1].x, points[1].y);
	SDL_RenderDrawLine(renderer, points[0].x, points[0].y, points[2].x, points[2].y);
	SDL_RenderDrawLine(renderer, points[1].x, points[1].y, points[3].x, points[3].y);
	SDL_RenderDrawLine(renderer, points[2].x, points[2].y, points[3].x, points[3].y);
}