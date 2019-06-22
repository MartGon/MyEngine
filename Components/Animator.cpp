#include "Animator.h"
#include "RendererManager.h"
#include "GameObject.h"

const int Animator::FPS = 60;

Animator::Animator()
{

}

Animator::Animator(const char* prefixPath, MapRGB* colorKey, TextureRenderer* tRenderer, int lastFrame, int firstFrame, Uint8 id)
{
	addAnimation(prefixPath, colorKey, tRenderer, lastFrame, firstFrame, id);
	if (!currentAnimation)
		setCurrentAnimation(animations.front());
}

Animation* Animator::addAnimation(const char* prefixPath, MapRGB* colorKey, TextureRenderer* tRenderer, int lastFrame, int firstFrame, Uint8 id)
{
	Animation *animation = new Animation();

	animation->name = prefixPath;
	animation->frames = loadFrames(prefixPath, colorKey, tRenderer, lastFrame, firstFrame);
	animation->tRenderer = tRenderer;
	setAnimationID(animation, id);

	animations.push_back(animation);

	return animation;
}

Animation* Animator::addAnimation(Animation* anim)
{
	setAnimationID(anim, id);

	animations.push_back(anim);

	return anim;
}

void Animator::setCurrentAnimation(Animation *animation)
{
	currentAnimation = animation;
	
	// Init current animation
	currentAnimation->tRenderer->texture = currentAnimation->frames.front()->texture;

	// Reset
	reset();
}

std::vector<Frame*> Animator::loadFrames(const char* prefixPath, MapRGB* colorKey, TextureRenderer *tRenderer, int lastFrame, int firstFrame)
{
	std::string path;
	std::vector<Frame*> frames;
	Frame *frame = nullptr;

	int frameAmount = lastFrame - firstFrame + 1;

	if (frameAmount <= 0)
	{
		printf("LastFrame bigger than first frame\n");
		return frames;
	}

	int frameDuration = calculateFrameDuration(frameAmount);

	for (int i = firstFrame; i <= lastFrame; i++)
	{
		frame = new Frame();
		path = getNextFramePath(prefixPath, i);
		frame->texture = Texture(path.c_str(), tRenderer->renderer, colorKey);
		frame->duration = frameDuration;
		frames.push_back(frame);
	}

	return frames;
}

std::string Animator::getNextFramePath(const char* prefixPath, int frameNumber)
{
	std::string prefix(prefixPath);
	std::string suffix(".png");
	std::string path = prefix + std::to_string(frameNumber) + suffix;

	return path;
}

int Animator::calculateFrameDuration(int frameAmount)
{
	return FPS / frameAmount;
}

void Animator::start()
{

}

void Animator::update()
{
	// If there's no animation, return
	if (!currentAnimation)
		return;

	// If the animation is finished, there's nothing to update
	if (isAnimationFinished)
		return;

	frameCount += frame_speed;
	if (frameCount >= currentAnimation->frames.at(currentIndex)->duration)
	{
		frameCount = 0;

		// Check if this is last frame
		if (currentIndex == (currentAnimation->frames.size() - 1))
		{
			// Check if it can loop
			if (currentAnimation->loop)
				// Restar
				currentIndex = 0;
			// Notify that animation has finished otherwise
			else
			{
				// Finish
				finishCurrentAnimation();
				return;
			}
		}
		// Choose next frame
		else
			currentIndex++;

		// Notify frame change
		gameObject->beforeAnimationFrame(currentAnimation, currentIndex);

		// Update frame to play
		currentAnimation->tRenderer->texture = currentAnimation->frames.at(currentIndex)->texture;
	}
}

void Animator::destroy()
{
	for (auto anim : animations)
	{
		for (auto f : anim->frames)
		{
			//f->texture.free();

			delete f;
		}
		delete anim;
	}
}

void Animator::setAnimationID(Animation *anim, Uint8 id)
{
	if (id)
		anim->id = id;
	else
		anim->id = lastID++;
}

void Animator::finishCurrentAnimation()
{
	isAnimationFinished = true;
	gameObject->onAnimationFinished(currentAnimation);
}

void Animator::reset()
{
	frameCount = 0;
	isAnimationFinished = false;
	currentIndex = 0;

	// Set to initial texture
	if(currentAnimation)
		currentAnimation->tRenderer->texture = currentAnimation->frames.at(currentIndex)->texture;
}