#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <cmath>
#include <cstddef>
#include <iostream>

bool init();
bool loadDoor();
void close();
void rotate(int theta);

// Screen dimenstions
int gScreenWidth{};
int gScreenHeight{};

SDL_Surface *gRectangle{NULL};
SDL_Window *gWindow{NULL};
SDL_Surface *gScreenSurface{NULL};
SDL_Renderer *gDoor{NULL};
SDL_RendererFlip flip;

double Force{};
float Length{};
double Theta{};
double Result{};
double Mass{};
double dt{20};
double Alpha{};
double Omega{5};
constexpr double PI = 3.14159265;

using namespace std;

int main(int argc, char *args[]) {
  cout << "Pass integers for screen size(Prefered 800*600)" << '\n';
  cout << "Screen Height:";
  cin >> gScreenHeight;

  cout << "Screen Width:";
  cin >> gScreenWidth;

  cout << "Pass real number" << '\n';
  cout << "Force(Newton):";
  cin >> Force;

  cout << "Length(Meter):";
  cin >> Length;

  if (Length > gScreenWidth / 2.0 && Length > gScreenHeight / 2.0) {
    cout << "length can't greater than width/2 and height/2. to insure that "
            "the object stay inside the screen";
    return -1;
  }

  cout << "Theta(Degree):";
  cin >> Theta;

  cout << "Mass(Kg):";
  cin >> Mass;

  double Tau = Force * sin(Theta * PI / 180) * Length;
  if (!init()) {
    cout << "failed to initialize!";
  } else {
    /** Hack to get window to stay up */
    SDL_Texture *gDoorTexture =
        SDL_CreateTexture(gDoor, SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_TARGET, 10.0f, Length);
    SDL_SetRenderTarget(gDoor, gDoorTexture);
    SDL_SetRenderDrawColor(gDoor, 0, 0, 0, 0);
    SDL_RenderClear(gDoor);
    SDL_SetRenderDrawColor(gDoor, 225, 225, 225, 225);
    SDL_FRect rect{gScreenWidth / 2.0f, gScreenHeight / 2.0f, Length, 3.0f};
    SDL_SetRenderTarget(gDoor, NULL);

    SDL_RenderFillRectF(gDoor, &rect);

    float angle{0};
    angle += Omega * dt;
    SDL_FPoint center{0, rect.h / 2};
    Uint32 startTime = SDL_GetTicks();
    SDL_Event e;
    bool quit{false};
    while (!quit) {
      Uint32 currentTime = SDL_GetTicks();
      float deltaTime = (currentTime - startTime) / 1000.0f;
      /** SDL_PollEvent(&event) checks if an event exists in the queue */
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }
      SDL_SetRenderDrawColor(gDoor, 30, 30, 30, 255);
      SDL_RenderClear(gDoor);
      SDL_RenderCopyExF(gDoor, gDoorTexture, NULL, &rect, angle, &center,
                        SDL_FLIP_NONE);
      SDL_RenderPresent(gDoor);
      angle += Omega * deltaTime;
      startTime = currentTime;
      SDL_Delay(16);

      /** Update the surface */
      SDL_UpdateWindowSurface(gWindow);
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
    if (gWindow == NULL) {
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
void rotate(int theta) {
  SDL_Point center;
  center.x = gScreenWidth / 2;
  center.y = gScreenHeight / 2;

  float w{5.0f};
  float l{Length};

  // corners
  SDL_FPoint p[4] = {
      {-w / 2, -l / 2}, {w / 2, -l / 2}, {w / 2, l / 2}, {-w / 2, l / 2}};

  for (int i{0}; i < 4; i++) {
    float x = p[i].x;
    float y = p[i].y;

    p[i].x = center.x + cos(theta) * x - sin(theta) * y;
    p[i].y = center.y + sin(theta) * x - cos(theta) * y;
  }
  SDL_RenderDrawLine(gDoor, p[0].x, p[0].y, p[1].x, p[1].y);
  SDL_RenderDrawLine(gDoor, p[1].x, p[1].y, p[2].x, p[2].y);
  SDL_RenderDrawLine(gDoor, p[2].x, p[2].y, p[3].x, p[3].y);
  SDL_RenderDrawLine(gDoor, p[3].x, p[3].y, p[0].x, p[0].y);
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
