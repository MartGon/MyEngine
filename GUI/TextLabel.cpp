#include "TextLabel.h"

bool TextLabel::isInitiliazed = false;
std::unordered_map<char, Texture> TextLabel::texture_map = std::unordered_map<char, Texture>();

// Constructor
TextLabel::TextLabel() 
{
	if(!isInitiliazed)
		init();

	Texture font = getTextureByChar('0');
	TextureRenderer* tRenderer = setComponent(new TextureRenderer(font, 255));
	font_tRenderers.push_back(tRenderer);
}

// Public Methods
void TextLabel::setText(std::string text) 
{
	// Set value
	this->text = text;

	// Change texture renderers
	int diff = text.length() - font_tRenderers.size();
	int to_modify = diff >= 0 ? font_tRenderers.size() : text.length();

	// Modify texture in tRenderers already created
	for (int i = 0; i < to_modify; i++)
	{
		TextureRenderer* tRenderer = font_tRenderers.at(i);
		tRenderer->texture = getTextureByChar(text.at(i));
	}

	// We may need to create more tRenderers
	if (diff >= 0)
	{
		// Create the ones we need
		for (int i = to_modify; i < text.length(); i++)
		{
			Texture font = getTextureByChar(text.at(i));
			TextureRenderer* tRenderer = setComponent(new TextureRenderer(font, 255));
			font_tRenderers.push_back(tRenderer);
		}
	}
	// We have more than enough Texture Renderers 
	else
	{
		// Disable the ones we dont need
		for (int i = to_modify; i < font_tRenderers.size(); i++)
		{
			TextureRenderer* tRenderer = font_tRenderers.at(i);
			tRenderer->isEnabled = false;
		}
	}

	// Fix positions
	for (int i = 1; i < font_tRenderers.size(); i++)
	{
		TextureRenderer* tRenderer = font_tRenderers.at(i);

		if (!tRenderer->isEnabled)
			continue;

		TextureRenderer* prev_tRenderer = font_tRenderers.at(i - 1);
		int prev_font_w = prev_tRenderer->texture.mWidth;
		Vector2<float> prev_offset = prev_tRenderer->render_offset;
		int offset = prev_offset.x + prev_font_w + 1;

		tRenderer->render_offset = Vector2<float>(offset, prev_offset.y);

	}

	return;
}

std::string TextLabel::getText()
{
	return text;
}

// Private methods
Texture TextLabel::getTextureByChar(char c)
{
	Texture font;

	// Texture cannot be found in map, so we load it
	if (texture_map.find(c) == texture_map.end())
	{
		// Getting path and color key
		std::string texture_path = getTexturePath(c);
		MapRGB* colorKey = new MapRGB;
		colorKey->green = 255;

		// Loads font texture
		font = Texture(texture_path.c_str(), RendererManager::renderer, colorKey);
		std::pair<char, Texture> pair(c, font);

		// Add to map
		texture_map.insert(pair);
	}
	else
		font = texture_map.at(c);

	return font;
}

std::string TextLabel::getTexturePath(char c)
{
	std::string path;
	path = fonts_path + c + fonts_ext;

	return path;
}

void TextLabel::init() 
{
	// Load blank texture
	MapRGB* colorKey = new MapRGB;
	colorKey->green = 255;
	std::string path = fonts_path + "space" + fonts_ext;
	Texture space_texture = Texture(path.c_str(), RendererManager::renderer, colorKey);
	
	// Create value pair
	char space = ' ';
	std::pair<char, Texture> pair(space, space_texture);
	texture_map.insert(pair);

	// Set init flag
	isInitiliazed = true;
}