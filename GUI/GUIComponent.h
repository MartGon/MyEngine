#pragma once
#include "GameObject.h"

class GUIComponent : public GameObject
{
public:
	GUIComponent() : GameObject() {};
	GUIComponent(Texture texture) : GameObject(texture)
	{

	};

	Uint8 getLayer();
	void setLayer(Uint8 layer);

private:
	Uint8 layer = 0;
};