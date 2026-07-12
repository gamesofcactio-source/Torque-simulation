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
int gScreenWidth{};
int gScreenHeight{};

SDL_Surface *gRectangle{NULL};
SDL_Window *gWindow{NULL};
SDL_Surface *gScreenSurface{NULL};
SDL_Renderer *gDoor{NULL};

double Force{};
float Length{};
double Theta{};
double Result{};
double Mass{};
double deltaTime{};
double Alpha{};
double Omega{};
constexpr double PI = 3.14159265;

using namespace std;

int main(int argc, char *args[]) {
  cout << "Pass integers for screen size(Prefered 800*600)" << '\n';
  cout << "Screen Height:";
  cin >> gScreenHeight;
  cout << "Screen Width:";
  cin >> gScreenWidth;
  if (gScreenWidth > 1920 || gScreenHeight > 1080) {
    return -1;
    cout << "max screen dimenstions are 1920*1080";
  }
  cout << "Pass real number" << '\n';
  cout << "Force(Newton):";
  cin >> Force;
  cout << "Length(Meter):";
  cin >> Length;
  if (Length > gScreenWidth) {
    cout << "screen width can't greater than width";
    return -1;
  }
  cout << "Theta(Degree):";
  cin >> Theta;
  cout << "Mass(Kg)";
  cin >> Mass;
  Result = Force * sin(Theta * PI / 180) * Length;
  if (!init()) {
    cout << "failed to initialize!";
  } else {
    /** What blitting does is take a source surface and stamps a copy of it
    onto the destination surface */

    SDL_FRect dest{gScreenWidth / 2.0f, gScreenHeight / 2.0f, Length, 20.0f};
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
                               gScreenHeight, SDL_WINDOW_SHOWN);
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
