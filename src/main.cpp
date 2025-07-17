//SDL3
#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

// Dear ImGui
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

// C++
#include <vector>


// Application Code Here
struct MyAppState {
	bool showDemoWindow = true;
	bool showAnotherWindow = false;
	float f = 0.0f;
	int counter = 0;
};

void DrawMyImGuiApp(MyAppState* myAppState) {
	//Demo code adapted from https://github.com/ocornut/imgui/blob/5d4126876bc10396d4c6511853ff10964414c776/examples/example_sdl3_sdlrenderer3/main.cpp#L134-L169

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (myAppState->showDemoWindow)
		ImGui::ShowDemoWindow(&myAppState->showDemoWindow);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Press ESC or Q to quit the application."); // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &myAppState->showDemoWindow); // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &myAppState->showAnotherWindow);

		ImGui::SliderFloat("float", &myAppState->f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

		if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
			myAppState->counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", myAppState->counter);

		const ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (myAppState->showAnotherWindow) {
		ImGui::Begin("Another Window", &myAppState->showAnotherWindow); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			myAppState->showAnotherWindow = false;
		ImGui::End();
	}
}

//---------------------------------------------------------------------------------------------------------------------
// Program Code Here
// Only edit if you know what you're doing
#pragma region Program Code
struct ProgramState {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* surface = nullptr;
	std::vector<SDL_Rect> previousFrameWindows{};

	MyAppState myAppState;
};

bool operator==(const SDL_Rect& l, const SDL_Rect& r) {
	return l.x == r.x && l.y == r.y && l.w == r.w && l.h == r.h;
}

SDL_AppResult SDL_AppInit(void** appstate, [[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
	ProgramState* programState = new ProgramState();
	*appstate = programState;

	constexpr SDL_WindowFlags flags = SDL_WINDOW_TRANSPARENT | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP;
	if (!SDL_CreateWindowAndRenderer("Hello World", 1, 1, flags, &programState->window, &programState->renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	if (!SDL_SetRenderVSync(programState->renderer, SDL_RENDERER_VSYNC_ADAPTIVE)) {
		SDL_Log("Couldn't set renderer VSync (ADAPTIVE): %s", SDL_GetError());
		if (!SDL_SetRenderVSync(programState->renderer, 1)) {
			SDL_Log("Couldn't set renderer VSync (1): %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}
	}

	SDL_Surface* surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA8888);
	SDL_ClearSurface(surface, 0.0f, 0.0f, 0.0f, 0.0f);
	if (!SDL_SetWindowShape(programState->window, surface)) {
		SDL_Log("Couldn't set window shape: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_DestroySurface(surface);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(programState->window, programState->renderer);
	ImGui_ImplSDLRenderer3_Init(programState->renderer);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	ImGui_ImplSDL3_ProcessEvent(event);
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

SDL_AppResult SDL_AppIterate(void* appstate) {
	ProgramState* programState = static_cast<ProgramState*>(appstate);

	SDL_SetRenderDrawColor(programState->renderer, 0, 0, 0, 0);
	SDL_RenderClear(programState->renderer);

	// Start the Dear ImGui frame
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Actually draw the Dear ImGui application
	DrawMyImGuiApp(&programState->myAppState);

	// Draw rectangles around the Dear ImGui windows
	ImGui::Render();

	int w = 0, h = 0;
	SDL_GetRenderOutputSize(programState->renderer, &w, &h);
	if (programState->surface == nullptr || programState->surface->w != w || programState->surface->h != h) {
		if (programState->surface) {
			SDL_DestroySurface(programState->surface);
		}
		programState->surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA8888);
	}
	SDL_ClearSurface(programState->surface, 0.0f, 0.0f, 0.0f, 0.0f);

	std::vector<SDL_Rect> thisFrameWindows{};
	for (ImGuiContext& imGuiContext = *GImGui; const ImGuiWindow* window : imGuiContext.WindowsFocusOrder) {
		SDL_Rect rect{
			.x = static_cast<int>(window->Pos.x),
			.y = static_cast<int>(window->Pos.y),
			.w = static_cast<int>(window->Size.x),
			.h = static_cast<int>(window->Size.y),
		};
		thisFrameWindows.push_back(rect);
		SDL_FillSurfaceRect(programState->surface, &rect, 0xFFFFFF);
	}

	// Finish frame
	// Only update the shape when necessary
	if (!std::ranges::equal(programState->previousFrameWindows, thisFrameWindows)) {
		if (!SDL_SetWindowShape(programState->window, programState->surface)) {
			SDL_Log("Couldn't set window shape: %s", SDL_GetError());
			return SDL_APP_FAILURE;
		}
		programState->previousFrameWindows = std::move(thisFrameWindows);
	}

	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), programState->renderer);
	SDL_RenderPresent(programState->renderer);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	const ProgramState* programState = static_cast<ProgramState*>(appstate);
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroySurface(programState->surface);
	delete programState;
}

#pragma endregion
