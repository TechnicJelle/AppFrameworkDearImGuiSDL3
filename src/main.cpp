#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>


struct MyAppState {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
};

SDL_AppResult SDL_AppInit(void** appstate, [[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	MyAppState* myAppState = new MyAppState();
	*appstate = myAppState;

	constexpr SDL_WindowFlags flags = SDL_WINDOW_TRANSPARENT | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALWAYS_ON_TOP;
	if (!SDL_CreateWindowAndRenderer("Hello World", 1, 1, flags, &myAppState->window, &myAppState->renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_Surface* surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA8888);
	if (!SDL_SetWindowShape(myAppState->window, surface)) {
		SDL_Log("Couldn't set window shape: %s", SDL_GetError());
		SDL_DestroySurface(surface);
		return SDL_APP_FAILURE;
	}
	SDL_DestroySurface(surface);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	MyAppState* myAppState = static_cast<MyAppState*>(appstate);
	switch (event->type) {
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_QUIT:
			switch (event->key.key) {
				case SDLK_ESCAPE:
				case SDLK_Q:
					return SDL_APP_SUCCESS;
				default: break;
			}
		default: break;
	}
	return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate) {
	MyAppState* myAppState = static_cast<MyAppState*>(appstate);

	/* Center the message and scale it up */
	int w = 0, h = 0;
	SDL_GetRenderOutputSize(myAppState->renderer, &w, &h);
	constexpr float scale = 4.0f;
	SDL_SetRenderScale(myAppState->renderer, scale, scale);
	const char* message = "Hello World!";
	const float x = (static_cast<float>(w) / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
	const float y = (static_cast<float>(h) / scale - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

	SDL_Surface* surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA8888);
	constexpr int div = 3;
	const SDL_Rect aroundText = {
		w / div * 1, h / div * 1,
		w / div * (div-2), h / div * (div-2),
	};
	SDL_ClearSurface(surface, 0.0f, 0.0f, 0.0f, 0.0f);
	SDL_FillSurfaceRect(surface, &aroundText, SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_GetWindowPixelFormat(myAppState->window)), nullptr, 255, 255, 255, 255));
	if (!SDL_SetWindowShape(myAppState->window, surface)) {
		SDL_Log("Couldn't set window shape: %s", SDL_GetError());
		SDL_DestroySurface(surface);
		return SDL_APP_FAILURE;
	}
	SDL_DestroySurface(surface);

	/* Draw the message */
	SDL_SetRenderDrawColor(myAppState->renderer, 0, 0, 0, 255);
	SDL_RenderClear(myAppState->renderer);
	SDL_SetRenderDrawColor(myAppState->renderer, 255, 255, 255, 255);
	SDL_RenderDebugText(myAppState->renderer, x, y, message);
	SDL_RenderPresent(myAppState->renderer);

	return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	MyAppState* myAppState = static_cast<MyAppState*>(appstate);
	delete myAppState;
}
