#pragma once

#include "Button.h"

class CheckBox : public Button
{
public:
	// Constructor
	CheckBox(Texture background);

	// Getter
	bool isSelected();

	// Setter
	void select();

	// Components
	TextureRenderer* cross_tRenderer = nullptr;

private:
	// Attributes
	bool selected = false;
};