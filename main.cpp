#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
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
SDL_Texture *gDoorTexture{NULL};

/** handles all drawing operations for the entire window */
SDL_Renderer *gRenderer{NULL};
SDL_Texture *gTextureText{NULL};

/**For rendering font */
TTF_Font *gFont;
SDL_Surface *gSurfaceText;

constexpr double PI = 3.14159265;

using namespace std;

int main(int argc, char *args[]) {

  double Force{};
  float Length{};
  double Theta{};
  double Result{};
  double Mass{};
  double dt{};

  cout << "Pass integers for screen size(Prefered 800*600)" << '\n';
  cout << "Screen Height:";
  cin >> gScreenHeight;
  cout << "Screen Width:";
  cin >> gScreenWidth;

  cout << "Pass real numbers" << '\n';
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

  cout << "Time(Sec):";
  cin >> dt;

  double tau = Force * sin(Theta * PI / 180) * Length;
  double movemntOfInertia = 1.0 / 3 * Mass * Length * Length;
  double alpha = tau / movemntOfInertia;
  double omege = alpha * dt;

  if (!init()) {
    cout << "failed to initialize!";
  } else {
    /** creating texture */
    gDoorTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET, 10.0f, Length);

    /** Redirect all drawing commands to our texture */
    SDL_SetRenderTarget(gRenderer, gDoorTexture);

    /** Frect for gDoorTexture */
    SDL_FRect rect{gScreenWidth / 2.0f, gScreenHeight / 2.0f, Length, 3.0f};
    SDL_RenderFillRectF(gRenderer, &rect);

    /** Fills the entire 10xLength texture with this color */
    SDL_SetRenderDrawColor(gRenderer, 0, 71, 171, 1);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderTarget(gRenderer, NULL);

    /** Render the text */
    SDL_Rect posText{10, 10, 400, 100};
    // SDL_RenderCopy(gRenderer, gTextureText, NULL, posText);

    float angle{0.0f};
    SDL_FPoint center{0, rect.h / 2};
    Uint32 startTime = SDL_GetTicks();
    SDL_Event e;
    bool quit{false};

    /** Hack to get window to stay up */
    while (!quit) {
      Uint32 currentTime = SDL_GetTicks();
      float deltaTime = (currentTime - startTime) / 1000.0f;
      startTime = currentTime;

      /** SDL_PollEvent(&event) checks if an event exists in the queue */
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
          quit = true;
        }
      }

      angle += omege * deltaTime;

      SDL_SetRenderDrawColor(gRenderer, 225, 225, 225, 255);
      /** Draw the background */

      /** give the window the red color */
      SDL_RenderClear(gRenderer);

      /** Rotating */
      SDL_RenderCopyExF(gRenderer, gDoorTexture, NULL, &rect, angle, &center,
                        SDL_FLIP_NONE);

      /** Update the screen */
      SDL_RenderPresent(gRenderer);

      /** Rough cap to ~60FPS */
      SDL_Delay(16);
    }
  }
  close();
  return 0;
}

bool init() {
  bool success{true};

  /** Initializing SDL's video subsystem */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "SDL could not be initialized" << SDL_GetError();
    success = false;
  } else {
    /** Create a gWindow */
    gWindow = SDL_CreateWindow("Torque Simulation", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, gScreenWidth,
                               gScreenHeight, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
    if (gWindow == NULL) {
      cout << "Window and door could not be created" << SDL_GetError();
      success = false;
    }
  }

  /** Initializing ttf */
  if (TTF_Init() < 0) {
    cout << "TTF could not be initialized" << TTF_GetError();
    success = false;
  }
  gFont = TTF_OpenFont("assets/GoogleSans_17pt-Regular.ttf", 32);
  if (gFont == NULL) {
    cout << "Font could not be loaded" << TTF_GetError();
    success = false;
  }

  SDL_Color fg = {0, 0, 0, 225};
  SDL_Color bg = {225, 225, 225, 225};
  gSurfaceText = TTF_RenderText(gFont, "Hello world", fg, bg);

  gTextureText = SDL_CreateTextureFromSurface(gRenderer, gSurfaceText);
  return success;
}
void close() {
  /** Deallocate the memory for the surface */
  SDL_FreeSurface(gSurfaceText);

  /** Close font */
  TTF_CloseFont(gFont);

  /** Destroy texture */
  SDL_DestroyTexture(gTextureText);
  SDL_DestroyTexture(gDoorTexture);

  /** Destroy render */
  SDL_DestroyRenderer(gRenderer);

  /** Destroy window */
  SDL_DestroyWindow(gWindow);
}
