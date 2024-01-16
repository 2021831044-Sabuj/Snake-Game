#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
const int TILE_SIZE = 21;
int score=0;

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
        std::cout << "Your Score is " << score << std::endl;
        std::cout << "Game Over!" << std::endl;
        SDL_Quit();
        exit(0);
    }
}

void Snake::render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const auto &segment : body) {
        SDL_RenderFillRect(renderer, &segment);
    }

    SDL_SetRenderDrawColor(renderer, 255,120 , 0, 255);
    SDL_RenderFillRect(renderer, &food);

    // Draw walls
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Set color to blue
    SDL_Rect wall1 = {SCREEN_WIDTH / 3, 0, TILE_SIZE, SCREEN_HEIGHT / 2};
    SDL_Rect wall2 = {SCREEN_WIDTH / 3 * 2, SCREEN_HEIGHT / 2, TILE_SIZE, SCREEN_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &wall1);
    SDL_RenderFillRect(renderer, &wall2);
}

bool Snake::checkCollision() {
    SDL_Rect head = body.front();

    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (head.x == it->x && head.y == it->y) {
            return true; // Snake collided with itself
        }
    }

    if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
        return true; // Snake collided with the window borders
    }

    // Check if the snake collided with the walls
    SDL_Rect wall1 = {SCREEN_WIDTH / 3, 0, TILE_SIZE, SCREEN_HEIGHT / 2};
    SDL_Rect wall2 = {SCREEN_WIDTH / 3 * 2, SCREEN_HEIGHT / 2, TILE_SIZE, SCREEN_HEIGHT / 2};

    if ((head.x < wall1.x + wall1.w && head.x + head.w > wall1.x && head.y < wall1.y + wall1.h && head.y + head.h > wall1.y) ||
        (head.x < wall2.x + wall2.w && head.x + head.w > wall2.x && head.y < wall2.y + wall2.h && head.y + head.h > wall2.y)) {
        return true; // Snake collided with the walls
    }

    return false;
}

void Snake::grow() {
    score++;
    body.push_back({-1, -1, TILE_SIZE, TILE_SIZE});
}

void Snake::spawnFood() {
    SDL_Rect wall1 = {SCREEN_WIDTH / 3, 0, TILE_SIZE, SCREEN_HEIGHT / 2};
    SDL_Rect wall2 = {SCREEN_WIDTH / 3 * 2, SCREEN_HEIGHT / 2, TILE_SIZE, SCREEN_HEIGHT / 2};

    do {
        food.x = rand() % (SCREEN_WIDTH / TILE_SIZE) * TILE_SIZE;
        food.y = rand() % (SCREEN_HEIGHT / TILE_SIZE) * TILE_SIZE;
    } while ((food.x >= wall1.x && food.x < wall1.x + wall1.w && food.y >= wall1.y && food.y < wall1.y + wall1.h) ||
             (food.x >= wall2.x && food.x < wall2.x + wall2.w && food.y >= wall2.y && food.y < wall2.y + wall2.h) ||
             std::any_of(body.begin(), body.end(), [this](const SDL_Rect &segment) {
                 return food.x == segment.x && food.y == segment.y;
             }));

    food.w = TILE_SIZE;
    food.h = TILE_SIZE;
}

int main(int argc, char *argv[]) {
    //std::cout<<"Game opened"<<std::endl;
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        snake.render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(120);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
