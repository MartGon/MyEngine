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
	void setSelected(bool selected);

	// Overrided
	void onSetEnabled(bool enabled) override;

private:
	// Attributes
	bool selected = false;

	// Components
	TextureRenderer* cross_tRenderer = nullptr;
};