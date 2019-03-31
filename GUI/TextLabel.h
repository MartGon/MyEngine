#include "GameObject.h"

#include <unordered_map>
#include <vector>

#pragma once

class TextLabel : public GameObject 
{
public:
	TextLabel();

	// Own methods
	void setText(std::string text);
	std::string getText();

	void setTextScale(Vector2<float> scale);
	Vector2<float> getTextScale();

private:
	// Attributes
	std::string text;

	// Texture renderers used for loading fonts
	std::vector<TextureRenderer*> font_tRenderers;

	// Can be used to obtain texture by char
	static bool isInitiliazed;
	static std::unordered_map<char, Texture> texture_map;

	// Fonts path for loading textures
	const std::string fonts_path = std::string("fonts/");
	const std::string fonts_ext = std::string(".png");

	// Private Methods
	void fixCharPositions();
	Texture getTextureByChar(char c);
	std::string getTexturePath(char c);
	void init();
};