#include "SceneManager.h"

Scene* SceneManager::scene = nullptr;
Scene* SceneManager::next_scene = nullptr;

bool SceneManager::quit = false;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::loadNextScene(Scene* s)
{
	next_scene = s;
}

void SceneManager::quitGame()
{
	quit = true;
}

bool SceneManager::canLoadNextScene()
{
	return next_scene;
}

void SceneManager::loadScene() 
{
	if (scene)
	{
		scene->destroy();
		delete scene;
	}

	// Set next scene as current scene
	scene = next_scene;

	// Init scene
	scene->loadMedia();
	scene->start();

	// Set next_scene to null
	next_scene = nullptr;
}