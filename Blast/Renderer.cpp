//
//  Renderer.cpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#include "Renderer.hpp"

using namespace std;

#define PI 3.14159f

Renderer::Renderer(int winW, int winH, SDL_Renderer *renderer)
{
    this->renderer = renderer;
    this->screenW = winW;
    this->screenH = winH;
}

Renderer::~Renderer()
{

}

void Renderer::renderFrame()
{
    SDL_RenderPresent(this->renderer);
}

void Renderer::prepareRender()
{
    SDL_SetRenderDrawColor(this->renderer, 0,0,0,255);
    SDL_RenderClear(this->renderer);
}

void Renderer::drawMap(vector<string> map)
{
    Map *classMap = new Map();
    
    float mapBlockSizeW = (float)classMap->fMapHudSizeW / map[0].size();
    float mapBlockSizeH = (float)classMap->fMapHudSizeH / map.size();
    
    for (int mapY = 0; mapY < map.size(); mapY++) {
        for (int mapX = 0; mapX < map[mapY].size(); mapX++) {
            
            //wall
            if (map[mapY][mapX] == '#')
            {
                distanceShader distShader;
                distShader.r = distShader.g = distShader.b = distShader.a = 255;
                this->fillRect(mapX * mapBlockSizeW, mapY * mapBlockSizeH, mapBlockSizeW + 1, mapBlockSizeH, distShader);
            }
            
            if (map[mapY][mapX] == '.')
            {
                distanceShader distShader;
                distShader.r = distShader.g = distShader.b = 0;
                distShader.a = 255;
                this->fillRect(mapX * mapBlockSizeW, mapY * mapBlockSizeH, mapBlockSizeW + 1, mapBlockSizeH, distShader);
            }
        }
    }
}

vector<Renderer::interceptions> Renderer::castRays(int numOfRays,float x, float y, float angle, vector<string> map)
{
    float FOV = 60 / (360 / PI);
    float xTo = x;
    float yTo = y;
    float stepInc = 0.01f;
    int mapX = 0;
    int mapY = 0;
    
    Map *classMap = new Map();
    float mapBlockSizeW = (float)classMap->fMapBlockW;
    float mapBlockSizeH = (float)classMap->fMapBlockH;
    
    vector<Renderer::interceptions> interceptions;
    
    for (float angleDelta = -FOV/2; angleDelta < FOV/2; angleDelta += (FOV / numOfRays)) {
        
        //init new loop values
        bool collision = false;
        xTo = x;
        yTo = y;
        
        //interpolate till collision
        while (!collision)
        {
            mapX = floor(xTo / mapBlockSizeW);
            mapY = floor(yTo / mapBlockSizeH);
            
            if (map[mapY][mapX] == '#')
            {
                float distX = fabs((x - xTo) * cosf(angle));
                float distY = fabs((y - yTo) * sinf(angle));
                float totalDist = fabs(distX + distY);
                
                Renderer::interceptions intercepts;
                intercepts.x = x;
                intercepts.y = y;
                intercepts.xTo = xTo;
                intercepts.yTo = yTo;
                intercepts.distance = totalDist;
                
                interceptions.push_back(intercepts);
                
                collision = true;
            }
            
            xTo += stepInc * cosf(angle + angleDelta);
            yTo += stepInc * sinf(angle + angleDelta);
        }
        
        //finally draw the line once collision is detected
        this->drawLine(x, y, xTo, yTo);
    }
    
    return interceptions;
}

void Renderer::drawLine(float x, float y, float xTo, float yTo)
{
    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(this->renderer, x, y, xTo, yTo);
}

void Renderer::fillRect(float x, float y, float w, float h, distanceShader shaderColor)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    SDL_SetRenderDrawColor(this->renderer, shaderColor.r,shaderColor.g,shaderColor.b, shaderColor.a);
    SDL_RenderFillRect(this->renderer, &rect);
}

void Renderer::drawRect(float x, float y, float w, float h, distanceShader shaderColor)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    SDL_SetRenderDrawColor(this->renderer, shaderColor.r,shaderColor.g,shaderColor.b, shaderColor.a);
    SDL_RenderDrawRect(this->renderer, &rect);
}

void Renderer::draw3dScene(vector<interceptions> interceptions)
{
    int rayIndex = 0;
    
    for (Renderer::interceptions intercept : interceptions)
    {
        float distance = intercept.distance;
        
        float wallFragH = this->wallHFactor / (distance / this->screenH);
        if (wallFragH < 40)
            wallFragH = 40;
        
        float wallFragY = (this->screenH - wallFragH) / 2;
        float wallFragW = (this->screenW / (interceptions.size() - 1));
        float wallFragX = rayIndex * wallFragW;
        
        fillRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShader(distance));
//        drawRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShader(distance));
        
        rayIndex++;
    }
}

Renderer::distanceShader Renderer::calcDistShader(float distance)
{
    distanceShader distS;
    int distShade = 255 - distance * 1.5f;
    if (distShade < 30) distShade = 30;
    
    distS.r =
    distS.g =
    distS.b = distShade;
    distS.a = 250;
    
    return distS;
}
