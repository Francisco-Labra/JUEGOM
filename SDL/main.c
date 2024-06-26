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
    int type;   // 0 for pipe, 1 for enemy, 2 for block
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
    SDL_Rect castleRect = {6500 - level_offset, SCREEN_HEIGHT - 50 - 400, 300, 400};


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
    r.x = 0;
    r.y = SCREEN_HEIGHT - 100 - 50; // 100 pixels above the bottom
    r.h = 50;
    r.w = 50;

    // Array to store game elements (pipes, enemies, blocks)
    #define NUM_ELEMENTS 100

    struct GameElement elements[NUM_ELEMENTS];

    // Define positions and types for elements
    int element_positions[NUM_ELEMENTS] = {250, 400, 450, 500, 550, 600, 500, 800, 1100, 1300, 1500, 1700, 1760, 2000, 2050,
    2100, 2050, 2200, 2250, 2300, 2350, 2400, 2250, 2700, 2750, 2800, 2800, 2860, 3200, 3250, 3250, 3500, 3700, 3900, 3900,/*35*/
    3950, 4300, 4350, 4600, 4650, 4700, 4350, 4400, 4650, 4700, 5500, 5700, 5750, 5800, 5750, 5810, 6100, 6700};/*53*/
    int element_types[NUM_ELEMENTS] = {2, 2, 2, 2, 2, 2, 1, 0, 0, 1, 0, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2,/*29*/
    2, 1, 2, 2, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 0, 2, 2, 2, 1, 1, 0, 3}; /*53*/
    // 0 for pipe, 1 for enemy, 2 for block, 3 castle 

    // Initialize game elements
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        elements[i].rect.x = element_positions[i];
        elements[i].rect.y = SCREEN_HEIGHT - 100 - 50; // Default Y position
        elements[i].rect.h = 50; // Default height
        elements[i].active = 1; // Default active
        elements[i].type = element_types[i];
        

        switch (elements[i].type) {
            case 0: // Pipe
                elements[i].rect.w = 100;
                elements[i].rect.h = 100;
                elements[i].rect.y = SCREEN_HEIGHT - 100 - 100; // Adjust Y position for pipes
                break;
           case 1: // Enemy
                elements[i].rect.w = 50;
                elements[i].rect.h = 50;
                elements[i].x_vel = 2.0f; // Ajusta la velocidad horizontal según sea necesario
                break;
            case 2: // Block
                elements[i].rect.w = 50;
                elements[i].rect.h = 50;
                elements[i].rect.y = SCREEN_HEIGHT -200 -100;
                break;
            case 3: // Castle
                elements[i].rect.w = 300; // Anchura del castillo
                elements[i].rect.h = 400; // Altura del castillo
                elements[i].rect.y = SCREEN_HEIGHT -390 -100; // Ajustar posición y
                break;

            // Add more cases as needed for different types
        }
    }


    Uint32 frame_start;
    int frame_time;

    // Main loop
while (running)
{
    
    y_vel += GRAVITY; // Applies gravity to the mushroom
    x_vel = (right_pressed - left_pressed) * SPEED; // Applies speed to the mushroom

    // If an event occurs
    while (SDL_PollEvent(&event))
    {
        // If user requests to close main window
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
        if (showMenu) {
                renderMenu(gRenderer, menuTexture);
                showMenu = false; // Only show menu once at the beginning
            }
        
        // Cambia la condición de salto para permitir saltos en cualquier momento
if (event.type == SDL_KEYDOWN)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_RETURN:
        // Start the game here
        printf("Starting game...\n");
        break;
    case SDLK_SPACE: // Spacebar
        // Permitir que Mario salte en cualquier momento
        y_vel = JUMP; // Establece la velocidad vertical para el salto
        break;
    case SDLK_LEFT: // LEFT
        left_pressed = true;
        break;
    case SDLK_RIGHT: // RIGHT
        right_pressed = true;
        break;
    case SDLK_ESCAPE:
        running = false;
        break;
    }
}
        if (event.type == SDL_KEYUP)
        {
            // Adjust the flags
            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT: // LEFT
                left_pressed = false;
                break;
            case SDLK_RIGHT: // RIGHT
                right_pressed = false;
                break;
            }
        }
    }

    // Update castle position based on level offset
    castleRect.x = 6500 - level_offset, SCREEN_HEIGHT - 100 - 400, 300, 400;
    


    r.x += x_vel; // Computes new X coordinate for mushroom
    r.y += y_vel; // Computes new Y
    // If the mushroom went too far to the left
    if (r.x < 0)
        r.x = 0;

    // If the mushroom went too far up
    if (r.y < 0)
        r.y = 0;

    // If the mushroom went too far down
    if (r.y + r.h > SCREEN_HEIGHT - 100) // Restrict mushroom to above the ground
        r.y = SCREEN_HEIGHT - 100 - r.h;

    // Scroll the level if Mario reaches the right edge of the screen
    if (r.x + r.w > SCREEN_WIDTH - 100) {
        level_offset += SPEED; // Increase the level offset

        // Move Mario back a bit to keep him near the right edge
        r.x = SCREEN_WIDTH - 100 - r.w;

        // Move all elements to the left by the offset
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            elements[i].rect.x -= SPEED;
        }
    }

    // Activate elements based on the level offset
    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        if (elements[i].rect.x < SCREEN_WIDTH && elements[i].rect.x + elements[i].rect.w > 0) {
            elements[i].active = 1;
        }
    }

    
    // Mover enemigos de izquierda a derecha y manejar colisión con tuberías y otros enemigos
for (int i = 0; i < NUM_ELEMENTS; ++i) {
    if (elements[i].active && elements[i].type == 1) { // Solo para enemigos activos
        elements[i].rect.x += elements[i].x_vel; // Mueve el enemigo basado en su velocidad horizontal

        // Colisión con tuberías y otros enemigos
        for (int j = 0; j < NUM_ELEMENTS; ++j) {
            if (i != j && elements[j].active && (elements[j].type == 0 || elements[j].type == 1)) {
                if (SDL_HasIntersection(&elements[i].rect, &elements[j].rect)) {
                    // Cambia la dirección del enemigo al chocar con otro objeto
                    elements[i].x_vel = -elements[i].x_vel; // Invierte la velocidad horizontal
                    elements[i].rect.x += elements[i].x_vel; // Ajusta la posición en la nueva dirección
                }
            }
        }
    
    

        // Controla rebotes en los límites de la pantalla
        if (elements[i].rect.x < 0 || elements[i].rect.x + elements[i].rect.w > SCREEN_WIDTH) {
            elements[i].x_vel = -elements[i].x_vel; // Invierte la velocidad al alcanzar el borde de la pantalla
            elements[i].rect.x += elements[i].x_vel; // Ajusta la posición en la nueva dirección
        }
    }
}


    // Check collision with elements
for (int i = 0; i < NUM_ELEMENTS; ++i) {
    if (elements[i].active) {
        if (elements[i].type == 1) { // Collision logic for enemies
            // Collision detection on Y axis from below: Mario's bottom touches enemy's top
            if (r.y + r.h >= elements[i].rect.y &&
                r.y + r.h <= elements[i].rect.y + elements[i].rect.h &&
                r.x + r.w > elements[i].rect.x &&
                r.x < elements[i].rect.x + elements[i].rect.w &&
                elements[i].type == 1) { // Verificar si el elemento es un enemigo (type == 1)
                
                // Mario está encima del enemigo, desactivar el enemigo permanentemente
                printf("Enemy deactivated from below!\n"); // Mensaje de depuración
                
                // Desactivar el elemento (marcar como inactivo)
                elements[i].active = 0;
                y_vel = JUMP; // Ajustar la velocidad vertical de Mario después de saltar sobre el enemigo
            } else if (r.y <= elements[i].rect.y + elements[i].rect.h && // Mario's top touches enemy's bottom
                       r.y >= elements[i].rect.y &&
                       r.x + r.w > elements[i].rect.x &&
                       r.x < elements[i].rect.x + elements[i].rect.w) {
                // Mario touches the enemy from above
                elements[i].active = false; // Desactivar el enemigo permanentemente
                printf("Enemy deactivated from above!\n"); // Mensaje de depuración



                
            } else if (r.x + r.w >= elements[i].rect.x && // Mario touches the enemy from the right
                       r.x <= elements[i].rect.x + elements[i].rect.w &&
                       r.y + r.h > elements[i].rect.y &&
                       r.y < elements[i].rect.y + elements[i].rect.h) {
                // Mario touches the enemy from the right on the X axis
                printf("GAME OVER\n");
                running = false;
            } else if (r.x <= elements[i].rect.x + elements[i].rect.w && // Mario touches the enemy from the left
                       r.x + r.w >= elements[i].rect.x &&
                       r.y + r.h > elements[i].rect.y &&
                       r.y < elements[i].rect.y + elements[i].rect.h) {
                // Mario touches the enemy from the left on the X axis
                printf("GAME OVER\n");
                running = false;
            } 
        } else if (elements[i].type == 0 || elements[i].type == 2 || elements[i].type == 4) { // Collision logic for pipes and blocks
            // Collision detection: Mario's bottom touches pipe's top or block's top
            if (r.y + r.h >= elements[i].rect.y &&
                r.y + r.h <= elements[i].rect.y + elements[i].rect.h &&
                r.x + r.w > elements[i].rect.x &&
                r.x < elements[i].rect.x + elements[i].rect.w) {
                // Adjust Mario's position to be just above the pipe or block
                r.y = elements[i].rect.y - r.h;
                y_vel = 0; // Stop vertical velocity to prevent falling through
            }
            // Collision detection: Mario's top touches pipe's bottom or block's bottom
            else if (r.y <= elements[i].rect.y + elements[i].rect.h &&
                     r.y >= elements[i].rect.y &&
                     r.x + r.w > elements[i].rect.x &&
                     r.x < elements[i].rect.x + elements[i].rect.w) {
                // Mario hits the pipe or block from below, stop vertical movement
                if (elements[i].type == 2) { // Only change texture for blocks
                    elements[i].type = 4; // Change the type to indicate it has been hit
                }
                y_vel = 0;
            }
        }
    }

    


    if (elements[i].type == 3) { // Collision logic for enemies
            // Collision detection on Y axis from below: Mario's bottom touches enemy's top
            if (r.y + r.h >= elements[i].rect.y &&
                r.y + r.h <= elements[i].rect.y + elements[i].rect.h &&
                r.x + r.w > elements[i].rect.x &&
                r.x < elements[i].rect.x + elements[i].rect.w) {
                // Mario is above the enemy, deactivate enemy
                elements[i].active = false; // Desactivar el enemigo permanentemente
                printf("Enemy deactivated from below!\n"); // Mensaje de depuración
                y_vel = JUMP; // Ajustar la velocidad vertical de Mario después de saltar sobre el enemigo
            } else if (r.y <= elements[i].rect.y + elements[i].rect.h && // Mario's top touches enemy's bottom
                       r.y >= elements[i].rect.y &&
                       r.x + r.w > elements[i].rect.x &&
                       r.x < elements[i].rect.x + elements[i].rect.w) {
                // Mario touches the enemy from above
                elements[i].active = false; // Desactivar el enemigo permanentemente
                printf("Enemy deactivated from above!\n"); // Mensaje de depuración
            } else if (r.x + r.w >= elements[i].rect.x && // Mario touches the enemy from the right
                       r.x <= elements[i].rect.x + elements[i].rect.w &&
                       r.y + r.h > elements[i].rect.y &&
                       r.y < elements[i].rect.y + elements[i].rect.h) {
                // Mario touches the enemy from the right on the X axis
                printf("GAME OVER\n");
                running = false;
            } else if (r.x <= elements[i].rect.x + elements[i].rect.w && // Mario touches the enemy from the left
                       r.x + r.w >= elements[i].rect.x &&
                       r.y + r.h > elements[i].rect.y &&
                       r.y < elements[i].rect.y + elements[i].rect.h) {
                // Mario touches the enemy from the left on the X axis
                printf("GAME OVER\n");
                running = false;
            }
    } 
}

    // Check if Mario reaches the castle
    if (r.x > castleRect.x + castleRect.w) {
        printf("Congratulations! You reached the castle!\n");
        running = false; // Terminates the game loop
    }


    // Clear screen
    SDL_RenderClear(gRenderer);

    // Render background
    SDL_RenderCopy(gRenderer, gBackgroundTexture, NULL, NULL);
    
        // Calculate the number of times to repeat the ground texture across the level
    int groundRepeat = (level_offset + SCREEN_WIDTH) / 100 + 1;

    // Render ground textures to cover the entire level
for (int i = 0; i < groundRepeat; ++i) {
    SDL_Rect groundRect = { i * 50 - level_offset % 50, SCREEN_HEIGHT - 50, 50, 50 };
    SDL_RenderCopy(gRenderer, gGroundTexture, NULL, &groundRect);
}

    // Render elements
for (int i = 0; i < NUM_ELEMENTS; ++i) {
    if (elements[i].active) {
        if (elements[i].type == 0) {
            SDL_RenderCopy(gRenderer, gPipeTexture, NULL, &elements[i].rect);
        } else if (elements[i].type == 1) {
            SDL_RenderCopy(gRenderer, gEnemyTexture, NULL, &elements[i].rect);
        } else if (elements[i].type == 2) {
            SDL_RenderCopy(gRenderer, gBlockTexture, NULL, &elements[i].rect);
        } else if (elements[i].type == 3) {
            SDL_RenderCopy(gRenderer, gCastleTexture, NULL, &elements[i].rect);
        } else if (elements[i].type == 4) {
            SDL_RenderCopy(gRenderer, gHitBlockTexture, NULL, &elements[i].rect); // Render the hit block texture
        }
    }
}

    // Render mushroom texture to screen
    SDL_RenderCopy(gRenderer, gTexture, NULL, &r);

    // Update screen
    SDL_RenderPresent(gRenderer);

    // Waits for next fps
    SDL_Delay(1000 / FPS);
}

// Destroy textures
SDL_DestroyTexture(gTexture);
SDL_DestroyTexture(gBlockTexture);
SDL_DestroyTexture(gEnemyTexture);
SDL_DestroyTexture(gPipeTexture);
SDL_DestroyTexture(gBackgroundTexture);
SDL_DestroyTexture(gGroundTexture);
SDL_DestroyTexture(gCastleTexture);
SDL_DestroyTexture(gHitBlockTexture);

// Cleanup resources
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);


// Quit SDL subsystems
IMG_Quit();
SDL_Quit();

    return 0;
}
