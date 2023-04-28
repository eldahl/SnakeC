#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "lib/include/SDL2/SDL.h"
#include "lib/include/SDL2/SDL_error.h"
#include "lib/include/SDL2/SDL_events.h"
#include "lib/include/SDL2/SDL_hints.h"
#include "lib/include/SDL2/SDL_keycode.h"
#include "lib/include/SDL2/SDL_rect.h"
#include "lib/include/SDL2/SDL_render.h"
#include "lib/include/SDL2/SDL_stdinc.h"
#include "lib/include/SDL2/SDL_surface.h"
#include "lib/include/SDL2/SDL_video.h"

int doRender = 1;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Surface* background;
SDL_Surface* snakeImg[14];
SDL_Texture* snakeTex[14];

int grid_size = 44;

// Width and height is assumed to be 44x44
struct TexObject {
  int x;
  int y;
  SDL_Texture* texture;
};

// Holds the snake body parts
struct TexObject snake[400];
size_t snakeSize;

// Surface to render to, snake parts array, length of snake array
void DrawSnake(SDL_Surface*, struct TexObject*, size_t);

// delta x, delta y, snake parts array, length of snake array
int MoveSnake(int dx, int dy, struct TexObject*, size_t);

int SDL_main(int argc, char** argv){
    
  if(SDL_CreateWindowAndRenderer(880, 880, 0, &window, &renderer) == -1) {
    printf("Failed to create renderer and window...!: ");
		return 0;
  }

  // Load images
  background = SDL_LoadBMP("res/gfx/grid.bmp");

  snakeImg[0] = SDL_LoadBMP("res/gfx/head-n.bmp");
  snakeImg[1] = SDL_LoadBMP("res/gfx/head-e.bmp");
  snakeImg[2] = SDL_LoadBMP("res/gfx/head-s.bmp"); 
  snakeImg[3] = SDL_LoadBMP("res/gfx/head-w.bmp");
  snakeImg[4] = SDL_LoadBMP("res/gfx/body-h.bmp");
  snakeImg[5] = SDL_LoadBMP("res/gfx/body-v.bmp");
  snakeImg[6] = SDL_LoadBMP("res/gfx/body-ne.bmp");
  snakeImg[7] = SDL_LoadBMP("res/gfx/body-se.bmp");
  snakeImg[8] = SDL_LoadBMP("res/gfx/body-sw.bmp");
  snakeImg[9] = SDL_LoadBMP("res/gfx/body-nw.bmp");
  snakeImg[10] = SDL_LoadBMP("res/gfx/tail-n.bmp");
  snakeImg[11] = SDL_LoadBMP("res/gfx/tail-e.bmp");
  snakeImg[12] = SDL_LoadBMP("res/gfx/tail-s.bmp");
  snakeImg[13] = SDL_LoadBMP("res/gfx/tail-w.bmp");

  SDL_Texture* backgroundTex = SDL_CreateTextureFromSurface(renderer, background);   
  
  // Create textures from the SDL_Surfaces
  for(int i = 0; i < 14; i++) {
    snakeTex[i] = SDL_CreateTextureFromSurface(renderer, snakeImg[i]);  
  }

  snake[0].x = 0;
  snake[0].y = 0;
  snake[0].texture = snakeTex[0];
  snake[1].x = 0;
  snake[1].y = 1;
  snake[1].texture = snakeTex[5];
  snake[2].x = 0;
  snake[2].y = 2;
  snake[2].texture = snakeTex[10];
  snakeSize = 3;

  SDL_Surface* ws = SDL_GetWindowSurface(window);

  SDL_Event event;
  while(doRender) {
    SDL_WaitEvent(&event);
    
    switch (event.type) {
      case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_UP) {
          MoveSnake(0, -1, snake, snakeSize);
        }
        if(event.key.keysym.sym == SDLK_DOWN) {
          MoveSnake(0, 1, snake, snakeSize);
        }
        if(event.key.keysym.sym == SDLK_LEFT) {
          MoveSnake(-1, 0, snake, snakeSize);
        }
        if(event.key.keysym.sym == SDLK_RIGHT) {
          MoveSnake(1, 0, snake, snakeSize);
        }
        break;

      case SDL_QUIT:
        doRender = 0;
        break;
    }
    SDL_Rect rect = { 0, 0, 880, 880};
    SDL_RenderCopy(renderer, backgroundTex, NULL, &rect);
    SDL_RenderPresent(renderer);
    
    DrawSnake(ws, snake, snakeSize);
  }

  SDL_DestroyTexture(backgroundTex);
  // Free surfaces and textures on exit
  for(int i = 0; i < 14; i++) {
    SDL_DestroyTexture(snakeTex[i]);
    SDL_FreeSurface(snakeImg[i]);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_VideoQuit();
  SDL_Quit();
  return 0;
}

void DrawSnake(SDL_Surface* surface, struct TexObject* snake, size_t sSize) {

  for(int i = 0; i < sSize; i++) {
    SDL_Rect rect = { snake[i].x * grid_size, snake[i].y * grid_size, 44, 44 };
    SDL_RenderCopy(renderer, snake[i].texture, NULL, &rect);
    SDL_RenderPresent(renderer);
  }

}

// Returns -1 for snake biting itself, 1 for valid movement
int MoveSnake(int dx, int dy, struct TexObject* snake, size_t sSize) {
  
  // Check if next head position is valid
  int newX = snake[0].x + dx;
  int newY = snake[0].y + dy;
  
  for(int i = 0; i < sSize; i++) {
    if(snake[i].x == newX && snake[i].y == newY) {
      return -1;
    }
  }
  
  // Iterate through and 
  //    - Move each part of the snake
  //    - Set each part of the snake to the correct texture
  //for(int i = 0; i < sSize)
  snake[0].x = newX;
  snake[0].y = newY;
  return 1;


}
