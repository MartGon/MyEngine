#include "InputManager.h"
#include "SceneManager.h"

bool InputManager::isKeyPressed(SDL_Scancode key, NetworkOwner owner)
{
	InputStatus status = getInputStatus(owner);
	Uint16 flag = getFlagByScanCode(key);

	return status.input_flags & flag;
}

bool InputManager::isMousePressed(Uint8 button, NetworkOwner owner)
{
	InputStatus status = getInputStatus(owner);
	Uint16 flag = getFlagByButton(button);

	return status.input_flags & flag;
}

bool InputManager::wasKeyPressed(SDL_Scancode key, NetworkOwner owner)
{
	InputStatus status = getPrevInputStatus(owner);
	Uint16 flag = getFlagByScanCode(key);

	return status.input_flags & flag;
}

bool InputManager::wasMousePressed(Uint8 button, NetworkOwner owner)
{
	InputStatus status = getPrevInputStatus(owner);
	Uint16 flag = getFlagByButton(button);

	return status.input_flags & flag;
}

bool InputManager::isKeyEvent(SDL_Scancode key, NetworkOwner owner, KeyEvent key_event)
{
	if (key_event == UP)
		return !isKeyPressed(key, owner) && wasKeyPressed(key, owner);
	else if (key_event == DOWN)
		return isKeyPressed(key, owner) && !wasKeyPressed(key, owner);
}

bool InputManager::isMouseEvent(Uint8 button, NetworkOwner owner, KeyEvent key_event)
{
	if (key_event == UP)
		return !isMousePressed(button, owner) && wasMousePressed(button, owner);
	else if (key_event == DOWN)
		return isMousePressed(button, owner) && !wasMousePressed(button, owner);
}

InputStatus InputManager::getInputStatus(NetworkOwner owner)
{
	return getInputStatus(owner, inputMap);
}

void InputManager::setInputStatus(InputStatus status, NetworkOwner owner)
{
	InputStatus prev_status = getInputStatus(owner);
	setInputStatus(status, owner, inputMap);
	setPrevInputStatus(prev_status, owner);
}

InputStatus InputManager::getPrevInputStatus(NetworkOwner owner)
{
	return getInputStatus(owner, prev_inputMap);
}

void InputManager::setPrevInputStatus(InputStatus status, NetworkOwner owner)
{
	setInputStatus(status, owner, prev_inputMap);
}

Uint16 InputManager::getFlagByScanCode(SDL_Scancode code)
{
	if (code == SDL_SCANCODE_W)
		return W_KEY_PRESSED;
	else if (code == SDL_SCANCODE_S)
		return S_KEY_PRESSED;
	else if (code == SDL_SCANCODE_A)
		return A_KEY_PRESSED;
	else if (code == SDL_SCANCODE_D)
		return D_KEY_PRESSED;
	else if (code == SDL_SCANCODE_E)
		return E_KEY_PRESSED;
	else if (code == SDL_SCANCODE_Q)
		return Q_KEY_PRESSED;

	return 0;
}

Uint16 InputManager::getFlagByButton(Uint8 button)
{
	if (button == SDL_BUTTON_LEFT)
		return LEFT_MOUSE_KEY_PRESSED;
	else if (button == SDL_BUTTON_RIGHT)
		return RIGHT_MOUSE_KEY_PRESSED;

	return 0;
}

InputStatus InputManager::getInputStatus(NetworkOwner owner, std::unordered_map<NetworkOwner, InputStatus>& map)
{
	if (map.find(owner) == map.end())
		return InputStatus();
	else
		return map[owner];
}

void InputManager::setInputStatus(InputStatus status, NetworkOwner owner, std::unordered_map<NetworkOwner, InputStatus>& map)
{
	if (map.find(owner) == map.end())
		map.insert({ owner, status });
	else
		map[owner] = status;
}

void InputManager::manage()
{
	InputStatus status;

	// Save keyboard state
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_W])
		status.input_flags += W_KEY_PRESSED;
	if (keyboard[SDL_SCANCODE_A])
		status.input_flags += A_KEY_PRESSED;
	if (keyboard[SDL_SCANCODE_D])
		status.input_flags += D_KEY_PRESSED;
	if (keyboard[SDL_SCANCODE_S])
		status.input_flags += S_KEY_PRESSED;
	if (keyboard[SDL_SCANCODE_E])
		status.input_flags += E_KEY_PRESSED;
	if (keyboard[SDL_SCANCODE_Q])
		status.input_flags += Q_KEY_PRESSED;

	// Save Mouse State
	int x, y;
	const Uint32 mouse_flags = SDL_GetMouseState(&x, &y);
	if (mouse_flags & SDL_BUTTON(SDL_BUTTON_LEFT))
		status.input_flags += LEFT_MOUSE_KEY_PRESSED;
	if (mouse_flags & SDL_BUTTON(SDL_BUTTON_RIGHT))
		status.input_flags += RIGHT_MOUSE_KEY_PRESSED;

	// Get window scaler
	auto scaler = RendererManager::getScaler();

	status.mouse_pos.x = x / scaler.x;
	status.mouse_pos.y = y / scaler.y;

	setInputStatus(status, SceneManager::scene->getNetworkOwnership());
}

InputManager* InputManager::get()
{
	return SceneManager::scene->inputManager;
}