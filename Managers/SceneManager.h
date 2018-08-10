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

	static void loadScene(Scene& s);

	// methods
	static void quitGame();
};

