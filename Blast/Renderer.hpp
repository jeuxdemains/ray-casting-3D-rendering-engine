//
//  Renderer.hpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "Map.hpp"

using namespace std;

class Renderer {
private:
    SDL_Renderer *renderer;
    int screenW, screenH;
    int wallHFactor = 10;
    
    SDL_Texture *wallTexture = nullptr;
    
public:
    Renderer(int winW, int winH, SDL_Renderer *renderer);
    ~Renderer();
    
    struct interceptions {
        float x;
        float y;
        float xTo;
        float yTo;
        float distance;
        int mapX;
        int mapY;
    };
    
    struct distanceShader {
        int r;
        int g;
        int b;
        int a;
    };
    
    void renderFrame();
    void prepareRender();
    
    void drawMap(vector<string> map);
    vector<interceptions> castRays(int numOfRays,float x, float y, float angle, vector<string> map);
    void draw3dScene(vector<interceptions> interceptions);
    
    //helper functions
    void textureRect(SDL_Texture *texture, SDL_Rect rect);
    
    //primitives
    void drawLine(float x, float y, float xTo, float yTo);
    void fillRect(float x, float y, float w, float h, distanceShader shaderColor);
    void drawRect(float x, float y, float w, float h, distanceShader shaderColor);
    
    //coloring
    distanceShader calcDistShader(float distance);
    
    bool debug = false;
};

#endif /* Renderer_hpp */
