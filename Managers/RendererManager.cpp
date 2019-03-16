#include "RendererManager.h"
#include "SceneManager.h"

Texture RendererManager::nullTexture;

SDL_Renderer* RendererManager::renderer = nullptr;

// GBA Res: 240 x 160

int RendererManager::nativeWidthResolution = 480;
int RendererManager::nativeHeightResolution = 320;

// Cam position
Vector2<int> RendererManager::camera_position = Vector2<int>(120, 80);

RendererManager::RendererManager()
{

}

void RendererManager::setNativeResolution(int width, int height)
{
	nativeHeightResolution = width;
	nativeWidthResolution = height;
}

Vector2<float> RendererManager::getScaler()
{
	Vector2<float> scaler(float(WINDOW_WIDTH) / float(nativeWidthResolution), float(WINDOW_HEIGHT) / float(nativeHeightResolution));
	return scaler;
}

Vector2<int> RendererManager::getNativeResolution()
{
	return Vector2<int>(nativeWidthResolution, nativeHeightResolution);
}

bool RendererManager::lesser(TextureRenderer* tr1, TextureRenderer* tr2)
{
	return (tr1->getLayer() < tr2->getLayer());
}

void RendererManager::init()
{
	if (!renderer)
	{
		printf("Renderer was not set before initialization\n");
		return;
	}

	nullTexture = Texture("null.png", renderer);
}

// Camera

void RendererManager::setCameraPosition(Vector2<int> camera_pos, Vector2<int> level_boundaries)
{
	if (camera_pos.x < 0)
		camera_position.x = 0;
	else if (camera_pos.x > (level_boundaries.x - nativeWidthResolution))
		camera_position.x = (level_boundaries.x - nativeWidthResolution);
	else
		camera_position.x = camera_pos.x;

	if (camera_pos.y < 0)
		camera_position.y = 0;
	else if (camera_pos.y > (level_boundaries.y - nativeHeightResolution))
		camera_position.y = (level_boundaries.y - nativeHeightResolution);
	else
		camera_position.y = camera_pos.y;

	// Set
}

Vector2<int> RendererManager::getCameraPosition()
{
	return camera_position;
}

// Non-static methods

void RendererManager::onAddComponent(TextureRenderer* tRenderer)
{
	std::sort(components.begin(), components.end(), lesser);
}

void RendererManager::manage()
{
	// Draw sprites
	for (int i = 0; i < components.size(); i++)
	{
		TextureRenderer* tRenderer = components.at(i);
		if (tRenderer->isEnabled)
			// Needs to check if gameobject is null (SetComponent was not called)
			if(tRenderer->gameObject->isActive)
				tRenderer->render();
	}

	// Draw Colliders which are set to be debugged.
	Manager<Collider*>* collider_manager = SceneManager::scene->getManager<Collider*>();

	if (collider_manager)
	{
		std::vector<Collider*> colliders = collider_manager->getComponents();

		for (auto &collider : colliders)
		{
			if (collider->gameObject->isActive)
				if(collider->isEnabled)
					if (collider->debug)
						collider->drawCollisionBoundaries();
		}
	}
}

// TODO - Poner layer a private. Hacer metodo setLayer que sortea el array cuando se cambia el layer
// TODO - La layer se tiene en cuenta una vez que el objeto esta creado, por lo que es necesario ordenar el array en otro momento. Solucion: Hacer metodo start en la clase manager