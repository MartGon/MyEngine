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
	if (isEnabled)
	{
		// Inverse flag
		selected = !selected;

		// De/Activate selected tRenderer
		cross_tRenderer->isEnabled = selected;
	}
}

void CheckBox::setSelected(bool select)
{
	// Inverse flag
	selected = select;

	// De/Activate selected tRenderer
	cross_tRenderer->isEnabled = select;
}

// overrided methods

void CheckBox::onSetEnabled(bool enabled)
{
	if(enabled)
		cross_tRenderer->texture.color_mod = { 255, 255, 255 };
	else
		cross_tRenderer->texture.color_mod = { 127, 127, 127 };
}