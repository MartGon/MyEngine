#pragma once
#include "TextureRenderer.h"
#include <vector>
#include <string>

struct Frame
{
	Texture texture;
	Uint32 duration = 1;
};

struct Animation
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
	Animator(const char* prefixPath, MapRGB* colorKey, TextureRenderer* tRenderer, int lastFrame, int firstFrame = 1, Uint8 id = 0);
	

	// Animation vector
	std::vector<Animation*> animations;

	// Attributes
	Animation *currentAnimation = nullptr;

	// Time Tracking
	float frame_speed = 1.f;
	double frameCount = 0;
	int currentIndex = 0;
	bool isAnimationFinished = false;

	// Override functions
	void start() override;
	void update() override;
	void destroy() override;
	
	// Methods
	Animation* addAnimation(const char* prefixPath, MapRGB* colorKey, TextureRenderer* tRenderer, int lastFrame, int firstFrame = 1, Uint8 id = 0);
	Animation* addAnimation(Animation* anim);
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