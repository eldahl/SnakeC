#include <stdio.h>
#include "lib/include/SDL2/SDL.h"
#include "lib/include/SDL2/SDL_error.h"
#include "lib/include/SDL2/SDL_events.h"
#include "lib/include/SDL2/SDL_hints.h"
#include "lib/include/SDL2/SDL_rect.h"
#include "lib/include/SDL2/SDL_render.h"
#include "lib/include/SDL2/SDL_surface.h"
#include "lib/include/SDL2/SDL_video.h"

int doRender = 1;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Surface* background;
SDL_Surface* snake[14];
SDL_Texture* snakeTex[14];

void DrawBackground(SDL_Surface*);
void DrawSnake(SDL_Surface*);

struct TexObject {
  int x;
  int y;
  // Width and height is assumed to be 44x44
  SDL_Texture* texture;
};

int SDL_main(int argc, char** argv){
    
  if(SDL_CreateWindowAndRenderer(880, 880, 0, &window, &renderer) == -1) {
    printf("Failed to create renderer and window...!: ");
		return 0;
  }

  // Load images
  background = SDL_LoadBMP("res/gfx/grid.bmp");

  snake[0] = SDL_LoadBMP("res/gfx/head-n.bmp");
  snake[1] = SDL_LoadBMP("res/gfx/head-e.bmp");
  snake[2] = SDL_LoadBMP("res/gfx/head-s.bmp"); 
  snake[3] = SDL_LoadBMP("res/gfx/head-w.bmp");
  snake[4] = SDL_LoadBMP("res/gfx/body-h.bmp");
  snake[5] = SDL_LoadBMP("res/gfx/body-v.bmp");
  snake[6] = SDL_LoadBMP("res/gfx/body-ne.bmp");
  snake[7] = SDL_LoadBMP("res/gfx/body-se.bmp");
  snake[8] = SDL_LoadBMP("res/gfx/body-sw.bmp");
  snake[9] = SDL_LoadBMP("res/gfx/body-nw.bmp");
  snake[10] = SDL_LoadBMP("res/gfx/tail-n.bmp");
  snake[11] = SDL_LoadBMP("res/gfx/tail-e.bmp");
  snake[12] = SDL_LoadBMP("res/gfx/tail-s.bmp");
  snake[13] = SDL_LoadBMP("res/gfx/tail-w.bmp");

  // Background
  SDL_Texture* testTex = SDL_CreateTextureFromSurface(renderer, background);   
  
  // Create textures from the SDL_Surfaces
  for(int i = 0; i < 14; i++) {
    snakeTex[i] = SDL_CreateTextureFromSurface(renderer, snake[i]);  
  }

  SDL_Surface* ws = SDL_GetWindowSurface(window);

  SDL_Event event;
  while(doRender) {
    SDL_WaitEvent(&event);
    
    switch (event.type) {
      case SDL_QUIT:
        doRender = 0;
        break;
    }

    DrawBackground(ws);
    DrawSnake(ws);
  }

  SDL_DestroyTexture(testTex);
  // Free surfaces and textures on exit
  for(int i = 0; i < 14; i++) {
    SDL_DestroyTexture(snakeTex[i]);
    SDL_FreeSurface(snake[i]);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_VideoQuit();
  SDL_Quit();
  return 0;
}

void DrawBackground(SDL_Surface* surface) {

  SDL_Rect rect = { 0, 0, 880, 880};
  SDL_RenderCopy(renderer, testTex, NULL, &rect);
  SDL_RenderPresent(renderer);
}

void DrawSnake(SDL_Surface* surface) {
  
}


