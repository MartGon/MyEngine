#pragma once
#include "Component.h"
#include "Utilities.h"
#include <vector>
#include "ManagerBase.h"

template <class T>
class Manager : public ManagerBase
{
public:
	void addComponent(T component)
	{
		components.push_back(component);
		onAddComponent(component);
	}

	void removeComponent(T component)
	{
		beforeRemoveComponent(component);
		Utilities::removeFromVector(component, components);
		onRemoveComponent(component);
	}

	virtual void onAddComponent(T component) {}
	virtual void onRemoveComponent(T component) {}
	virtual void beforeRemoveComponent(T component) {}

	std::vector<T> getComponents()
	{
		return components;
	}

protected:
	std::vector<T> components;
};