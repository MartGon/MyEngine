#include "TextLabel.h"

std::unordered_map<char, Texture> TextLabel::texture_map = std::unordered_map<char, Texture>();

// Constructor
TextLabel::TextLabel() 
{
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
		tRenderer->isEnabled = true;
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
	fixCharPositions();

	// Set text color
	setTextColor(text_color);

	return;
}

std::string TextLabel::getText()
{
	return text;
}

void TextLabel::setTextScale(Vector2<float> scale)
{
	transform.scale = scale;

	// Fix positions
	fixCharPositions();
}

Vector2<float> TextLabel::getTextScale()
{
	return transform.scale;
}

void TextLabel::setTextColor(MapRGB color)
{
	text_color = color;

	for (auto tRenderer : font_tRenderers)
	{
		tRenderer->texture.color_mod = color;
	}
}

Vector2<float> TextLabel::getNextCharPos(int index)
{
	Vector2<float> v_offset(-1, -1);

	if (font_tRenderers.empty())
		return v_offset;

	if (index == 0)
		return font_tRenderers.at(0)->render_offset;

	TextureRenderer* prev_tRenderer = font_tRenderers.at(index - 1);
	int prev_font_w = prev_tRenderer->texture.mWidth * transform.scale.x;
	Vector2<float> prev_offset = prev_tRenderer->render_offset;
	int offset = prev_offset.x + prev_font_w + 1 * transform.scale.x;

	v_offset = Vector2<float>( offset, prev_offset.y );

	return v_offset;
}


Vector2<float> TextLabel::getDimensions()
{
	Vector2<float> i_dim;

	// Get any texture height
	if (!font_tRenderers.empty())
	{
		if (TextureRenderer* tRenderer = font_tRenderers.back())
		{
			Texture texture = tRenderer->texture;

			float height = texture.mHeight * transform.scale.y;
			i_dim = Vector2<float>(getNextCharPos(text.size()).x, height);
		}
	}

	return i_dim;
}

// Private methods
void TextLabel::fixCharPositions()
{
	for (int i = 1; i < font_tRenderers.size(); i++)
	{
		TextureRenderer* tRenderer = font_tRenderers.at(i);
		tRenderer->render_offset = getNextCharPos(i);
	}
}

Texture TextLabel::getTextureByChar(char c)
{
	Texture font;

	// Texture cannot be found in map, so we load it
	if (texture_map.find(c) == texture_map.end())
	{
		// Getting path and color key
		std::string texture_path = getTexturePath(c);
		MapRGB colorKey;
		colorKey.green = 255;

		// Loads font texture
		font = Texture(texture_path.c_str(), RendererManager::renderer, &colorKey);
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
	std::string name = "";

	switch (c)
	{
	case '.':
		name = "dot";
		break;
	case ':':
		name = "colon";
		break;
	case '!':
		name = "Exclamation";
		break;
	case '/':
		name = "Slash";
		break;
	default:
		name += c;
		break;
	}

	std::string path = fonts_path + name + fonts_ext;

	return path;
}

void TextLabel::init() 
{
	// Load blank texture
	MapRGB colorKey;
	colorKey.green = 255;
	std::string path = fonts_path + "space" + fonts_ext;
	Texture space_texture = Texture(path.c_str(), RendererManager::renderer, &colorKey);
	
	// Create value pair
	char space = ' ';
	std::pair<char, Texture> pair(space, space_texture);
	texture_map.insert(pair);
}