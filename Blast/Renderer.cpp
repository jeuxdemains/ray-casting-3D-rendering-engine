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
    TTF_Init();
   
    this->renderer = renderer;
    this->screenW = winW;
    this->screenH = winH;
}

Renderer::~Renderer()
{
    IMG_Quit();
    TTF_Quit();
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
    double xTo = x;
    double yTo = y;
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
        
        //extend ray till collision
        while (!collision)
        {
            mapX = floor(xTo / mapBlockSizeW);
            mapY = floor(yTo / mapBlockSizeH);
            
            if (map[mapY][mapX] == '#')
            {
                double distX = fabs((x - xTo) * cosf(angle));
                double distY = fabs((y - yTo) * sinf(angle));
                double totalDist = fabs(distX + distY);
                
                //get wall corners coordinates in screen space
                //uses: MapX, MapY, mapBlockSizeW, mapBlockSizeH

                float tileCoordX1 = mapX * mapBlockSizeW;
                float tileCoordY1 = mapY * mapBlockSizeH;
                float tileCoordX2 = ((mapX+1) * mapBlockSizeW);
                float tileCoordY2 = ((mapY+1) * mapBlockSizeH);
                
                float fCornerDist[4] = {0,0,0,0};
                
                bool isCorner = false;
                
                if ((tileCoordX1 == round(xTo) || tileCoordX2 == round(xTo)) &&
                    (tileCoordY1 == round(yTo) || tileCoordY2 == round(yTo)))
                {
                    isCorner = true;
                    
                    float c_distX = fabs(x - tileCoordX1) * cosf(angle);
                    float c_distY = fabs(y - tileCoordY1) * sinf(angle);
                    fCornerDist[0] = fabs(c_distX - c_distY);
                    
                    c_distX = fabs(x - tileCoordX2) * cosf(angle);
                    fCornerDist[1] = fabs(c_distX - c_distY);
                    
                    c_distX = fabs(x - tileCoordX2) * cosf(angle);
                    c_distY = fabs(y - tileCoordY2) * sinf(angle);
                    fCornerDist[2] = fabs(c_distX - c_distY);
                    
                    c_distX = fabs(x - tileCoordX1) * cosf(angle);
                    c_distY = fabs(y - tileCoordY2) * sinf(angle);
                    fCornerDist[3] = fabs(c_distX - c_distY);
                }
                
                Renderer::interceptions intercepts;
                intercepts.x = x;
                intercepts.y = y;
                intercepts.xTo = xTo;
                intercepts.yTo = yTo;
                intercepts.distance = totalDist;
                intercepts.mapX = mapX;
                intercepts.mapY = mapY;
                intercepts.isCorner = isCorner;
                intercepts.objX1 = tileCoordX1;
                intercepts.objY1 = tileCoordY1;
                intercepts.objX2 = tileCoordX2;
                intercepts.objY2 = tileCoordY2;
                intercepts.corner1Dist = fCornerDist[0];
                intercepts.corner2Dist = fCornerDist[1];
                intercepts.corner3Dist = fCornerDist[2];
                intercepts.corner4Dist = fCornerDist[3];
                
                interceptions.push_back(intercepts);
                
                collision = true;
            }
            
            xTo += stepInc * cosf(angle + angleDelta);
            yTo += stepInc * sinf(angle + angleDelta);
        }
        
        //finally draw the line on the map once collision is detected
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
    int map_x_crd_last = 0;
    int map_y_crd_last = 0;

    Renderer::qpoint curQPoint;
    vector<Renderer::qpoint> surfacesPointsList;
    
    for (Renderer::interceptions intercept : interceptions)
    {
        double distance = intercept.distance;
        
        float wallFragH = this->wallHFactor / (distance / this->screenH);
        if (wallFragH < 40)
            wallFragH = 40;
        
        float wallFragY = (this->screenH - wallFragH) / 2;
        float wallFragW = (this->screenW / (interceptions.size() - 1));
        float wallFragX = rayIndex * wallFragW;
        
        fillRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShader(distance));
//        drawRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShader(distance));

        //SET OBJECT'S SURFACES WIREFRAME
        
        //if still at the same wall object keep scanning the surface
        if (map_y_crd_last == intercept.mapY && map_x_crd_last == intercept.mapX)
        {
            curQPoint.x2 = curQPoint.x3 = wallFragX;
            curQPoint.y2 = wallFragY;
            curQPoint.y3 = wallFragY + wallFragH;
            
            if (intercept.isCorner)
            {
                curQPoint.x2 = curQPoint.x3 = wallFragX;

                surfacesPointsList.push_back(curQPoint);

                //prepare new surface
                curQPoint.x1 = curQPoint.x4 = wallFragX;
                curQPoint.y1 = curQPoint.y2 = wallFragY;
                curQPoint.y3 = curQPoint.y4 = wallFragY + wallFragH;
            }
        }
        else //if new obj scan started -- save the surface of the old one
        {
            curQPoint.x2 = curQPoint.x3 = wallFragX;
            
            surfacesPointsList.push_back(curQPoint);
            
            //prepare new surface
            curQPoint.x1 = curQPoint.x4 = wallFragX;
            curQPoint.y1 = curQPoint.y2 = wallFragY;
            curQPoint.y3 = curQPoint.y4 = wallFragY + wallFragH;
        }
        
        rayIndex++;
        map_x_crd_last = intercept.mapX;
        map_y_crd_last = intercept.mapY;
    }
    
    //push the last one
    surfacesPointsList.push_back(curQPoint);
    
    if (debug)
    {
        drawQuadrangles(surfacesPointsList);
    }
}

void Renderer::drawQuadrangles(vector<Renderer::qpoint> points)
{
    for (Renderer::qpoint quadPoint : points) {
        drawQuadrangle(quadPoint);
    }
}

void Renderer::drawQuadrangle(qpoint points)
{
    this->drawLine(points.x1, points.y1, points.x2, points.y2);
    this->drawLine(points.x2, points.y2, points.x3, points.y3);
    this->drawLine(points.x3, points.y3, points.x1, points.y4);
    this->drawLine(points.x1, points.y4, points.x1, points.y1);
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

void Renderer::drawText(const char* text, float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b)
{
    TTF_Font* Sans = TTF_OpenFont("assets/font.ttf", 24);
    if(!Sans)
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    
    SDL_Color color = {r, g, b};
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, color);
    
    SDL_Texture* Message = SDL_CreateTextureFromSurface(this->renderer, surfaceMessage);
    
    SDL_Rect Message_rect;
    Message_rect.x = x;
    Message_rect.y = y;
    Message_rect.w = w;
    Message_rect.h = h;
    
    SDL_RenderCopy(this->renderer, Message, NULL, &Message_rect);
}


//image load and draw in rectangle
//        if (this->wallTexture == nullptr)
//        {
//            SDL_Surface *image = IMG_Load("assets/img.jpg");
//            this->wallTexture = SDL_CreateTextureFromSurface(this->renderer, image);
//        }
//        SDL_RenderCopy(this->renderer, this->wallTexture, &src_rect, &dest_rect);
