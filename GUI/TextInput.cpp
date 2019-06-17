#include "TextInput.h"

// Constructors
TextInput::TextInput(Texture background, std::string placeholder) : Button(background)
{
	// Set method
	onClick = std::bind(&TextInput::OnClick, this);

	// Set text
	tLabel->setText(placeholder);

	// Init component
	MapRGB colorKey{0, 255, 0};
	cursor_Renderer = setComponent(new TextureRenderer("fonts/cursor.png", &colorKey));
	cursor_Renderer->isEnabled = false;
	cursor_Renderer->setBlink(4, -1);
}

// Methods

void TextInput::setSelected(bool select)
{
	isSelected = select;

	// Enable
	cursor_Renderer->isEnabled = select;

	// Update cursor
	if (select)
	{
		int index = tLabel->getText().size();
		auto last_char_pos = tLabel->getNextCharPos(index);
		updateCursor(last_char_pos);
	}
}

void TextInput::updateCursor(Vector2<float> last_char_pos)
{
	cursor_Renderer->render_offset = tLabel->transform.position + last_char_pos;
}

void TextInput::OnClick()
{
	setSelected(true);
}

// Overrided methods

bool TextInput::OnHandleEvent(const SDL_Event& event)
{
	bool result = Button::OnHandleEvent(event);

	if (!isSelected)
		return result;

	if (event.type == SDL_KEYDOWN)
	{
		// Update text
		SDL_Keycode code = event.key.keysym.sym;

		std::string next_text = tLabel->getText();
		if (next_text.size() < text_limit)
		{
			if (code == '\b')
			{
				if (!next_text.empty())
					next_text.erase(next_text.size() - 1, 1);
			}
			else if (code >= '0' && code <= '9' || code == '.')
				next_text += (char)code;

			// Update text
			tLabel->setText(next_text);

			// Update cursor
			int index = next_text.size();
			auto last_char_pos = tLabel->getNextCharPos(index);
			updateCursor(last_char_pos);
		}

		// Update flag
		result = true;
	}

	return result;
}