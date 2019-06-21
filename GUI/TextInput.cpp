#include "TextInput.h"

#include <regex>

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
	cursor_Renderer->setBlink(8, -1);
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

		// OnSelect hook
		onSelect();
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

std::string TextInput::getText()
{
	return tLabel->getText();
}

void TextInput::setText(std::string text)
{
	tLabel->setText(text);
	tLabel->setCenteredWithinParent();
}

// Overrided methods

bool TextInput::OnHandleEvent(const SDL_Event& event)
{
	bool result = Button::OnHandleEvent(event);

	if (!isSelected)
		return result;

	std::string next_text = tLabel->getText();

	if (event.type == SDL_TEXTINPUT)
	{
		// Get input text
		std::string new_text = std::string(event.edit.text);

		if (std::regex_match(new_text, valid_inputs))
		{

			// Append chars until limit is reached
			for (int i = 0; i < new_text.size(); i++)
			{
				if (next_text.size() < text_limit)
					next_text.append(std::string(1, new_text[i]));
				else
					break;
			}

			// Update flag
			result = true;
		}
	}
	else if (event.type == SDL_KEYDOWN)
	{
		SDL_Keycode code = event.key.keysym.sym;

		switch (code)
		{
		case SDLK_BACKSPACE:
			if (!next_text.empty())
				next_text.erase(next_text.size() - 1, 1);

			// Update flag
			result = true;
			break;
		case SDLK_RETURN:
			setSelected(false);
			break;
		}
	}

	if (result)
	{
		// Update text and format
		setText(next_text);

		// Update cursor
		int index = tLabel->getText().size();
		auto last_char_pos = tLabel->getNextCharPos(index);
		updateCursor(last_char_pos);
	}

	return result;
}