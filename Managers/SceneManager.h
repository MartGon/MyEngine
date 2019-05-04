#pragma once
#include "Scene.h"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	static Scene *scene;

	// Scene loading
	static void loadNextScene(Scene* s);
	static void loadScene();

	// methods
	static void quitGame();
	static bool canLoadNextScene();

private:
	// load next scene flag
	static bool load_next_scene;
	static Scene* next_scene;
};

