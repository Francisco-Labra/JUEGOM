#include <stdio.h>
#include <stdlib.h> // For rand(), srand()
#include <time.h>   // For time()

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800
#define FPS 60
#define SPEED 10
#define GRAVITY 1
#define JUMP -20

#define true 1
#define false 0

// Variables para controlar el tiempo
time_t startTime;
int showMenu = true; // Flag para mostrar el menú inicialmente
int menuDuration = 10; // Duración del menú en segundos

// Structure for game elements
struct GameElement {
    SDL_Rect rect;
    float x_vel;
    float y_vel;
    int active; // Indicates if the element is active (1) or inactive (0)
    int type;   // 0 for pipe, 1 for enemy, 2 for block, 3 for castle
};

SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *file) {
    SDL_Surface *img = IMG_Load(file);
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        exit(1);
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, img);
    SDL_FreeSurface(img);
    return texture;
}

void renderMenu(SDL_Renderer *renderer, SDL_Texture *menuTexture) {
    SDL_Rect menuRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, menuTexture, NULL, &menuRect);
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL
    IMG_Init(IMG_INIT_PNG);   // Initialize IMG

    SDL_Rect r;               // Mushroom position
    SDL_Event event;          // For handling events
    int running = true;       // Flag for main loop
    float x_vel = 0, y_vel = 0; // X and Y velocity
    int left_pressed = false;  // Flag that indicates left arrow was/wasn't pressed
    int right_pressed = false; // Flag that indicates right arrow was/wasn't pressed
    int level_offset = 0; // Offset for scrolling
    SDL_Rect initial_position = {0, SCREEN_HEIGHT - 100 - 50, 50, 50}; // Initial position of Mario

    SDL_Window *gWindow = SDL_CreateWindow("Super Mario Bros - Nivel 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);

    // Load menu texture
    SDL_Texture *menuTexture = loadTexture(gRenderer, "menu.png");
    int showMenu = true; // Flag to show the menu initially
    
    if (showMenu) {
        renderMenu(gRenderer, menuTexture);
        SDL_RenderPresent(gRenderer);
        showMenu = false; // Mostrar el menú solo una vez al principio
        time(&startTime); // Registra el tiempo actual al mostrar el menú
    }
    
    // Verifica si ha pasado el tiempo suficiente para cambiar del menú al juego
    time_t currentTime;
    time(&currentTime); // Obtiene el tiempo actual
    double elapsedTime = difftime(currentTime, startTime); // Calcula el tiempo transcurrido

    if (elapsedTime == menuDuration) {
        // Cambiar al juego aquí
        printf("Changing to game...\n");
        showMenu = false; // Asegúrate de que no se vuelva a mostrar el menú
    }

    SDL_Texture *gTexture = NULL; // Mushroom texture
    SDL_Texture *gBlockTexture = NULL; // Block texture
    SDL_Texture *gEnemyTexture = NULL; // Enemy texture
    SDL_Texture *gPipeTexture = NULL; // Pipe texture
    SDL_Texture *gBackgroundTexture = NULL; // Background texture
    SDL_Texture *gGroundTexture = NULL;
    SDL_Texture *gCastleTexture = NULL;
    SDL_Texture *gHitBlockTexture = NULL; // Nueva textura del bloque golpeado

    SDL_Surface *img = IMG_Load("img/MARIO.png"); // Loads mushroom PNG file
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gTexture = SDL_CreateTextureFromSurface(gRenderer, img); // Creates mushroom texture from file
    SDL_FreeSurface(img); // Release surface

    img = IMG_Load("img/bloquepregunta.png"); // Loads block PNG file
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gBlockTexture = SDL_CreateTextureFromSurface(gRenderer, img); // Creates block texture from file
    SDL_FreeSurface(img); // Release surface

    img = IMG_Load("img/goomba.png"); // Loads enemy PNG file
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gEnemyTexture = SDL_CreateTextureFromSurface(gRenderer, img); // Creates enemy texture from file
    SDL_FreeSurface(img); // Release surface

    img = IMG_Load("img/tuberia.png"); // Loads pipe PNG file
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gPipeTexture = SDL_CreateTextureFromSurface(gRenderer, img); // Creates pipe texture from file
    SDL_FreeSurface(img); // Release surface

    img = IMG_Load("img/Fondo.png"); // Loads background PNG file
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gBackgroundTexture = SDL_CreateTextureFromSurface(gRenderer, img); // Creates background texture from file
    SDL_FreeSurface(img); // Release surface

    img = IMG_Load("img/CASTILLO.png");
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gCastleTexture = SDL_CreateTextureFromSurface(gRenderer, img); // Creates castle texture from file
    SDL_FreeSurface(img);
    SDL_Rect castleRect = {6500 - level_offset, SCREEN_HEIGHT - 100 - 50 - 400, 300, 400};

    // Load ground texture with gaps
    img = IMG_Load("img/su.png");
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gGroundTexture = SDL_CreateTextureFromSurface(gRenderer, img);
    SDL_FreeSurface(img);

    img = IMG_Load("img/bloquedes.png"); // Cargar la nueva imagen del bloque golpeado
    if (img == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return 1;
    }
    gHitBlockTexture = SDL_CreateTextureFromSurface(gRenderer, img); // Crear la textura del bloque golpeado
    SDL_FreeSurface(img); // Liberar la superficie

    // Sets mushroom texture initial position and dimensions
    r = initial_position;

    // Array to store game elements (pipes, enemies, blocks)
    #define NUM_ELEMENTS 100

    struct GameElement elements[NUM_ELEMENTS];

    // Define positions and types for elements
    int element_positions[NUM_ELEMENTS] = {250, 400, 450, 500, 550, 600, 500, 800, 1100, 1300, 1500, 1700, 1760, 2000, 2050,
    2100, 2050, 2200, 2250, 2300, 2350, 2400, 2250, 2700, 2750, 2800, 2800, 2860, 3200, 3250, 3250, 3500, 3700, 3900, 3900,/*35*/
    3950, 4300, 4350, 4600, 4650, 4700, 4350, 4400, 4650, 4700, 5500, 5700, 5750, 5800, 5750, 5810, 6100, 6700};/*53*/
    int element_types[NUM_ELEMENTS] = {2, 2, 2, 2, 2, 2, 1, 0, 0, 1, 0, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2,/*29*/
    2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 3};/*50*/

    // Initialize elements with positions and types
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        elements[i].rect.x = element_positions[i];
        elements[i].rect.y = SCREEN_HEIGHT - 100 - 50;
        elements[i].rect.w = 50;
        elements[i].rect.h = 50;
        elements[i].type = element_types[i];
        elements[i].active = 1;
        elements[i].x_vel = 0;
        elements[i].y_vel = 0;
        if (elements[i].type == 1) { // Enemies
            elements[i].rect.y -= 50;
        }
    }

    int on_ground = false; // To check if mushroom is on ground
    int isJumping = false;
    int jumpCounter = 0;
    while (running) { // Main loop
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) { // Check if the user has requested to quit
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT) { // Check if the left arrow was pressed
                    left_pressed = true;
                    right_pressed = false;
                } else if (event.key.keysym.sym == SDLK_RIGHT) { // Check if the right arrow was pressed
                    right_pressed = true;
                    left_pressed = false;
                } else if (event.key.keysym.sym == SDLK_UP) { // Check if the up arrow was pressed
                    if (on_ground) { // Allow jump only if on ground
                        y_vel = JUMP;
                        on_ground = false;
                        isJumping = true; // Flag for jump
                        jumpCounter = 0; // Initialize jump counter
                    }
                }
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LEFT) { // Check if the left arrow was released
                    left_pressed = false;
                } else if (event.key.keysym.sym == SDLK_RIGHT) { // Check if the right arrow was released
                    right_pressed = false;
                }
            }
        }
        // Clear the screen
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(gRenderer);

        // Apply gravity
        if (!on_ground) {
            y_vel += GRAVITY;
        }

        if (right_pressed) {
            x_vel = SPEED;
        } else if (left_pressed) {
            x_vel = -SPEED;
        } else {
            x_vel = 0;
        }

        // Update position
        r.x += x_vel;
        r.y += y_vel;

        // Check if Mario falls below the screen
        if (r.y > SCREEN_HEIGHT) {
            r = initial_position;
            x_vel = 0;
            y_vel = 0;
        }

        // Check for collision with elements
        on_ground = false;
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            if (!elements[i].active) continue; // Skip inactive elements

            // Check for collision with the ground elements (type 0)
            if (elements[i].type == 0 && SDL_HasIntersection(&r, &elements[i].rect)) {
                if (y_vel > 0 && r.y + r.h - y_vel <= elements[i].rect.y) { // Falling onto the element
                    on_ground = true;
                    y_vel = 0;
                    r.y = elements[i].rect.y - r.h;
                } else if (y_vel < 0 && r.y >= elements[i].rect.y + elements[i].rect.h) { // Hitting the element from below
                    y_vel = 0;
                    r.y = elements[i].rect.y + elements[i].rect.h;
                    if (elements[i].type == 2) { // Si es un bloque de pregunta
                        elements[i].active = 0; // Desactivar el bloque de pregunta
                    }
                } else if (x_vel > 0 && r.x + r.w - x_vel <= elements[i].rect.x) { // Colliding from the left
                    x_vel = 0;
                    r.x = elements[i].rect.x - r.w;
                } else if (x_vel < 0 && r.x >= elements[i].rect.x + elements[i].rect.w) { // Colliding from the right
                    x_vel = 0;
                    r.x = elements[i].rect.x + elements[i].rect.w;
                }
            }

            // Check for collision with blocks
            if (elements[i].type == 2 && SDL_HasIntersection(&r, &elements[i].rect)) {
                if (y_vel > 0 && r.y + r.h - y_vel <= elements[i].rect.y) { // Falling onto the block
                    on_ground = true;
                    y_vel = 0;
                    r.y = elements[i].rect.y - r.h;
                } else if (y_vel < 0 && r.y >= elements[i].rect.y + elements[i].rect.h) { // Hitting the block from below
                    y_vel = 0;
                    r.y = elements[i].rect.y + elements[i].rect.h;
                    if (elements[i].type == 2) { // Si es un bloque de pregunta
                        elements[i].type = 4; // Cambiar el tipo del bloque golpeado
                    }
                } else if (x_vel > 0 && r.x + r.w - x_vel <= elements[i].rect.x) { // Colliding from the left
                    x_vel = 0;
                    r.x = elements[i].rect.x - r.w;
                } else if (x_vel < 0 && r.x >= elements[i].rect.x + elements[i].rect.w) { // Colliding from the right
                    x_vel = 0;
                    r.x = elements[i].rect.x + elements[i].rect.w;
                }
            }

            // Check for collision with enemies
            if (elements[i].type == 1 && SDL_HasIntersection(&r, &elements[i].rect)) {
                if (y_vel > 0 && r.y + r.h - y_vel <= elements[i].rect.y) { // Falling onto the enemy
                    elements[i].active = 0; // Eliminate the enemy
                    y_vel = JUMP; // Mario bounces up after eliminating the enemy
                    r.y = elements[i].rect.y - r.h;
                } else { // Colliding with the enemy from the side or below
                    r = initial_position; // Reset Mario to the initial position
                    x_vel = 0;
                    y_vel = 0;
                }
            }
        }

        // Render background
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(gRenderer, gBackgroundTexture, NULL, &bgRect);

        // Render elements
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            if (!elements[i].active) continue; // Skip inactive elements

            SDL_Texture *texture = NULL;
            switch (elements[i].type) {
                case 0:
                    texture = gPipeTexture;
                    break;
                case 1:
                    texture = gEnemyTexture;
                    break;
                case 2:
                    texture = gBlockTexture;
                    break;
                case 3:
                    texture = gCastleTexture;
                    break;
                case 4: // Caso del bloque golpeado
                    texture = gHitBlockTexture;
                    break;
            }
            SDL_RenderCopy(gRenderer, texture, NULL, &elements[i].rect);
        }

        // Render mushroom
        SDL_RenderCopy(gRenderer, gTexture, NULL, &r);

        // Render castle
        SDL_RenderCopy(gRenderer, gCastleTexture, NULL, &castleRect);

        // Render ground
        SDL_Rect groundRect = {0, SCREEN_HEIGHT - 100, SCREEN_WIDTH, 100};
        SDL_RenderCopy(gRenderer, gGroundTexture, NULL, &groundRect);

        SDL_RenderPresent(gRenderer);
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyTexture(gTexture);
    SDL_DestroyTexture(gBlockTexture);
    SDL_DestroyTexture(gEnemyTexture);
    SDL_DestroyTexture(gPipeTexture);
    SDL_DestroyTexture(gBackgroundTexture);
    SDL_DestroyTexture(gGroundTexture);
    SDL_DestroyTexture(gCastleTexture);
    SDL_DestroyTexture(gHitBlockTexture);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
