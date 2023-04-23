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
SDL_Surface* snakeImg[14];
SDL_Texture* snakeTex[14];

struct TexObject {
  int x;
  int y;
  // Width and height is assumed to be 44x44
  SDL_Texture* texture;
};

void DrawSnake(SDL_Surface*, struct TexObject*);

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

  SDL_Texture* testTex = SDL_CreateTextureFromSurface(renderer, background);   
  
  // Create textures from the SDL_Surfaces
  for(int i = 0; i < 14; i++) {
    snakeTex[i] = SDL_CreateTextureFromSurface(renderer, snakeImg[i]);  
  }

  struct TexObject snake[3];
  snake[0].x = 0;
  snake[0].y = 0;
  snake[0].texture = snakeTex[0];

  snake[1].x = 0;
  snake[1].y = 44;
  snake[1].texture = snakeTex[5];
  
  snake[2].x = 0;
  snake[2].y = 88;
  snake[2].texture = snakeTex[10];

  SDL_Surface* ws = SDL_GetWindowSurface(window);

  SDL_Event event;
  while(doRender) {
    SDL_WaitEvent(&event);
    
    switch (event.type) {
      case SDL_QUIT:
        doRender = 0;
        break;
    }
    SDL_Rect rect = { 0, 0, 880, 880};
    SDL_RenderCopy(renderer, testTex, NULL, &rect);
    SDL_RenderPresent(renderer);
    
    DrawSnake(ws, snake);
  }

  SDL_DestroyTexture(testTex);
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

void DrawSnake(SDL_Surface* surface, struct TexObject* snake) {
    
  for(int i = 0; i < 3; i++) {
    SDL_Rect rect = { snake[i].x, snake[i].y, 44, 44 };
    SDL_RenderCopy(renderer, snake[i].texture, NULL, &rect);
    SDL_RenderPresent(renderer);
  }

}


