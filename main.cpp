#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <iostream>

bool init();
bool loadMedia();
void close();

// Screen dimenstions
const int SCREEN_WIDTH{640};
const int SCREEN_HEIGHT{480};

SDL_Surface *gRectangle{NULL};
SDL_Window *gWindow{NULL};
SDL_Surface *gScreenSurface{NULL};
int main(int argc, char *args[]) {
  if (!init()) {
    std::cout << "failed to initialize!";
  } else {
    if (!loadMedia()) {
      std::cout << "failed to load media!";
    } else {
      SDL_BlitSurface(gRectangle, NULL, gScreenSurface, NULL);
      SDL_UpdateWindowSurface(gWindow);
      // Hack to get window to stay up
      SDL_Event e;
      bool quit = false;
      while (quit == false) {
        while (SDL_PollEvent(&e)) {
          if (e.type == quit) {
            quit = true;
          }
        }
      }
    }
  }
  close();
  return 0;
}
bool init() {
  bool success{true};
  // Initialized SDL's video subsystem
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL could not be initialized";
    success = false;
  } else {
    // Create a gWindow
    gWindow = SDL_CreateWindow("Torque Simulation", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_WIDTH, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      std::cout << "gWindow could not be created" << SDL_GetError();
      success = false;
    } else {
      /*
      Get gWindow surface
      An SDL surface is just an image data type that contains the pixels of an
      image along with all data needed to render it
      */
      gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
  }
  return success;
}

bool loadMedia() {
  bool success{true};
  gRectangle = SDL_LoadBMP("assets/Rectangle.bmp");
  if (gRectangle == NULL) {
    std::cout << "unable to load the meadia" << SDL_GetError();
    success = false;
  }
  return success;
}

void close() {
  // Deallocate surface
  SDL_FreeSurface(gRectangle);
  gRectangle = NULL;

  // Destroy window
  SDL_DestroyWindow(gWindow);
}
