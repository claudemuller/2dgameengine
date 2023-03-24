#ifndef KEY_PRESSED_EVENT_H
#define KEY_PRESSED_EVENT_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class KeyPressedEvent: public Event {
public:
	SDL_Keycode symbol;

	KeyPressedEvent(SDL_Keycode symbol): symbol(symbol) {}
};

#endif // KEY_PRESSED_EVENT_H
