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
    image = IMG_Load("assets/box2.jpg");
    _boxTexture = SDL_CreateTextureFromSurface(_renderer, image);
    
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
    
    float mapBlockSizeW = classMap->fMapBlockW; //classMap->fMapHudSizeW / map[0].size();
    float mapBlockSizeH = classMap->fMapBlockH; //classMap->fMapHudSizeH / map.size();
    
    for (int mapY = 0; mapY < map.size(); mapY++) {
        for (int mapX = 0; mapX < map[mapY].size(); mapX++) {
            
            //wall
            if (map[mapY][mapX] == '#' || map[mapY][mapX] == '*')
            {
                distanceShader distShader;
                distShader.r = distShader.g = distShader.b = distShader.a = 255;
                this->fillRect(mapX * mapBlockSizeW, mapY * mapBlockSizeH, mapBlockSizeW + 1, mapBlockSizeH + 1, distShader);
            }
            
            if (map[mapY][mapX] == '.')
            {
                distanceShader distShader;
                distShader.r = distShader.g = distShader.b = 0;
                distShader.a = 255;
                this->fillRect(mapX * mapBlockSizeW, mapY * mapBlockSizeH, mapBlockSizeW + 1, mapBlockSizeH + 1, distShader);
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
    float mapBlockSizeW = classMap->fMapBlockW;
    float mapBlockSizeH = classMap->fMapBlockH;
    
    RendererHelperFunctions *renderHelper = new RendererHelperFunctions();
    
    double FOV = 60 / (360 / PI);

    x += mapBlockSizeW / 2;
    y += mapBlockSizeH / 2;
    
    double xTo = x;
    double yTo = y;

    double stepInc = 0.2f;
    double mapX = 0;
    double mapY = 0;
    
    vector<Renderer::interceptions> interceptions;
    
    int screenColumn = 0;
    
    double angleDelta = -FOV/2;
    int scanField = this->screenW / _resolution_factor;
    
    int lastObjX = 0;
    int lastObjY = 0;
    int lastCornerX = 0;
    int lastCornerY = 0;
    
    for (int screenX = 0; screenX <= scanField; screenX++) {
        
        //init new loop values
        bool collision = false;
        xTo = x;
        yTo = y;
        angleDelta += (FOV / scanField);
        
        //extend ray till collision
        while (!collision)
        {
            mapX = int(xTo / mapBlockSizeW);
            mapY = int(yTo / mapBlockSizeH);
            
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
                                              x, y,
                                              xTo, yTo);
            
                
//                if (isCorner && round(xTo) == lastCornerX && round(yTo) == lastCornerY)
//                {
//                    isCorner = false;
//                }
                
                lastCornerX = round(xTo);
                lastCornerY = round(yTo);
                
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
            if (ceil(xTo) / mapBlockSizeW >= map[0].size() ||
                ceil(yTo) / mapBlockSizeH >= map.size() ||
                floor(yTo) / mapBlockSizeH <= 0 ||
                floor(xTo) / mapBlockSizeW <= 0)
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
    
    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
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
    
    Renderer::qpoint curQPoint;
    vector<Renderer::qpoint> surfacesPointsList;

    wallSliceStruct wallSlice;
    vector<wallSliceStruct> wallSlices;
    
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
        
//        fillRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShaderGrayscale(intercept.distance));
//        drawRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShaderGrayscale(intercept.distance));
        
        int mapX_screen_to = (intercept.mapX + 1) * _wallBlockSize;
        int mapX_screen_from = mapX_screen_to - _wallBlockSize;
        
        int mapY_screen_to = (intercept.mapY + 1) * _wallBlockSize;
        int mapY_screen_from = mapY_screen_to - _wallBlockSize;
        
        int xto_screen = intercept.xTo * 10;
        int yto_screen = intercept.yTo * 10;
        
        int wallOffset = 0;
        
        if (yto_screen + 5 >= mapY_screen_to)
            wallOffset = mapX_screen_to - xto_screen;
        if (mapY_screen_from >= yto_screen - 4)
            wallOffset = 1 + xto_screen - mapX_screen_from;
        
        if (mapX_screen_from >= xto_screen - 4)
            wallOffset = mapY_screen_to - yto_screen;
        if (mapX_screen_to <= xto_screen + 5)
            wallOffset = 1 + yto_screen - mapY_screen_from;


        if (intercept.objType == 0)
        cout
        << "mapX coord: " << intercept.mapX + 1
        << " | mapX_screen_from: " << mapX_screen_from
        << " | mapX_screen_to: " << mapX_screen_to
        << " | xTo: " << xto_screen + 1 << endl
        << "mapY coord: " << intercept.mapY + 1
        << " | mapY_screen_from: " << mapY_screen_from
        << " | mapY_screen_to: " << mapY_screen_to
        << " | yTo: " << yto_screen + 1 << endl
        << " wallOffset: " << wallOffset << endl
        << "-----------------------------"
        << endl;

        //SET OBJECT'S SURFACES WIREFRAME
        if (initNewObject)
        {
            curQPoint.x1 = curQPoint.x4 = wallFragX;
            curQPoint.y1 = curQPoint.y2 = wallFragY;
            curQPoint.y3 = curQPoint.y4 = wallFragY + wallFragH;
            initNewObject = false;
            
            wallSlices.clear();
            
            wallSlice.x = wallFragX;
            wallSlice.y = wallFragY;
            wallSlice.w = wallFragW;
            wallSlice.h = wallFragH;
            wallSlice.distance = intercept.distance;
            wallSlice.objType = intercept.objType;
            wallSlice.texOffset = wallOffset;
            
            wallSlices.push_back(wallSlice);
            
        }

        //if still at the same wall object keep scanning the surface
        if (map_y_crd_last == intercept.mapY && map_x_crd_last == intercept.mapX)
        {
            curQPoint.x2 = curQPoint.x3 = wallFragX;
            curQPoint.y2 = wallFragY;
            curQPoint.y3 = wallFragY + wallFragH;
            
            wallSlice.x = wallFragX;
            wallSlice.y = wallFragY;
            wallSlice.w = wallFragW;
            wallSlice.h = wallFragH;
            wallSlice.distance = intercept.distance;
            wallSlice.objType = intercept.objType;
            wallSlice.texOffset = wallOffset;
            
            wallSlices.push_back(wallSlice);

            if (intercept.isCorner)
            {
                goto render_wall;
            }
        }
        else //save the surface and rise the flag to start new object wireframe in the next run
        {
        render_wall:
            //---------
            wallSlice.x = wallFragX;
            wallSlice.y = wallFragY;
            wallSlice.w = wallFragW;
            wallSlice.h = wallFragH;
            wallSlice.distance = intercept.distance;
            wallSlice.objType = intercept.objType;
            wallSlice.texOffset = wallOffset;
            
            wallSlices.push_back(wallSlice);
            
            DrawWallTexture(wallSlices);
            //---------
            
            curQPoint.x2 = curQPoint.x3 = wallFragX;
            surfacesPointsList.push_back(curQPoint);

            initNewObject = true;
        }
        
        map_x_crd_last = intercept.mapX;
        map_y_crd_last = intercept.mapY;
    }
    
    //push the last one
    surfacesPointsList.push_back(curQPoint);
    
    DrawWallTexture(wallSlices);
    
    if (debug)
    {
        drawQuadrangles(surfacesPointsList);
    }
}

void Renderer::DrawWallTexture(vector<wallSliceStruct> wallSlices)
{
    if (wallSlices.size() == 0)
        return;
    
    int texSliceW = _wallBlockSize;
    SDL_Texture* texture = nullptr;
    
    switch (wallSlices[0].objType) {
        case 0:
            texture = _boxTexture;
            break;
        case 1:
        default:
            texture = _wallTexture;
            break;
    }
    
    if (wallSlices.size() > 1)
        texSliceW = _wallBlockSize / wallSlices.size() + 1;
    
    for (wallSliceStruct ws : wallSlices)
    {
        TextureMap(texture,
                   _wallBlockSize - ws.texOffset, 0,
                   texSliceW, _wallBlockSize,
                   ws.w, ws.h,
                   ws.x, ws.y);
        
        //shade
        distanceShader ds;
        ds.r = ds.g = ds.b = 0;
        ds.a = (ws.distance > 240) ? 240 : ws.distance;
        fillRect(ws.x, ws.y, ws.w, ws.h, ds);
    }
}

void Renderer::TextureMap(SDL_Texture *texture,
                          double srcX, double srcY,
                          double srcW, double srcH,
                          double destW, double destH,
                          double destX, double destY)
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
