#pragma once
#include "Component.h"
#include "Utilities.h"
#include <vector>

template <class T>
class Manager
{
public:
	virtual void manage() {};

	void addComponent(T component)
	{
		components.push_back(component);
		onAddComponent(component);
	}

	void removeComponent(T component)
	{
		Utilities::removeFromVector(component, components);
		onRemoveComponent(component);
	}

	virtual void onAddComponent(T component) {}
	virtual void onRemoveComponent(T component) {}

protected:
	std::vector<T> components;
};