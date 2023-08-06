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

#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

int doGameLoop = 1;

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Surface* apple;
SDL_Texture* appleTex;

SDL_Surface* background;
SDL_Texture* backgroundTex;

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
struct TexObject food;

/* ------ Functions ------ */
// Surface to render to, snake parts array, length of snake array
void DrawSnake(SDL_Surface*, struct TexObject*, size_t);
// Checks for snake head collisions
int CheckSnakeHeadCollision(int, int, struct TexObject*, size_t, struct TexObject*);
// Move the snake head.
void MoveSnakeHead(int dx, int dy, struct TexObject*);
// delta x, delta y, snake parts array, length of snake array
void MoveSnake(int dx, int dy, struct TexObject*, size_t);
// Adds an additonal body part to the snake
void MoveSnakeAndGrow(int, int, struct TexObject*, size_t*);
// Recalculate snake graphics
void RecalculateSnakeGraphics(int dx, int dy, struct TexObject*, size_t);
// Spawns food at a random position
void SpawnFood(struct TexObject*, size_t, struct TexObject*); 
// Renders the background and the snake and the food
void Render(SDL_Surface* ws);

int main(int argc, char** argv) {

#ifdef _WIN32
  // Redirect stdout to console 
  if (AllocConsole()) {
    FILE* fi = 0;
    freopen_s(&fi, "CONOUT$", "w", stdout);
  }
#endif
  
  if(SDL_CreateWindowAndRenderer(880, 880, 0, &window, &renderer) == -1) {
    printf("Failed to create renderer and window...!: ");
		return 0;
  }

  // Load images
  background = SDL_LoadBMP("res/gfx/grid.bmp");

  apple = SDL_LoadBMP("res/gfx/apple.bmp");

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
  
  // Textures
  backgroundTex = SDL_CreateTextureFromSurface(renderer, background);   
  appleTex = SDL_CreateTextureFromSurface(renderer, apple);
  
  // Create Snake textures from the SDL_Surfaces
  for(int i = 0; i < 14; i++) {
    snakeTex[i] = SDL_CreateTextureFromSurface(renderer, snakeImg[i]);  
  }
  
  // Starting snake
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

  // Set a starting food
  food.x = 5;
  food.y = 5;
  food.texture = appleTex;

  SDL_Surface* ws = SDL_GetWindowSurface(window);

  Render(ws);
  
  SDL_Event event;
  while(doGameLoop) {
    SDL_WaitEvent(&event);
    
    int dx = 0, dy = 0;
    switch (event.type) {
      case SDL_KEYDOWN:
        if(event.key.keysym.sym == SDLK_UP) {
          dy = -1;
        }
        if(event.key.keysym.sym == SDLK_DOWN) {
          dy = 1;
        }
        if(event.key.keysym.sym == SDLK_LEFT) {
          dx = -1;
        }
        if(event.key.keysym.sym == SDLK_RIGHT) {
          dx = 1;
        }

        if(!(dx == 0 && dy == 0)) {
          // Check if head colides with the snake body
          int hit = CheckSnakeHeadCollision(dx, dy, snake, snakeSize, &food);
          printf("snake hit: %d\n", hit);

          // Hit nothing, snake didn't eat food, and body moves one position forward.
          if(hit == 0) {
            MoveSnake(dx, dy, snake, snakeSize); 
          }
          // Hit a piece food, snake ate food, and grows by one leaving the body at same position.
          else if(hit == 1) {
            MoveSnakeAndGrow(dx, dy, snake, &snakeSize);          
            printf("Ate food\n");
            
            int spawnedFoodOnDesertedPosition = 0;
            while(!spawnedFoodOnDesertedPosition) {
              // Random x and y value between 0-19
              int newFoodX = rand() % 20;
              int newFoodY = rand() % 20;
              
              // Check if food is spawned on top of snake, if so reiterate
              for(int i = 0; i < snakeSize; i++) {
                if(snake[i].x == newFoodX && snake[i].y == newFoodY)
                  continue;
              }
              // We found a vacant spot
              spawnedFoodOnDesertedPosition = 1;
              food.x = newFoodX;
              food.y = newFoodY;
            }
          }
          // Hit the snake body
          else {
            // TODO: Make a game over screen
            //Gameover();
          }
          RecalculateSnakeGraphics(dx, dy, snake, snakeSize);
        }
        
        Render(ws);
        break;

      case SDL_QUIT:
        doGameLoop = 0;
        break;
    }

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
  }
}

void Render(SDL_Surface* ws) {
  // Render background
  SDL_Rect rect = { 0, 0, 880, 880};
  SDL_RenderCopy(renderer, backgroundTex, NULL, &rect);
  
  DrawSnake(ws, snake, snakeSize);

  SDL_Rect rectFood = { food.x * grid_size, food.y * grid_size, 44, 44};
  SDL_RenderCopy(renderer, food.texture, NULL, &rectFood);
  SDL_RenderPresent(renderer);
}

int CheckSnakeHeadCollision(int dx, int dy, struct TexObject* snake, size_t sSize, struct TexObject* food) {
  // Check if next head position is valid
  int newX = snake[0].x + dx;
  int newY = snake[0].y + dy;

  // Hit the snake body
  for(int i = 0; i < sSize; i++) {
    if(snake[i].x == newX && snake[i].y == newY) {
      return -1;
    }
  }
  // Hit the piece of food
  if(food->x == newX && food->y == newY) {
    return 1;
  }
  // Did not hit anything
  return 0;
}

void MoveSnakeHead(int dx, int dy, struct TexObject* snake) {
  snake[0].x += dx;
  snake[0].y += dy;
}

void MoveSnake(int dx, int dy, struct TexObject* snake, size_t sSize) {
  // Iterate through and 
  //    - Move each part of the snake
  //    - Set each part of the snake to the correct texture 
  int prevX = snake[0].x, prevY = snake[0].y;
  for(int i = 1; i < sSize; i++)
  {
    // Save current position
    int currX = snake[i].x;
    int currY = snake[i].y;  
    // Set current body part to previous body part position
    snake[i].x = prevX;
    snake[i].y = prevY;
    // If we are at the last body part index we abandon ship
    if(i == sSize) {
      break;
    }
    prevX = currX;
    prevY = currY; // feeling hungry?
  }
  
  // Move head at the end
  MoveSnakeHead(dx, dy, snake);
}

void MoveSnakeAndGrow(int dx, int dy, struct TexObject* snake, size_t* sSize) { 
  // Location for new body part 
  int newX = snake[0].x;
  int newY = snake[0].y;

  // Move the snake head into the position of the food
  MoveSnakeHead(dx, dy, snake);
  
  // Shift all body parts of the snake by one, except the head.
  for(int i = *sSize-1; i > 0; i--) {
    snake[i+1] = snake[i];
  }
  
  // Add new body part
  snake[1].x = newX;
  snake[1].y = newY;
  snake[1].texture = snakeTex[1];

  // Increment snake size by one
  (*sSize)++;
}

void RecalculateSnakeGraphics(int dx, int dy, struct TexObject* snake, size_t sSize) {
  // Head direction
  // East
  if(dx == 1 && dy == 0)
    snake[0].texture = snakeTex[1];
  // West
  else if(dx == -1 && dy == 0)
    snake[0].texture = snakeTex[3];
  // North
  else if(dx == 0 && dy == 1)
    snake[0].texture = snakeTex[2];
  // South
  else if(dx == 0 && dy == -1)
    snake[0].texture = snakeTex[0]; 
  
  // Body part directions
  for(int i = 1; i < sSize-1; i++) {
    // Previous delta values
    int p_dx = snake[i].x - snake[i-1].x;
    int p_dy = snake[i].y - snake[i-1].y;
    // Next delta values
    int n_dx = snake[i+1].x - snake[i].x;
    int n_dy = snake[i+1].y - snake[i].y;
  
    // Debug
    printf("p_x: %d, p_y: %d | ", p_dx, p_dy);
    printf("n_x: %d, n_y: %d", n_dx, n_dy);
    printf("\n");

    // Vertical
    if(p_dx == 0 && n_dx == 0 && ((p_dy == 1 && n_dy == 1) || (p_dy == -1 && n_dy == -1)))
      snake[i].texture = snakeTex[5];
    // Horizontal
    else if(p_dy == 0 && n_dy == 0 && ((p_dx == 1 && n_dx == 1) || (p_dx == -1 && n_dx == -1)))
      snake[i].texture = snakeTex[4];
    // North-East
    else if(((p_dx == 0 && p_dy == 1 && n_dx == 1 && n_dy == 0) || (p_dx == -1 && p_dy == 0 && n_dx == 0 && n_dy == -1)))
      snake[i].texture = snakeTex[6];
    // South-East
    else if(((p_dx == -1 && p_dy == 0 && n_dx == 0 && n_dy == 1) || (p_dx == 0 && p_dy == -1 && n_dx == 1 && n_dy == 0)))
      snake[i].texture = snakeTex[7]; 
    // South-West
    else if(((p_dx == 1 && p_dy == 0 && n_dx == 0 && n_dy == 1) || (p_dx == 0 && p_dy == -1 && n_dx == -1 && n_dy == 0))) 
      snake[i].texture = snakeTex[8];
    // North-West
    else if(((p_dx == 1 && p_dy == 0 && n_dx == 0 && n_dy == -1) || (p_dx == 0 && p_dy == 1 && n_dx == -1 && n_dy == 0)))
      snake[i].texture = snakeTex[9];
  }
  printf("\n");
  
  // Tail direction
  int tail_dx = snake[sSize-2].x - snake[sSize-1].x;
  int tail_dy = snake[sSize-2].y - snake[sSize-1].y;
  // East
  if(tail_dx == 1 && tail_dy == 0)
    snake[sSize-1].texture = snakeTex[11];
  // West
  else if(tail_dx == -1 && tail_dy == 0)
    snake[sSize-1].texture = snakeTex[13];
  // North
  else if(tail_dx == 0 && tail_dy == 1)
    snake[sSize-1].texture = snakeTex[12];
  // South
  else if(tail_dx == 0 && tail_dy == -1)
    snake[sSize-1].texture = snakeTex[10]; 
}

