#include "GUIComponent.h"

Uint8 GUIComponent::getLayer()
{
	Uint8 l = this->layer;

	if (Transform* t = transform.parent)
		if (GameObject* go = t->gameObject)
			if (GUIComponent* gui = dynamic_cast<GUIComponent*>(go))
				l = gui->layer;

	return l;
}

void GUIComponent::setLayer(Uint8 layer)
{
	this->layer = layer;
}
