#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

bool init();
bool loadDoor();
void close();
void rotate(int theta);

// Screen dimenstions
const int gScreenWidth{600};
const int gScreenHeight{800};

SDL_Window *gWindow{NULL};
SDL_Texture *gRodTexture{NULL};

/** handles all drawing operations for the entire window */
SDL_Renderer *gRenderer{NULL};

/**For rendering font */
TTF_Font *gFont;
SDL_Surface *gSurfaceText1{NULL};
SDL_Texture *gTextureText1{NULL};

SDL_Surface *gSurfaceText2{NULL};
SDL_Texture *gTextureText2{NULL};

SDL_Surface *gSurfaceText3{NULL};
SDL_Texture *gTextureText3{NULL};

constexpr double PI = 3.14159265;

using namespace std;

int main(int argc, char *args[]) {

  double Force{};
  float Length{};
  double Theta{};
  double Result{};
  double Mass{};
  double dt{};

  cout << "Pass real numbers" << '\n';
  cout << "Force(Newton):";
  cin >> Force;

  cout << "Length(Meter):";
  cin >> Length;

  cout << "Theta(Degree):";
  cin >> Theta;

  cout << "Mass(Kg):";
  cin >> Mass;

  cout << "Time(Sec):";
  cin >> dt;

  double tau = Force * sin(Theta * PI / 180) * Length;
  double movemntOfInertia = 1.0 / 3 * Mass * Length * Length;
  double alpha = tau / movemntOfInertia;
  double omege{};
  omege += alpha * dt;

  if (!init()) {
    cout << "failed to initialize!";
  } else {
    /** creating texture */
    gRodTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET, 10.0f, Length);

    /** Redirect all drawing commands to our texture */
    SDL_SetRenderTarget(gRenderer, gRodTexture);

    /** Frect for gRodTexture */
    SDL_FRect rect{gScreenWidth / 2.0f, gScreenHeight / 2.0f, Length, 3.0f};
    SDL_RenderFillRectF(gRenderer, &rect);

    /** Fills the entire 10xLength texture with this color */
    SDL_SetRenderDrawColor(gRenderer, 0, 71, 171, 1);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderTarget(gRenderer, NULL);

    /** Render the text */
    SDL_Color bg = {0, 0, 0, 225};
    SDL_Color fg = {225, 225, 225, 225};
    vector<string> lines = {"Torque: " + to_string(tau),
                            "Angular velocity: " + to_string(omege),
                            "Angular accelaration: " + to_string(alpha)};
    gSurfaceText1 = TTF_RenderText(gFont, lines[0].c_str(), fg, bg);
    gSurfaceText2 = TTF_RenderText(gFont, lines[1].c_str(), fg, bg);
    gSurfaceText3 = TTF_RenderText(gFont, lines[2].c_str(), fg, bg);

    gTextureText1 = SDL_CreateTextureFromSurface(gRenderer, gSurfaceText1);
    gTextureText2 = SDL_CreateTextureFromSurface(gRenderer, gSurfaceText2);
    gTextureText3 = SDL_CreateTextureFromSurface(gRenderer, gSurfaceText3);

    int line1height{};
    int line1Width{};
    TTF_SizeText(gFont, lines[0].c_str(), &line1Width, &line1height);
    int line2height{};
    int line2Width{};
    TTF_SizeText(gFont, lines[1].c_str(), &line2Width, &line2height);
    int line3height{};
    int line3Width{};
    TTF_SizeText(gFont, lines[2].c_str(), &line3Width, &line3height);

    SDL_Rect posLine1{20, 20, line1Width, line1height};
    SDL_Rect posLine2{20, posLine1.h + 20, line2Width, line2height};
    SDL_Rect posLine3{20, posLine1.h + 40, line3Width, line3height};

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
        if (e.key.keysym.sym == SDLK_ESCAPE) {
          quit = true;
        }
      }

      angle += omege * deltaTime;

      SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 225);
      /** Draw the background */

      /** give the window the red color */
      SDL_RenderClear(gRenderer);

      /** Rotating */
      SDL_RenderCopyExF(gRenderer, gRodTexture, NULL, &rect, angle, &center,
                        SDL_FLIP_NONE);

      SDL_RenderCopy(gRenderer, gTextureText1, NULL, &posLine1);
      SDL_RenderCopy(gRenderer, gTextureText2, NULL, &posLine2);
      SDL_RenderCopy(gRenderer, gTextureText3, NULL, &posLine3);

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
  gFont = TTF_OpenFont("assets/GoogleSans_17pt-Regular.ttf", 13);
  if (gFont == NULL) {
    cout << "Font could not be loaded" << TTF_GetError();
    success = false;
  }
  /** Initializing keyboard input */

  return success;
}
void close() {
  /** Deallocate the memory for the surface */
  SDL_FreeSurface(gSurfaceText1);
  SDL_FreeSurface(gSurfaceText2);
  SDL_FreeSurface(gSurfaceText3);

  /** Close font */
  TTF_CloseFont(gFont);

  /** Destroy texture */
  SDL_DestroyTexture(gRodTexture);
  SDL_DestroyTexture(gTextureText1);
  SDL_DestroyTexture(gTextureText2);
  SDL_DestroyTexture(gTextureText3);

  /** Destroy render */
  SDL_DestroyRenderer(gRenderer);

  /** Destroy window */
  SDL_DestroyWindow(gWindow);
}
