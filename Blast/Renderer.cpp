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
    IMG_Init(IMG_INIT_PNG);
   
    this->renderer = renderer;
    this->screenW = winW;
    this->screenH = winH;
}

Renderer::~Renderer()
{
    IMG_Quit();
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
                intercepts.mapX = mapX;
                intercepts.mapY = mapY;
                
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
    int r, g, b;
    r = g = b = 255;
    
    if (debug)
    {
        r = 255; g = b = 0;
    }
    
    SDL_SetRenderDrawColor(this->renderer, r, g, b, SDL_ALPHA_OPAQUE);
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
    SDL_Rect src_rect, dest_rect;
    int lastMapXObjCrd = 0;
    int lastMapYObjCrd = 0;
    
    struct qpoint {
        float x1;
        float y1;
        float x2;
        float y2;
        float x3;
        float y3;
        float x4;
        float y4;
    };
    
    qpoint curQuadPoint;
    vector<qpoint> qPoint;
    
    for (Renderer::interceptions intercept : interceptions)
    {
        float distance = intercept.distance;
        
        float wallFragH = this->wallHFactor / (distance / this->screenH);
        if (wallFragH < 40)
            wallFragH = 40;
        
        float wallFragY = (this->screenH - wallFragH) / 2;
        float wallFragW = (this->screenW / (interceptions.size() - 1));
        float wallFragX = rayIndex * wallFragW;
        
//        if (this->wallTexture == nullptr)
//        {
//            SDL_Surface *image = IMG_Load("assets/img.jpg");
//            this->wallTexture = SDL_CreateTextureFromSurface(this->renderer, image);
//        }
        
//        src_rect.x = wallFragX;
//        src_rect.y = wallFragY;
//        src_rect.w = wallFragW;
//        src_rect.h = wallFragH;
//
//        dest_rect.x = wallFragX;
//        dest_rect.y = wallFragY;
//        dest_rect.w = wallFragW;
//        dest_rect.h = wallFragH;
        
//        this->textureRect(this->wallTexture, rect);
//        SDL_RenderCopy(this->renderer, this->wallTexture, &src_rect, &dest_rect);
        fillRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShader(distance));
//        drawRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShader(distance));
        
        //if still at the same wall object
        if (lastMapYObjCrd == intercept.mapY && lastMapXObjCrd == intercept.mapX)
        {
            curQuadPoint.x2 = wallFragX;
            curQuadPoint.y2 = wallFragY;
            curQuadPoint.x3 = wallFragX;
            curQuadPoint.y3 = wallFragY + wallFragH;
        }
        else //draw the surface
        {
            curQuadPoint.x2 = wallFragX;
            curQuadPoint.x3 = wallFragX;
            
            if (debug)
            {
                this->drawLine(curQuadPoint.x1, curQuadPoint.y1, curQuadPoint.x2, curQuadPoint.y2);
                this->drawLine(curQuadPoint.x2, curQuadPoint.y2, curQuadPoint.x3, curQuadPoint.y3);
                this->drawLine(curQuadPoint.x3, curQuadPoint.y3, curQuadPoint.x1, curQuadPoint.y4);
                this->drawLine(curQuadPoint.x1, curQuadPoint.y4, curQuadPoint.x1, curQuadPoint.y1);
            }
            
            //prepare new surface
            curQuadPoint.x1 = wallFragX;
            curQuadPoint.y1 = wallFragY;
            curQuadPoint.y2 = wallFragY;
            curQuadPoint.y3 = wallFragY + wallFragH;
            curQuadPoint.y4 = wallFragY + wallFragH;
            curQuadPoint.x4 = wallFragX;
        }
        
        rayIndex++;
        lastMapXObjCrd = intercept.mapX;
        lastMapYObjCrd = intercept.mapY;
    }
    
    if (debug)
    {
        this->drawLine(curQuadPoint.x1, curQuadPoint.y1, curQuadPoint.x2, curQuadPoint.y2);
        this->drawLine(curQuadPoint.x2, curQuadPoint.y2, curQuadPoint.x3, curQuadPoint.y3);
        this->drawLine(curQuadPoint.x3, curQuadPoint.y3, curQuadPoint.x1, curQuadPoint.y4);
        this->drawLine(curQuadPoint.x1, curQuadPoint.y4, curQuadPoint.x1, curQuadPoint.y1);
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

void Renderer::textureRect(SDL_Texture *texture, SDL_Rect rect)
{
    SDL_RenderCopy(this->renderer, this->wallTexture, NULL, &rect);
}
