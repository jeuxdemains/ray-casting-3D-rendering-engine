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
   
    _renderer = renderer;
    this->screenW = winW;
    this->screenH = winH;
    
    SDL_Surface *image = IMG_Load("assets/ceil.jpg");
    _ceilTexture = SDL_CreateTextureFromSurface(_renderer, image);
    image = IMG_Load("assets/floor.jpg");
    _floorTexture = SDL_CreateTextureFromSurface(_renderer, image);
    image = IMG_Load("assets/bricks.png");
    _wallTexture = SDL_CreateTextureFromSurface(_renderer, image);
    
    _font = TTF_OpenFont("assets/font.ttf", 24);
    if(!_font)
        printf("TTF_OpenFont: %s\n", TTF_GetError());
}

Renderer::~Renderer()
{
    IMG_Quit();
    TTF_Quit();
}

void Renderer::renderFrame()
{
    SDL_RenderPresent(_renderer);
}

void Renderer::prepareRender()
{
    SDL_SetRenderDrawColor(_renderer, 0,0,0,255);
    SDL_RenderClear(_renderer);
}

void Renderer::drawMap(vector<string> map, vector<linePoints> mapRays, double fPlayerX, double fPlayerY, double fPlayerA, double fMovDir)
{
    Map *classMap = new Map();
    
    double mapBlockSizeW = (double)classMap->fMapHudSizeW / map[0].size();
    double mapBlockSizeH = (double)classMap->fMapHudSizeH / map.size();
    
    for (int mapY = 0; mapY < map.size(); mapY++) {
        for (int mapX = 0; mapX < map[mapY].size(); mapX++) {
            
            //wall
            if (map[mapY][mapX] == '#' || map[mapY][mapX] == '*')
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
    
    //draw player on the map
    distanceShader distShader;
    //196, 196, 27
    distShader.r = distShader.g = 196; distShader.b = 27;
    distShader.a = 255;
    this->drawRect(fPlayerX, fPlayerY, mapBlockSizeW + 1, mapBlockSizeH, distShader);
    
    //draw rays
    if (debug)
    {
        for (linePoints points : mapRays)
        {
            drawLine(points.x, points.y, points.xTo, points.yTo);
        }
    }
   
    //draw player direction on the map
    double lineX = fPlayerX + mapBlockSizeH / 2;
    double lineY = fPlayerY + mapBlockSizeW / 2;
    double lineX2 = fPlayerX + 8 * cosf(fPlayerA);
    lineX2 += mapBlockSizeH / 2;
    double lineY2 = fPlayerY + 8 * sinf(fPlayerA);
    lineY2 += mapBlockSizeW / 2;
    drawLine(lineX, lineY, lineX2, lineY2, 196, 196, 27);
}

vector<Renderer::interceptions> Renderer::castRays(double x, double y, double angle, vector<string> map, const char scanChar)
{
    Map *classMap = new Map();
    double mapBlockSizeW = (double)classMap->fMapBlockW;
    double mapBlockSizeH = (double)classMap->fMapBlockH;
    
    RendererHelperFunctions *renderHelper = new RendererHelperFunctions();
    
    double FOV = 60 / (360 / PI);
    
    x += mapBlockSizeW / 2;
    y += mapBlockSizeH / 2;
    double xTo = x;
    double yTo = y;

    double stepInc = 0.2f;
    int mapX = 0;
    int mapY = 0;
    
    vector<Renderer::interceptions> interceptions;
    
    int screenColumn = 0;
    
    double angleDelta = -FOV/2;
    int scanField = this->screenW / _resolution_factor;
    
    int objIndex = 0;
    int lastObjX = 0;
    int lastObjY = 0;
    
    for (int screenX = 0; screenX <= scanField; screenX++) {
        
        //init new loop values
        bool collision = false;
        xTo = x;
        yTo = y;
        angleDelta += (FOV / scanField);
        
        //extend ray till collision
        while (!collision)
        {
            mapX = floor(xTo / mapBlockSizeW);
            mapY = floor(yTo / mapBlockSizeH);
            
            //default scanChar = '*'
            if (map[mapY][mapX] == scanChar)
            {
                int objType = 1;
                
                double totalDist = renderHelper->objDistFromCamera(x, xTo, y, yTo, angle);
                
                bool isCorner =
                renderHelper->isRayHitsCorner(mapX * mapBlockSizeW,
                                              mapY * mapBlockSizeH,
                                              (mapX+1) * mapBlockSizeW,
                                              (mapY+1) * mapBlockSizeH,
                                              xTo, yTo);
            
                
                if (map[mapY][mapX] == '#')
                {   //if object already registered in the global register then skip it
                    for (Renderer::objectInScreen object : globalObjectsRegister)
                    {   
                        if (object.rayAngle == (angle + angleDelta) &&
                            object.objType == 1 &&
                            object.distance < totalDist)
                        {
                            goto skip_object;
                        }
                    }
                    objType = 0;
                }
                
                if (mapX != lastObjX || mapY != lastObjY)
                    objIndex++;
            
                
                bool rayDirRight = false;
                if (x < xTo)
                    rayDirRight = true;
                
                bool rayDirDown = false;
                if (y < yTo)
                    rayDirDown = true;
                
                
                Renderer::interceptions intercepts;
                intercepts.x = x;
                intercepts.y = y;
                intercepts.xTo = xTo;
                intercepts.yTo = yTo;
                intercepts.distance = totalDist;
                intercepts.mapX = mapX;
                intercepts.mapY = mapY;
                intercepts.isCorner = isCorner;
                intercepts.objType = objType;
                intercepts.rayIndex = screenColumn;
                intercepts.rayAngle = angle + angleDelta;
                intercepts.objIndex = objIndex;
                intercepts.hitsXSide = xTo - mapX * mapBlockSizeW;
                intercepts.hitsYSide = yTo - mapY * mapBlockSizeH;
                
                interceptions.push_back(intercepts);
                
                Renderer::objectInScreen object;
                object.rayAngle = angle + angleDelta;
                object.objType = objType;
                object.distance = totalDist;
                
                globalObjectsRegister.push_back(object);
                
                screenColumn++;
                collision = true;
            }
            
        skip_object:
            
            xTo += stepInc * cosf(angle + angleDelta);
            yTo += stepInc * sinf(angle + angleDelta);
            
            //if the ray goes outside the map -> register out of bounds hit
            if (round(xTo) / mapBlockSizeW >= map[0].size() ||
                round(yTo) / mapBlockSizeH >= map.size()||
                round(yTo) / mapBlockSizeH <= 0 ||
                round(xTo) / mapBlockSizeW <= 0)
            {

                Renderer::interceptions intercepts;
                intercepts.distance = -1;
                intercepts.rayIndex = screenColumn;
                interceptions.push_back(intercepts);

                screenColumn++;
                collision = true;
            }
        }
        
        //finally save the line on the map once collision is detected
        double distX = fabs(x - xTo) * cosf(angle);
        double distY = fabs(y - yTo) * sinf(angle);
        double fPointsDist = fabs(distX + distY);
        
        linePoints lPoints;
        lPoints.x = x;
        lPoints.y = y;
        lPoints.xTo = xTo;
        lPoints.yTo = yTo;
        lPoints.rayIndex = screenColumn-1;
        lPoints.distance = fPointsDist;
        
        //if second iteration (smaller objects)
        if (mapRays.size() > scanField)
        {
            if (mapRays[screenColumn-1].distance > fPointsDist)
            {
                mapRays.at(screenColumn-1) = lPoints;
            }
        }
        else
        {
            mapRays.push_back(lPoints);
        }
        
        lastObjX = mapX;
        lastObjY = mapY;
    }
    
    return interceptions;
}

void Renderer::drawLine(double x, double y, double xTo, double yTo, int r, int g, int b)
{
    if (debug)
    {
        r = 255; g = b = 0;
    }
    
    SDL_SetRenderDrawColor(_renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(_renderer, x, y, xTo, yTo);
}

void Renderer::fillRect(double x, double y, double w, double h, distanceShader shaderColor)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    SDL_SetRenderDrawColor(_renderer, shaderColor.r,shaderColor.g,shaderColor.b, shaderColor.a);
    SDL_RenderFillRect(_renderer, &rect);
}

void Renderer::drawRect(double x, double y, double w, double h, distanceShader shaderColor)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    SDL_SetRenderDrawColor(_renderer, shaderColor.r,shaderColor.g,shaderColor.b, shaderColor.a);
    SDL_RenderDrawRect(_renderer, &rect);
}

void Renderer::RenderScene(vector<interceptions> interceptions)
{
    int map_x_crd_last = 0;
    int map_y_crd_last = 0;
    
    bool initNewObject = true;
    
    Map *classMap = new Map();
    
    Renderer::qpoint curQPoint;
    vector<Renderer::qpoint> surfacesPointsList;
    
    for (Renderer::interceptions intercept : interceptions)
    {
        if (intercept.distance == -1)
        {
            initNewObject = true;
            continue;
        }
        
        double wallFragH = _wallBlockSize / ((intercept.distance / screenH / 2) * _resolution_factor);
        if (wallFragH < 40)
            wallFragH = 40;
        
        double wallFragY = (this->screenH - wallFragH) / 2;
        double wallFragW = (this->screenW / _wallBlockSize);
        double wallFragX = intercept.rayIndex * wallFragW;
        
        if (intercept.objType == 1)
        {
            wallFragH *= 2;
            wallFragY -= wallFragH / 2;
        }
        
        fillRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShaderGrayscale(intercept.distance));
        
//        drawRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShaderGrayscale(intercept.distance));

        //SET OBJECT'S SURFACES WIREFRAME
        if (initNewObject)
        {
            curQPoint.x1 = curQPoint.x4 = wallFragX;
            curQPoint.y1 = curQPoint.y2 = wallFragY;
            curQPoint.y3 = curQPoint.y4 = wallFragY + wallFragH;
            initNewObject = false;
        }

        //if still at the same wall object keep scanning the surface
        if (map_y_crd_last == intercept.mapY && map_x_crd_last == intercept.mapX)
        {
            curQPoint.x2 = curQPoint.x3 = wallFragX;
            curQPoint.y2 = wallFragY;
            curQPoint.y3 = wallFragY + wallFragH;

//            if (intercept.isCorner)
//            {
//                goto save_surface;
//            }
        }
        else //save the surface and rise the flag to start new object wireframe in the next run
        {
            
            double wallW = _wallBlockSize / (intercept.distance / this->screenW * 4);
            double wallH = _wallBlockSize / (intercept.distance / this->screenH * 4);
            double wallX = intercept.rayIndex * (this->screenW / (interceptions.size() - 1));
            double wallY = (this->screenH - wallH) / 2;
            
            if (intercept.objType == 1)
            {
                wallH *= 2;
                wallY -= wallH / 2;
            }
            
            struct mapBlock
            {
                double x, y, x2, y2;
            };
            
            mapBlock mp;
            mp.x = interceptions[0].mapX * classMap->fMapBlockW;
            mp.y = interceptions[0].mapY * classMap->fMapBlockH;
            mp.x2 = interceptions[0].mapX * classMap->fMapBlockW + classMap->fMapBlockW;
            mp.y2 = interceptions[0].mapY * classMap->fMapBlockH + classMap->fMapBlockH;
            
//            cout << "MAP X1 " << mp.x << " MAP X2 " << mp.x2 << " MAP Y1 " << mp.y << " MAP Y2 " << mp.y2 << endl;
//            cout << "X " << interceptions[0].xTo << " Y " << interceptions[0].yTo << endl;
//            cout << "xSide val: " << interceptions[0].hitsXSide << " ySide val: " << interceptions[0].hitsYSide << endl;
            
//            drawLine(wallX, wallY, wallX+wallW, wallY, 255,0,0);
            
        save_surface:
            curQPoint.x2 = curQPoint.x3 = wallFragX;
            surfacesPointsList.push_back(curQPoint);

            initNewObject = true;
        }
        
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

void Renderer::TextureMap(SDL_Texture *texture, double srcX, double srcY, double srcW, double srcH, double destW, double destH, double destX, double destY)
{
    SDL_Rect srcRect;
    srcRect.x = srcX;
    srcRect.y = srcY;
    srcRect.w = srcW;
    srcRect.h = srcH;
    
    SDL_Rect destRect;
    destRect.x = destX;
    destRect.y = destY;
    destRect.w = destW;
    destRect.h = destH;
    
    SDL_RenderCopy(_renderer, texture, &srcRect, &destRect);
}

void Renderer::drawCeil()
{
    SDL_Rect dest_rect;
    dest_rect.x = dest_rect.y = 0;
    dest_rect.w = this->screenW;
    dest_rect.h = this->screenH / 2;
    
    if (_ceilTexture == nullptr)
    {
        SDL_Surface *image = IMG_Load("assets/ceil.jpg");
        _ceilTexture = SDL_CreateTextureFromSurface(_renderer, image);
    }
    
    SDL_RenderCopy(_renderer, _ceilTexture, NULL, &dest_rect);
}

void Renderer::drawFloor()
{
    SDL_Rect dest_rect;
    dest_rect.x = 0;
    dest_rect.y = this->screenH / 2;
    dest_rect.w = this->screenW;
    dest_rect.h = this->screenH / 2;
    
    if (_floorTexture == nullptr)
    {
        SDL_Surface *image = IMG_Load("assets/floor.jpg");
        _floorTexture = SDL_CreateTextureFromSurface(_renderer, image);
    }
    
    SDL_RenderCopy(_renderer, _floorTexture, NULL, &dest_rect);
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

Renderer::distanceShader Renderer::calcDistShaderGrayscale(double distance, int color)
{
    distanceShader distS;
    int distShade = color - distance * 1.5f;
    if (distShade < 30) distShade = 30;
    
    distS.r =
    distS.g =
    distS.b = distShade;
    distS.a = 250;
    
    return distS;
}

Renderer::distanceShader Renderer::calcDistShaderColor(double distance, int r, int g, int b, int a)
{
    distanceShader distS;
    int distShade = a - distance * 1.5f;
    if (distShade < 30) distShade = 30;
    
    distS.r = r;
    distS.g = g;
    distS.b = b;
    distS.a = a;
    
    return distS;
}

void Renderer::drawText(const char* text, double x, double y, double w, double h, Uint8 r, Uint8 g, Uint8 b)
{
    
    SDL_Color color = {r, g, b};
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(_font, text, color);
    
    SDL_Texture* Message = SDL_CreateTextureFromSurface(_renderer, surfaceMessage);
    
    SDL_Rect Message_rect;
    Message_rect.x = x;
    Message_rect.y = y;
    Message_rect.w = w;
    Message_rect.h = h;
    
    SDL_RenderCopy(_renderer, Message, NULL, &Message_rect);
}


//image load and draw in rectangle
//        if (this->wallTexture == nullptr)
//        {
//            SDL_Surface *image = IMG_Load("assets/img.jpg");
//            this->wallTexture = SDL_CreateTextureFromSurface(_renderer, image);
//        }
//        SDL_RenderCopy(_renderer, this->wallTexture, &src_rect, &dest_rect);
