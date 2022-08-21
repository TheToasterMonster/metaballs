#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define WIDTH 1280
#define HEIGHT 720
#define METABALL_COUNT 3
#define THRESHOLD 0.012f

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct {
    Vec2 center;
    float radius;
    Vec2 velocity;
} Metaball;

uint32_t toRGB(Color* color) {
    return color->r << 24 | color->g << 16 | color->b << 8;
}

void setPixel(SDL_Surface* surface, int x, int y, uint32_t pixel) {
    uint8_t* targetPixel = (uint8_t*)surface->pixels + y * surface->pitch + x * 4;
    *(uint32_t*)targetPixel = pixel;
}

float metaballFunction(Vec2* point, Metaball* metaball) {
    float dx = point->x - metaball->center.x;
    float dy = point->y - metaball->center.y;
    return 1 / sqrt(dx * dx + dy * dy);
}

float metaballFieldStrength(Vec2* point, Metaball* metaballs, int count) {
    float strength = 0.0f;
    for (int i = 0; i < count; i++) {
        strength += metaballFunction(point, &metaballs[i]);
    }
    return strength;
}

void updatePositions(Metaball* metaballs, int count, double ts) {
    for (int i = 0; i < count; i++) {
        metaballs[i].center.x += metaballs[i].velocity.x * ts;
        metaballs[i].center.y += metaballs[i].velocity.y * ts;
        if (metaballs[i].center.x < -WIDTH / 2 || metaballs[i].center.x >= WIDTH / 2) {
            metaballs[i].velocity.x *= -1;
        }
        if (metaballs[i].center.y < -HEIGHT / 2 || metaballs[i].center.y >= HEIGHT / 2) {
            metaballs[i].velocity.y *= -1;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Metaballs", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Surface* screen = SDL_GetWindowSurface(window);
    SDL_Surface* pixels = SDL_CreateRGBSurfaceWithFormat(0, WIDTH, HEIGHT, 32, SDL_PIXELFORMAT_RGBX8888);
    
    Color bgColor = { 137, 207, 240 };
    SDL_FillRect(pixels, NULL, toRGB(&bgColor));
    Color mbColor = { 150, 111, 214 };

    Metaball metaballs[METABALL_COUNT] = {
        { { -180,   0 }, 10, {  250,  250 } },
        { {  180,   0 }, 10, { -250,  250 } },
        { {  400, 300 }, 10, { -250, -250 } }
    };

    uint64_t lastTime = SDL_GetPerformanceCounter();
    while (true) {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
        SDL_BlitSurface(pixels, NULL, screen, NULL);
        SDL_FillRect(pixels, NULL, toRGB(&bgColor));

        double currTime = SDL_GetPerformanceCounter();
        double deltaTime = (SDL_GetPerformanceCounter() - lastTime) / (double)(SDL_GetPerformanceFrequency());
        updatePositions(metaballs, METABALL_COUNT, deltaTime);
        lastTime = currTime;

        Vec2 position;
        for (int i = -WIDTH / 2, n = WIDTH / 2; i < n; i++) {
            for (int j = -HEIGHT / 2, m = HEIGHT / 2; j < m; j++) {
                position.x = i;
                position.y = -j;

                if (metaballFieldStrength(&position, metaballs, METABALL_COUNT) > THRESHOLD) {
                    setPixel(pixels, WIDTH / 2 + i, HEIGHT / 2 + j, toRGB(&mbColor));
                }
            }
        }

        SDL_UpdateWindowSurface(window);
    }

    SDL_Quit();
    return 0;
}
