#pragma once
#include "Scene.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	static Scene *scene;

	// quit flag
	static bool quit;

	// Scene loading
	static void loadNextScene(Scene* s);
	static void loadScene();

	// methods
	static void quitGame();
	static bool canLoadNextScene();

private:
	static Scene* next_scene;
};

