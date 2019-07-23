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
#include "RendererHelperFunctions.hpp"

using namespace std;

class Renderer {
private:
    SDL_Renderer* _renderer;
    int _wallBlockSize = 64;
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
        double x;
        double y;
        double xTo;
        double yTo;
        double distance;
        int mapX;
        int mapY;
        bool isCorner;
        double corner1Dist, corner2Dist, corner3Dist, corner4Dist;
        int objType;
        int rayIndex;
        double rayAngle;
        int objIndex;
        double hitsXSide;
        double hitsYSide;
    };
    
    struct objectInScreen {
        double distance;
        double rayAngle;
        double objType;
    };
    
    struct qpoint {
        double x1; double y1;
        double x2; double y2;
        double x3; double y3;
        double x4; double y4;
    };
    
    struct linePoints {
        double x, y, xTo, yTo, distance;
        int rayIndex;
    };
    
    vector<linePoints> mapRays;
    vector<Renderer::objectInScreen> globalObjectsRegister;
    
    //functions
    Renderer(int winW, int winH, SDL_Renderer *renderer);
    ~Renderer();
    
    void renderFrame();
    void prepareRender();
    
    void drawMap(vector<string> map, vector<linePoints> mapRays, double fPlayerX, double fPlayerY, double fPlayerA, double fMovDir);
    vector<interceptions> castRays(double x, double y, double angle, vector<string> map, const char scanChar = '*');
    void RenderScene(vector<interceptions> interceptions);
    void TextureMap(SDL_Texture *texture,double srcX, double srcY, double srcW, double srcH, double destW, double destH, double destX, double destY);
    void drawCeil();
    void drawFloor();
    void drawQuadrangles(vector<qpoint> points);
    void drawQuadrangle(qpoint points);
    
    //primitives
    void drawLine(double x, double y, double xTo, double yTo, int r = 255, int g = 255, int b = 255);
    void fillRect(double x, double y, double w, double h, distanceShader shaderColor);
    void drawRect(double x, double y, double w, double h, distanceShader shaderColor);
    void drawText(const char* text, double x, double y, double w, double h, Uint8 r, Uint8 g, Uint8 b);
    
    //coloring
    distanceShader calcDistShaderGrayscale(double distance, int color = 255);
    distanceShader calcDistShaderColor(double distance, int r, int g, int b, int a);
};


#endif /* Renderer_hpp */
