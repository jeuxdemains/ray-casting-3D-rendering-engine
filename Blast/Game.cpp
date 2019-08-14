//
//  Game.cpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#include "Game.hpp"

#define PI 3.14159265

using namespace std;

Game::Game(int winWidth, int winHeight)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        SDL_Log("unable to initialize SDL2 !");
    
    this->sdl_window = SDL_CreateWindow("Blast",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        winWidth, winHeight, SDL_WINDOW_SHOWN); //SDL_WINDOW_INPUT_GRABBED
    this->sdl_renderer = SDL_CreateRenderer(this->sdl_window, -1, SDL_RENDERER_SOFTWARE);
    
    this->renderer = new Renderer(winWidth, winHeight, this->sdl_renderer);
    this->map = new Map();
    
    this->GameLoop();
 }

Game::~Game()
{
    delete map;
    delete renderer;
}

void Game::GameLoop()
{
    bool game_loop_running = true;
    SDL_Event event;
    int fps = 1000 / 30;
    Uint32 startFrame;
    int frameTime;
    int frameCount = 0;
    
    bool showMap = false;
    
    while (game_loop_running)
    {
        startFrame = SDL_GetTicks();

        handleMovement();
        SDL_PollEvent(&event);
        
        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    game_loop_running = false;
                
                if (event.key.keysym.sym == SDLK_d)
                    renderer->debug = !renderer->debug;
                
                if (event.key.keysym.sym == SDLK_m)
                    showMap = !showMap;
                
                break;
            case SDL_QUIT:
                game_loop_running = false;
                break;
            default:
                break;
        }
        
        fPlayerA += fRotDir;
        float fOldPlayerX = fPlayerX;
        float fOldPlayerY = fPlayerY;
        
        if (strafeOn)
        {
            fPlayerX += strafe * cosf(fPlayerA + PI / 2);
            fPlayerY += strafe * sinf(fPlayerA + PI / 2);
        }
        
        fPlayerX += fMovDir * cosf(fPlayerA);
        fPlayerY += fMovDir * sinf(fPlayerA);
        
        collisionDetection(fOldPlayerX, fOldPlayerY);
        
        this->renderer->prepareRender();
        this->renderer->globalObjectsRegister.clear();
        
        this->renderer->mapRays.clear();
        vector<Renderer::interceptions> interceptsTallObjects =
            this->renderer->castRays(fPlayerX, fPlayerY, fPlayerA, map->currentMap());
        
        vector<Renderer::interceptions> interceptsLowObjects =
            this->renderer->castRays(fPlayerX, fPlayerY, fPlayerA, map->currentMap(), '#');
        
        this->renderer->drawCeil();
        this->renderer->drawFloor();
        
        this->renderer->RenderScene(interceptsTallObjects); //draw back objects
        this->renderer->RenderScene(interceptsLowObjects); //draw front objects
        
        if (renderer->debug)
            renderer->drawText("DEBUG: ON", renderer->screenW - 120, 5, 100, 24, 255, 0, 0);
        
        if (showMap)
            this->renderer->drawMap(this->map->currentMap(), this->renderer->mapRays, fPlayerX, fPlayerY, fPlayerA, fMovDir);
        
        this->renderer->renderFrame();
        
        frameCount++;
        
        frameTime = SDL_GetTicks() - startFrame;
        
        if (fps > frameTime)
        {
            SDL_Delay(fps - frameTime);
        }
    }
    
    SDL_DestroyRenderer(this->sdl_renderer);
    SDL_DestroyWindow(this->sdl_window);
    
    SDL_Quit();
}


void Game::collisionDetection(float fOldPlayerX, float fOldPlayerY)
{
    float cameraXDist = map->fMapBlockW / 2;
    float cameraYDist = map->fMapBlockH / 2;
    
    Map::MapCoords mapCoords = map->posToCrd(fPlayerX + cameraXDist, fPlayerY + cameraYDist);
    
    vector<string> map = this->map->currentMap();
    
    if ((fPlayerX > fOldPlayerX && (map[mapCoords.y][mapCoords.x+1] == '#' || map[mapCoords.y][mapCoords.x+1] == '*')) ||
        (fPlayerX < fOldPlayerX && (map[mapCoords.y][mapCoords.x-1] == '#' || map[mapCoords.y][mapCoords.x-1] == '*')))
    {
        fPlayerX = fOldPlayerX;
    }
    
    if ((fPlayerY > fOldPlayerY && (map[mapCoords.y+1][mapCoords.x] == '#' || map[mapCoords.y+1][mapCoords.x] == '*')) ||
        (fPlayerY < fOldPlayerY && (map[mapCoords.y-1][mapCoords.x] == '#' || map[mapCoords.y-1][mapCoords.x] == '*')))
    {
        fPlayerY = fOldPlayerY;
    }
}

void Game::handleMovement()
{
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    fRotDir     = 0.0f;
    strafe      = 0;
    fMovDir     = 0.0f;
    strafeOn    = false;
    fRunAcc     = 0.0f;
    
    if (keyState[SDL_SCANCODE_Q])
        strafeOn = true;
    
    if (keyState[SDL_SCANCODE_W])
        fRunAcc = 2.0f;
    
    if (keyState[SDL_SCANCODE_LEFT])
    {
        fRotDir = -(fRotAcc + (fRunAcc / 2) / 24);
        
        if (strafeOn)
        {
            strafe = -1;
            fRotDir = 0;
        }
    }
    
    if (keyState[SDL_SCANCODE_RIGHT])
    {
        fRotDir = fRotAcc + (fRunAcc / 2) / 24;
        
        if (strafeOn)
        {
            strafe = 1;
            fRotDir = 0;
        }
    }
    
    if (keyState[SDL_SCANCODE_UP])
        fMovDir = fMovAcc + fRunAcc;
    
    if (keyState[SDL_SCANCODE_DOWN])
        fMovDir = -(fMovAcc + fRunAcc);
}
