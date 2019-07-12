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
#include <SDL2_ttf/SDL_ttf.h>

#include "Map.hpp"

using namespace std;

class Renderer {
private:
    SDL_Renderer* _renderer;
    int _wallHFactor = 10;
    int _resolution_factor = 10;
    
    SDL_Texture* _wallTexture = nullptr;
    SDL_Texture* _ceilTexture = nullptr;
    SDL_Texture* _floorTexture = nullptr;
    
    TTF_Font* _font = nullptr;
    
    struct distanceShader {
        int r;
        int g;
        int b;
        int a;
    };
    
public:
    //vars
    bool debug = false;
    int screenW, screenH;
    
    struct interceptions {
        float x;
        float y;
        float xTo;
        float yTo;
        float distance;
        int mapX;
        int mapY;
        bool isCorner;
        float objX1, objX2, objY1, objY2;
        float corner1Dist, corner2Dist, corner3Dist, corner4Dist;
        int objType;
        int rayIndex;
        float rayAngle;
    };
    
    struct objectInScreen {
        float distance;
        float rayAngle;
        float objType;
    };
    
    struct qpoint {
        float x1; float y1;
        float x2; float y2;
        float x3; float y3;
        float x4; float y4;
    };
    
    struct linePoints {
        float x, y, xTo, yTo, distance;
        int rayIndex;
    };
    
    vector<linePoints> mapRays;
    vector<Renderer::objectInScreen> globalObjectsRegister;
    
    //functions
    Renderer(int winW, int winH, SDL_Renderer *renderer);
    ~Renderer();
    
    void renderFrame();
    void prepareRender();
    
    void drawMap(vector<string> map, vector<linePoints> mapRays, float fPlayerX, float fPlayerY, float fPlayerA, float fMovDir);
    vector<interceptions> castRays(float x, float y, float angle, vector<string> map, const char scanChar = '*');
    void RenderScene(vector<interceptions> interceptions);
    void drawCeil();
    void drawFloor();
    void drawQuadrangles(vector<qpoint> points);
    void drawQuadrangle(qpoint points);
    
    //primitives
    void drawLine(float x, float y, float xTo, float yTo, int r = 255, int g = 255, int b = 255);
    void fillRect(float x, float y, float w, float h, distanceShader shaderColor);
    void drawRect(float x, float y, float w, float h, distanceShader shaderColor);
    void drawText(const char* text, float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b);
    
    //coloring
    distanceShader calcDistShaderGrayscale(float distance, int color = 255);
    distanceShader calcDistShaderColor(float distance, int r, int g, int b, int a);
};


#endif /* Renderer_hpp */
