#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <cmath>
#include <cstddef>
#include <iostream>

bool init();
bool loadDoor();
void close();
double pixelToMeter(double pixels);

// Screen dimenstions
int gScreenWidth{640};
int gScreenHeight{480};

SDL_Surface *gRectangle{NULL};
SDL_Window *gWindow{NULL};
SDL_Surface *gScreenSurface{NULL};
SDL_Renderer *gDoor{NULL};

double Force{1};
double Length{1};
double Theta{30};
double Result;
constexpr double PI = 3.14159265;

using namespace std;

int main(int argc, char *args[]) {
  cout << "Pass integers" << '\n';
  cout << "Screen Height:";
  cin >> gScreenHeight;
  cout << "Screen Width:";
  cin >> gScreenWidth;

  cout << "Pass real number" << '\n';
  cout << "Force(Newton):";
  cin >> Force;
  cout << "Length(Meter):";
  cin >> Length;
  cout << "Theta(Degree):";
  cin >> Theta;

  Result = Force * sin(Theta * PI / 180) * Length;

  if (!init()) {
    cout << "failed to initialize!";
  } else {
    /** What blitting does is take a source surface and stamps a copy of it
    onto the destination surface */

    SDL_FRect dest{static_cast<float>(gScreenWidth / 2.0 + 1),
                   static_cast<float>(gScreenHeight / 2.0 + 1),
                   static_cast<float>(pixelToMeter(Length)),
                   static_cast<float>(pixelToMeter(20.0f))};
    SDL_SetRenderDrawColor(gDoor, 225, 0, 0, 225);
    SDL_RenderClear(gDoor);
    SDL_SetRenderDrawColor(gDoor, 225, 225, 225, 225);
    SDL_RenderFillRectF(gDoor, &dest);
    SDL_RenderPresent(gDoor);

    /** Update the surface */
    SDL_UpdateWindowSurface(gWindow);

    /** Hack to get window to stay up */
    SDL_Event e;
    bool quit{false};
    while (!quit) {
      /** SDL_PollEvent(&event) checks if an event exists in the queue */
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
    }
  }
  close();
  return 0;
}

bool init() {
  bool success{true};
  /** Initialized SDL's video subsystem */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "SDL could not be initialized";
    success = false;
  } else {
    /** Create a gWindow */
    gWindow = SDL_CreateWindow("Torque Simulation", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, gScreenWidth,
                               gScreenWidth, SDL_WINDOW_SHOWN);
    gDoor = SDL_CreateRenderer(gWindow, -1, 0);
    if (gWindow == NULL && gDoor == NULL) {
      cout << "Window and door could not be created" << SDL_GetError();
      success = false;
    } else {
      /** Get gWindow surface An SDL surface is just an image data type that
      contains the pixels of an image along with all data needed to render it */
      gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
  }
  return success;
}

bool loadMedia() {
  bool success{true};
  gRectangle = SDL_LoadBMP("assets/Rectangle.bmp");

  if (gRectangle == NULL) {
    cout << "unable to load the meadia" << SDL_GetError();
    success = false;
  }
  return success;
}
double pixelToMeter(double pixels) {
  /** 1 pixel = 2 meters */
  double result{pixels / 2};
  return result;
}
void close() {
  /** Deallocate surface */
  SDL_FreeSurface(gRectangle);
  gRectangle = NULL;
  /** Destroy render */
  SDL_DestroyRenderer(gDoor);
  /** Destroy window */
  SDL_DestroyWindow(gWindow);
}
