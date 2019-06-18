#pragma once
#include "Button.h"
#include "TextLabel.h"

class TextInput : public Button
{
public:
	// Constructors
	TextInput(Texture background, std::string placeholder);

	// Components
	TextureRenderer* cursor_Renderer = nullptr;

	// Methods
	void setSelected(bool select);
	void updateCursor(Vector2<float> last_char_pos);
	void OnClick();

	std::string getText();
	void setText(std::string text);

	// Overrided methods
	bool OnHandleEvent(const SDL_Event& event) override;

	// Attributes
	bool isSelected = false;
	int text_limit = 21;
};