#include "Checkbox.h"

// Constructor

CheckBox::CheckBox(Texture background) : Button(background)
{
	// Bind function call
	onClick = std::bind(&CheckBox::select, this);

	// Create cross tRenderer
	cross_tRenderer = setComponent(new TextureRenderer("checkbox_check.png"));
	cross_tRenderer->isEnabled = false;
}

// Getter

bool CheckBox::isSelected()
{
	return selected;
}

// Setter

void CheckBox::select()
{
	// Inverse flag
	selected = !selected;

	// De/Activate selected tRenderer
	cross_tRenderer->isEnabled = selected;
}