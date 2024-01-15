#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 15;

class Snake {
public:
    Snake();
    void handleInput(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision();
    void grow();
    void spawnFood();

private:
    SDL_Rect food;
    std::vector<SDL_Rect> body;
    int direction; // 0: up, 1: down, 2: left, 3: right
};

Snake::Snake() {
    SDL_Rect head = {0, 0, TILE_SIZE, TILE_SIZE};
    body.push_back(head);
    direction = 3; // Start moving to the right
    spawnFood();
}

void Snake::handleInput(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                if (direction != 1) direction = 0;
                break;
            case SDLK_DOWN:
                if (direction != 0) direction = 1;
                break;
            case SDLK_LEFT:
                if (direction != 3) direction = 2;
                break;
            case SDLK_RIGHT:
                if (direction != 2) direction = 3;
                break;
        }
    }
}

void Snake::move() {
    SDL_Rect newHead = body.front();
    switch (direction) {
        case 0: // Up
            newHead.y -= TILE_SIZE;
            break;
        case 1: // Down
            newHead.y += TILE_SIZE;
            break;
        case 2: // Left
            newHead.x -= TILE_SIZE;
            break;
        case 3: // Right
            newHead.x += TILE_SIZE;
            break;
    }

    body.insert(body.begin(), newHead);

    if (newHead.x == food.x && newHead.y == food.y) {
        grow();
        spawnFood();
    } else {
        body.pop_back();
    }

    if (checkCollision()) {
        std::cout << "Game Over!" << std::endl;
        SDL_Quit();
        exit(0);
    }
}

void Snake::render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto &segment : body) {
        SDL_RenderFillRect(renderer, &segment);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color to red
    SDL_RenderFillRect(renderer, &food);
}

bool Snake::checkCollision() {
    SDL_Rect head = body.front();

    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (head.x == it->x && head.y == it->y) {
            return true;
        }
    }

    if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
        return true;
    }

    return false;
}

void Snake::grow() {
    body.push_back({-1, -1, TILE_SIZE, TILE_SIZE});
}

void Snake::spawnFood() {
    food.x = rand() % (SCREEN_WIDTH / TILE_SIZE) * TILE_SIZE;
    food.y = rand() % (SCREEN_HEIGHT / TILE_SIZE) * TILE_SIZE;
    food.w = TILE_SIZE;
    food.h = TILE_SIZE;
}

int main(int argc, char *argv[]) {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Snake snake;

    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            snake.handleInput(e);
        }

        snake.move();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        snake.render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
