#include "BoxCollider.h"
#include "GameObject.h"
#include "RendererManager.h"
#include <algorithm>


BoxCollider::BoxCollider() : Collider()
{

}

BoxCollider::BoxCollider(Texture texture) : BoxCollider(texture.mWidth, texture.mHeight)
{
}

BoxCollider::BoxCollider(int width, int height) : Collider()
{
	// Set values
	cWidth = width;
	cHeight = height;
}

void BoxCollider::calculateColliderBoundaries()
{
	Vector2<float> position = gameObject->transform.position;
	Vector2<float> dimensions = getDimensions();
	Vector2<float> offsetVector = getOffsetVector();

	int fWidth = dimensions.x;
	int fHeight = dimensions.y;
	int xOffset = offsetVector.x;
	int yOffset = offsetVector.y;

	cLeft = position.x + xOffset;
	cRight = position.x + fWidth + xOffset;
	cTop = position.y + yOffset;
	cBottom = position.y + fHeight + yOffset;
}

Vector2<float> BoxCollider::getCollisionCenter()
{
	int x = (float)(cLeft + cRight) / 2;
	int y = (float)(cTop + cBottom) / 2;

	return Vector2<float>(x, y);
}

void BoxCollider::update()
{
	calculateColliderBoundaries();
}

bool BoxCollider::checkCollision(BoxCollider* collider)
{
	if (cRight <= collider->cLeft)
		return false;

	if (cLeft >= collider->cRight)
		return false;

	if (cBottom <= collider->cTop)
		return false;

	if (cTop >= collider->cBottom)
		return false;

	return true;
}

// TODO - Needs overhaul
bool BoxCollider::isCollidingWith(Collider* collider)
{
	if (BoxCollider* box_collider = dynamic_cast<BoxCollider*>(collider))
		return checkCollision(box_collider);
	else if (RotatableBoxCollider *rot_collider = dynamic_cast<RotatableBoxCollider*>(collider))
	{
		RotatableBoxCollider c1(this);
		c1.gameObject = this->gameObject;
		return c1.checkCollision(*rot_collider);
	}

	return false;
}

void BoxCollider::drawCollisionBoundaries()
{
	SDL_Renderer* renderer = RendererManager::renderer;
	Vector2<float> scaler = RendererManager::getScaler();
	Vector2<int> cam_pos = RendererManager::getCameraPosition();

	SDL_Rect rect;
	rect.x = (cLeft - cam_pos.x) * scaler.x ;
	rect.w = (cRight - cLeft) * scaler.x ;
	rect.y = (cBottom - cam_pos.y) * scaler.y ;
	rect.h = (cTop - cBottom) * scaler.y;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(renderer, &rect);
}

void BoxCollider::destroy()
{
	//gColliders.erase(std::remove(gColliders.begin(), gColliders.end(), this), gColliders.end());
	// TODO - Remove from collision manager list
}

Vector2<float> BoxCollider::getDimensions()
{
	int fWidth = cWidth * gameObject->transform.scale.x;
	int fHeight = cHeight * gameObject->transform.scale.y;

	return Vector2<float>(fWidth, fHeight);
}

Vector2<float> BoxCollider::getOffsetVector()
{
	int xOffset = offset.x * gameObject->transform.scale.x;
	int yOffset = offset.y * gameObject->transform.scale.y;

	return Vector2<float>(xOffset, yOffset);
}