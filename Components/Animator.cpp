#include "Animator.h"
#include "RendererManager.h"
#include "GameObject.h"

const int Animator::FPS = 60;

Animator::Animator(const char* prefixPath, int frameAmount)
{
	this->prefixPath = prefixPath;
	this->frameAmount = frameAmount;
}

void Animator::loadFrames()
{
	std::string path;
	for (int i = 1; i <= frameAmount; i++)
	{
		path = getNextFramePath(i);
		frames.push_back(Texture(path.c_str(), tRenderer->renderer));
	}
}

std::string Animator::getNextFramePath(int frameNumber)
{
	std::string prefix(prefixPath);
	std::string suffix(".png");
	std::string path = prefix + std::to_string(frameNumber) + suffix;

	return path;
}

int Animator::calculateFrameDuration()
{
	return FPS / frameAmount;
}

void Animator::start()
{
	tRenderer = gameObject->getComponent<TextureRenderer>();

	if (!tRenderer)
	{
		printf("Animator: %s doesn't have a TextureRenderer component, creating one\n", gameObject->name.c_str());
		tRenderer = gameObject->setComponent(new TextureRenderer());
	}

	frameDuration = calculateFrameDuration();
	loadFrames();
	tRenderer->texture = frames.front();
}

void Animator::update()
{
	frameCount++;

	if (frameCount >= frameDuration)
	{
		frameCount = 0;

		if (currentIndex == (frames.size() - 1))
			currentIndex = 0;
		else
			currentIndex++;
		
		tRenderer->texture = frames.at(currentIndex);
	}
}