#include "GUIComponent.h"

#include <unordered_map>
#include <vector>

#pragma once

class TextLabel : public GUIComponent
{
public:
	TextLabel();

	// Own methods
	void setText(std::string text);
	std::string getText();

	// Scale
	void setTextScale(Vector2<float> scale);
	Vector2<float> getTextScale();

	// Set text color
	void setTextColor(MapRGB color);

	// Char pos
	Vector2<float> getNextCharPos(int index);

	// Override
	Vector2<float> getDimensions() override;

	static std::unordered_map<char, Texture> texture_map;

	// Texture renderers used for loading fonts
	std::vector<TextureRenderer*> font_tRenderers;
private:
	// Attributes
	std::string text;
	MapRGB text_color{255, 255, 255};

	// Fonts path for loading textures
	const std::string fonts_path = std::string("fonts/");
	const std::string fonts_ext = std::string(".png");

	// Private Methods
	void fixCharPositions();
	Texture getTextureByChar(char c);
	std::string getTexturePath(char c);
	void init();
};