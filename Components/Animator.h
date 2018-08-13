#pragma once
#include "TextureRenderer.h"
#include <vector>
#include <string>

class Animator : public Component
{
public:

	static const int FPS;

	// Constructors
	Animator(const char* prefixPath, int frameAmount);
	Animator(const char* prefixPath, int frameAmount, MapRGB* colorKey, TextureRenderer* tRenderer = nullptr);

	// Attributes
	const char* prefixPath = nullptr;
	int frameAmount = 0;
	std::vector<Texture> frames;
	TextureRenderer *tRenderer = nullptr;

	// Time Tracking
	int frameDuration = 60;
	int frameCount = 0;
	int currentIndex = 0;

	void start() override;
	void update() override;

private:

	// MapRGB
	MapRGB *colorKey = nullptr;

	void loadFrames();
	int calculateFrameDuration();
	std::string getNextFramePath(int frameNumber);
};