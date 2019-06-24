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
	if (isSelectable)
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