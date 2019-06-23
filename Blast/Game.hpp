//
//  Game.hpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "Renderer.hpp"
#include "Map.hpp"

class Game {
    
private:
    void createWindow(int w, int h);
    
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    
public:
    Game(int winWidth, int winHeight);
    ~Game();
    
    void GameLoop();
    void collisionDetection(float fOldPlayerX, float fOldPlayerY);
    void handleMovement();
    
    Renderer *renderer = nullptr;
    Map *map = nullptr;
    
    float fPlayerA = 0.0f;
    float fPlayerX = 10.0f;
    float fPlayerY = 10.0f;
    
    float fRotAcc = 0.03f;
    float fMovAcc = 1.0f;
    float fRunAcc = 0.0f;
    float fRotDir = 0.0f;
    float fMovDir = 0.0f;
    
    bool strafeOn = false;
    int strafe = 0;
};

#endif /* Game_hpp */


