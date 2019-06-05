#pragma once

#include <unordered_map>

#include <SDL.h>

#include "GameObject.h"

enum KeyEvent
{
	UP,
	DOWN
};

struct InputStatus
{
	Uint16 input_flags = 0;
	Vector2<Uint16> mouse_pos;
};

class InputManager : public ManagerBase
{
public:

	// Member
	NetworkOwner default_owner;

	// Input State
	bool isKeyPressed(SDL_Scancode key, NetworkOwner owner);
	bool wasKeyPressed(SDL_Scancode key, NetworkOwner owner);
	bool isMousePressed(Uint8 button, NetworkOwner owner);
	bool wasMousePressed(Uint8 button, NetworkOwner owner);

	// Event State
	bool isKeyEvent(SDL_Scancode key, NetworkOwner owner, KeyEvent key_event);
	bool isMouseEvent(Uint8 button, NetworkOwner owner, KeyEvent key_event);

	// State getters and setter
	void setInputStatus(InputStatus status, NetworkOwner owner);
	InputStatus getInputStatus(NetworkOwner owner);
	void setPrevInputStatus(InputStatus status, NetworkOwner owner);
	InputStatus getPrevInputStatus(NetworkOwner owner);

	// Translation SDL
	Uint16 getFlagByScanCode(SDL_Scancode code);
	Uint16 getFlagByButton(Uint8 button);

	// Convenience
	static InputManager* get();

	// Override
	void manage() override;
private:
	std::unordered_map<NetworkOwner, InputStatus> inputMap;
	std::unordered_map<NetworkOwner, InputStatus> prev_inputMap;

	void setInputStatus(InputStatus status, NetworkOwner owner, std::unordered_map<NetworkOwner, InputStatus>& map);
	InputStatus getInputStatus(NetworkOwner owner, std::unordered_map<NetworkOwner, InputStatus>& map);
};