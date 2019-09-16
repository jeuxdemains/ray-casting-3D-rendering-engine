//
//  Game.cpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#include "Game.hpp"

#define PI 3.14159265
#define RES_SCALE_FACTOR 1

using namespace std;

Game::Game(int winWidth, int winHeight)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        SDL_Log("unable to initialize SDL2 !");
    
    this->sdl_window = SDL_CreateWindow("Blast Engine",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        winWidth, winHeight, SDL_WINDOW_SHOWN); //SDL_WINDOW_INPUT_GRABBED
    this->sdl_renderer = SDL_CreateRenderer(this->sdl_window, -1, SDL_RENDERER_SOFTWARE);
    
    this->renderer = new Renderer(winWidth / RES_SCALE_FACTOR, winHeight / RES_SCALE_FACTOR, this->sdl_renderer);
    SDL_RenderSetLogicalSize(this->sdl_renderer, winWidth / RES_SCALE_FACTOR, winHeight / RES_SCALE_FACTOR);
    
    this->map = new Map();
	this->GeneratePlayerAnglesMap();
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
		fPlayerA = (double)((int)(fPlayerA*100))/100;
		if (fPlayerA > 6.28f || fPlayerA < -6.28f)
			fPlayerA = 0.0f;
		
        float fOldPlayerX = fPlayerX;
        float fOldPlayerY = fPlayerY;
        
        if (strafeOn)
        {
            fPlayerX += strafe * cosf(fPlayerA + PI / 2);
            fPlayerY += strafe * sinf(fPlayerA + PI / 2);
        }
		
		fPlayerX += fMovDir * playerAngleMapCos[fPlayerA];
		fPlayerY += fMovDir * playerAngleMapSin[fPlayerA];
		
        collisionDetection(fOldPlayerX, fOldPlayerY);
        
        this->renderer->prepareRender();
        this->renderer->globalObjectsRegister.clear();
        
        this->renderer->mapRays.clear();
		
//        vector<Renderer::interceptions> interceptsTallObjects =
//            this->renderer->castRays(fPlayerX, fPlayerY, fPlayerA, map->currentMap());
//
//        vector<Renderer::interceptions> interceptsLowObjects =
//            this->renderer->castRays(fPlayerX, fPlayerY, fPlayerA, map->currentMap(), '#');
//
		vector<vector<Renderer::interceptions>> interceptsHiLo =
		this->renderer->castRays(fPlayerX, fPlayerY, fPlayerA, map->currentMap());
		
        this->renderer->drawCeil();
        this->renderer->drawFloor();
        
        this->renderer->RenderScene(interceptsHiLo[0]); //draw back objects
        this->renderer->RenderScene(interceptsHiLo[1]); //draw front objects
		
        if (renderer->debug)
            DebugInfo();
        
        if (showMap)
            this->renderer->drawMap(this->map->currentMap(), this->renderer->mapRays, fPlayerX, fPlayerY, fPlayerA, fMovDir);
        
        this->renderer->renderFrame();
        
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

void Game::GeneratePlayerAnglesMap()
{
	for (float angle = -6.28f; angle <= 6.28f; angle += 0.01f) {
		float rounded = (double)((int)(angle*100))/100;
		playerAngleMapCos[rounded] = cosf(rounded);
		playerAngleMapSin[rounded] = sinf(rounded);
	}
}

void Game::DebugInfo()
{
    double x, y, lineH;
    x = renderer->screenW;
    y = 5;
    lineH = 23.0f;
    char buffer[128];
    double str_w = 0.0f;
    
    renderer->drawText("DEBUG: ON", x - 120, y, 100, lineH, 255, 0, 0);
    
    //cameraX info
    y += lineH;
    str_w = sprintf(buffer, "posX: %f", fPlayerX);
    str_w *= lineH / 2.5f;
    renderer->drawText(buffer, x - str_w, y, str_w , lineH, 255, 0, 0);
    
    //cameraY info
    y += lineH;
    str_w = sprintf(buffer, "posY: %f", fPlayerY);
    str_w *= lineH / 2.5f;
    renderer->drawText(buffer, x - str_w, y, str_w , lineH, 255, 0, 0);
    
    //camera angle info
    y += lineH;
    str_w = sprintf(buffer, "posA: %f", fPlayerA);
    str_w *= lineH / 2.5f;
    renderer->drawText(buffer, x - str_w, y, str_w , lineH, 255, 0, 0);
    
    //map info
    Map::MapCoords mapCoords = map->posToCrd(fPlayerX, fPlayerY);
    y += lineH;
    str_w = sprintf(buffer, "MapX: %d, MapY: %d", mapCoords.x, mapCoords.y);
    str_w *= lineH / 2.5f;
    renderer->drawText(buffer, x - str_w, y, str_w , lineH, 255, 0, 0);
}


void Game::collisionDetection(float fOldPlayerX, float fOldPlayerY)
{
    float cameraXDist = map->fMapBlockSize / 2;
    float cameraYDist = map->fMapBlockSize / 2;

    Map::MapCoords mapCoords = map->posToCrd(fPlayerX + cameraXDist, fPlayerY + cameraYDist);
    
    vector<string> map = this->map->currentMap();
    
    if ((fPlayerX > fOldPlayerX && (map[mapCoords.y][mapCoords.x+1] != '.')) ||
        (fPlayerX < fOldPlayerX && (map[mapCoords.y][mapCoords.x-1] != '.')) ||
		mapCoords.x < 1)
    {
        fPlayerX = fOldPlayerX;
    }
    
    if ((fPlayerY > fOldPlayerY && (map[mapCoords.y+1][mapCoords.x] != '.')) ||
        (fPlayerY < fOldPlayerY && (map[mapCoords.y-1][mapCoords.x] != '.')) ||
		mapCoords.y < 1)
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
	
	float fRunAccAmp = 2.5f;
    
    if (keyState[SDL_SCANCODE_Q])
        strafeOn = true;
    
    if (keyState[SDL_SCANCODE_W])
        fRunAcc = fRunAccAmp;
	
	if (keyState[SDL_SCANCODE_S])
		fRunAcc = -0.5f;
    
    if (keyState[SDL_SCANCODE_LEFT])
    {
        fRotDir = -(fRotAcc + (fRunAcc / 4) / 24);
        
        if (strafeOn)
        {
            strafe = -1;
            fRotDir = 0;
        }
    }
    
    if (keyState[SDL_SCANCODE_RIGHT])
    {
        fRotDir = fRotAcc + (fRunAcc / 4) / 24;
        
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
