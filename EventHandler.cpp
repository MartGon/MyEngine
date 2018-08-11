#include "EventHandler.h"
#include "SceneManager.h"

// Constructors

EventHandler::EventHandler()
{
	SceneManager::scene->addEventHandler(this);
}