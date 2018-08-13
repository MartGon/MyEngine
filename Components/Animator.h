#pragma once
#include "TextureRenderer.h"
#include <vector>
#include <string>

extern struct Frame
{
	Texture texture;
	Uint32 duration = 1;
};

extern struct Animation
{
	TextureRenderer *tRenderer;
	Uint8 id = 0;
	std::vector<Frame*> frames;
	std::string name;
	bool loop = false;
};

class Animator : public Component
{
public:

	static const int FPS;

	// Constructors
	Animator();
	Animator(const char* prefixPath, MapRGB* colorKey, TextureRenderer* tRenderer, int lastFrame, int firstFrame = 0, Uint8 id = 0);

	// Animation vector
	std::vector<Animation*> animations;

	// Attributes
	Animation *currentAnimation = nullptr;

	// Time Tracking
	int frameCount = 0;
	int currentIndex = 0;
	bool isAnimationFinished = false;

	// Override functions
	void start() override;
	void update() override;
	
	// Methods
	Animation* addAnimation(const char* prefixPath, MapRGB* colorKey, TextureRenderer* tRenderer, int lastFrame, int firstFrame = 1, Uint8 id = 0);
	void setCurrentAnimation(Animation *animation);
	void reset();

private:

	Uint8 lastID = 0;

	std::vector<Frame*> loadFrames(const char* prefixPath, MapRGB* colorKey, TextureRenderer *tRenderer, int lastFrame, int firstFrame);
	int calculateFrameDuration(int frameAmount);
	std::string getNextFramePath(const char* prefixPath, int frameNumber);
	void setAnimationID(Animation *anim, Uint8 id);
	void finishCurrentAnimation();
};