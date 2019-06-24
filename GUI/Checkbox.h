#pragma once

#include "Button.h"

class CheckBox : public Button
{
public:
	// Constructor
	CheckBox(Texture background);

	// Getter
	bool isSelected();
	bool isSelectable = true;

	// Setter
	void select();
	void setSelected(bool selected);

	// Components
	TextureRenderer* cross_tRenderer = nullptr;

private:
	// Attributes
	bool selected = false;
};