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
    image = IMG_Load("assets/bricks2.jpg");
    _wallTexture = SDL_CreateTextureFromSurface(_renderer, image);
    image = IMG_Load("assets/box2.jpg");
    _boxTexture = SDL_CreateTextureFromSurface(_renderer, image);
	image = IMG_Load("assets/box3.jpg");
    _boxTexture2 = SDL_CreateTextureFromSurface(_renderer, image);
	
    _font = TTF_OpenFont("assets/font.ttf", 12);
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
    
    float mapBlockSizeW = classMap->fMapHudSizeW / map[0].size();
    float mapBlockSizeH = classMap->fMapHudSizeH / map.size();
    
    for (int mapY = 0; mapY < map.size(); mapY++) {
        for (int mapX = 0; mapX < map[mapY].size(); mapX++) {
            
           
            if (map[mapY][mapX] == '.')
            {
                distanceShader distShader;
                distShader.r = distShader.g = distShader.b = 0;
                distShader.a = 255;
                this->fillRect(mapX * mapBlockSizeW, mapY * mapBlockSizeH, mapBlockSizeW + 1, mapBlockSizeH + 1, distShader);
            }
			else
			{
				distanceShader distShader;
				distShader.r = distShader.g = distShader.b = distShader.a = 255;
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
	
	delete classMap;
}

vector<vector<Renderer::interceptions>>
Renderer::castRays(double x, double y, double angle, vector<string> map, const char scanChar)
{
	Map *classMap = new Map();
	float mapBlockSize = classMap->fMapBlockSize;
	RendererHelperFunctions *renderHelper = new RendererHelperFunctions();
	vector<Renderer::interceptions> interceptionsLowObjs;
	vector<Renderer::interceptions> interceptionsHighObjs;
	
	const float RAY_STEP_INC = 0.02f;
	const double FOV = 60.0f / (360.0f / PI);
	const int scanField = this->screenW / _resolution_factor;
	
	SDL_Texture* texture = nullptr;
	
	x += mapBlockSize / 2;
	y += mapBlockSize / 2;
	
	float xTo = x;
	float yTo = y;
	
	int mapX = 0;
	int mapY = 0;
	
	int screenColumn = 0;
	float angleDelta = -FOV / 2;
	
	for (int screenX = 0; screenX <= scanField; screenX += 1) {
		
		//init new loop values
		bool collision = false;
		bool isSmallObjHit = false;
		xTo = x;
		yTo = y;
		angleDelta += (FOV / scanField);
		
		float cosDelta = cosf(angle + angleDelta);
		float sinDelta = sinf(angle + angleDelta);
		
		//extend ray till collision
		while (!collision)
		{
			mapX = int(xTo / mapBlockSize);
			mapY = int(yTo / mapBlockSize);
			
			//default scanChar = '*'
			if (map[mapY][mapX] == scanChar || (map[mapY][mapX] != '.' && !isSmallObjHit))
			{
				int objType = 0;
				texture = _wallTexture;
				double totalDist = renderHelper->objDistFromCamera(x, xTo, y, yTo, angle);
				
				if (map[mapY][mapX] != '*' && !isSmallObjHit)
				{
					isSmallObjHit = true;
					objType = 1;
					mapRays.push_back(addMapDebugRay(x, y, xTo, yTo, screenColumn, angle));
					
					switch (map[mapY][mapX]) {
						case '#':
							texture = _boxTexture;
							break;
						case '@':
							texture = _boxTexture2;
							break;
						default:
							texture = _boxTexture;
							break;
					}
				}
				
				Renderer::interceptions intercepts;
				intercepts.x = x;
				intercepts.y = y;
				intercepts.xTo = xTo;
				intercepts.yTo = yTo;
				intercepts.distance = totalDist;
				intercepts.mapX = mapX;
				intercepts.mapY = mapY;
				intercepts.objType = objType;
				intercepts.texture = texture;
				intercepts.rayIndex = screenColumn;
				intercepts.rayAngle = angle + angleDelta;
				
				if (objType == 1)
					interceptionsLowObjs.push_back(intercepts);
				else
				{
					interceptionsHighObjs.push_back(intercepts);
					screenColumn++;
					collision = true;
				}
			}

			xTo += RAY_STEP_INC * cosDelta;
			yTo += RAY_STEP_INC * sinDelta;
		}
		
		if (!isSmallObjHit)
			mapRays.push_back(addMapDebugRay(x, y, xTo, yTo, screenColumn, angle));
	}
	
	delete renderHelper;
	delete classMap;
	
	vector<vector<Renderer::interceptions>> hiLoIntercepts;
	hiLoIntercepts.push_back(interceptionsHighObjs);
	hiLoIntercepts.push_back(interceptionsLowObjs);
	
	return hiLoIntercepts;
}

Renderer::linePoints Renderer::addMapDebugRay(float x, float y, float xTo, float yTo, int scrnClmn, float angle)
{
	double distX = fabs(x - xTo) * cosf(angle);
	double distY = fabs(y - yTo) * sinf(angle);
	double fPointsDist = fabs(distX + distY);
	
	linePoints lPoints;
	lPoints.x = x;
	lPoints.y = y;
	lPoints.xTo = xTo;
	lPoints.yTo = yTo;
	lPoints.rayIndex = scrnClmn-1;
	lPoints.distance = fPointsDist;
	
	return lPoints;
}

void Renderer::drawLine(double x, double y, double xTo, double yTo, int r, int g, int b)
{
    if (debug)
    	r = 255; g = b = 0;
    
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

    wallSliceStruct wallSlice;
    vector<wallSliceStruct> wallSlices;
    
    int last_mapX = -1;
    int last_mapY = -1;
    
    for (Renderer::interceptions intercept : interceptions)
    {
        if (intercept.distance == -1)
            continue;
        
        double wallFragH = _wallBlockSize / ((intercept.distance / (300.0f * _resolution_factor / 2)) * _resolution_factor);
        if (wallFragH < 10)
            wallFragH = 10;
        
        double wallFragY = (double)(this->screenH - wallFragH) / 2;
		double wallFragW = _resolution_factor;
        double wallFragX = (double)intercept.rayIndex * (double)wallFragW;
        
        if (intercept.objType == 0)
        {
            wallFragH *= 2;
            wallFragY -= wallFragH / 2;
        }
        
//        fillRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShaderGrayscale(intercept.distance));
//        drawRect(wallFragX, wallFragY, wallFragW, wallFragH, calcDistShaderGrayscale(intercept.distance));
        
        
        bool isNewBlock = (last_mapX != intercept.mapX || last_mapY != intercept.mapY);
        bool isFirstBlock = (last_mapX == -1 || last_mapY == -1);

        if (isNewBlock || isFirstBlock)
        {
            if (isNewBlock)
            	DrawWallTexture(wallSlices);
            
            wallSlices.clear();
        }

        double wallOffset = CalcTextureOffset(intercept.mapX, intercept.mapY, intercept.xTo, intercept.yTo);
        
        wallSlice.x = wallFragX;
        wallSlice.y = wallFragY;
        wallSlice.w = wallFragW;
        wallSlice.h = wallFragH;
        wallSlice.distance = intercept.distance;
        wallSlice.objType = intercept.objType;
        wallSlice.texOffset = wallOffset;
		wallSlice.texture = intercept.texture;
//		wallSlice.isWestLook = (intercept.xTo > (intercept.mapX + _wallBlockSize / 10)) ? true : false;
//		wallSlice.isNorthLook = (intercept.y < intercept.yTo) ? true : false;
		
        wallSlices.push_back(wallSlice);
        
        last_mapX = intercept.mapX;
        last_mapY = intercept.mapY;
    }
    
    DrawWallTexture(wallSlices);

}

double Renderer::CalcTextureOffset(int mapX, int mapY, double xTo, double yTo)
{
    int mapX_screen_to = (mapX + 1) * _wallBlockSize;
    int mapX_screen_from = mapX_screen_to - _wallBlockSize;
    
    int mapY_screen_to = (mapY + 1) * _wallBlockSize;
    int mapY_screen_from = mapY_screen_to - _wallBlockSize;
    
    double xto_screen = xTo * 10;
    double yto_screen = yTo * 10;

    double wallOffset = 64;
	
    int varFactor = 2; //6;

    if (int(xTo * 10) - varFactor <= mapX_screen_from)
        wallOffset = mapY_screen_to - (double)yto_screen;

    if (int(yTo * 10) - varFactor <= mapY_screen_from)
        wallOffset = (double)xto_screen - mapX_screen_from;

    if (int(xTo * 10) + varFactor >= mapX_screen_to)
        wallOffset = (double)yto_screen - mapY_screen_from;

    if (int(yTo * 10) + varFactor >= mapY_screen_to)
        wallOffset = mapX_screen_to - (double)xto_screen;
    
    return wallOffset;
}

void Renderer::DrawWallTexture(vector<wallSliceStruct> wallSlices)
{
    if (wallSlices.size() == 0)
        return;
    
    const int MAX_SHADE = 220;
	const int DARK_SHADE = 0;
	const int LIGHT_SHADE = 100;
	
    int texSliceW = _wallBlockSize / wallSlices.size() * _scale_factor;
    int texX;
    int texH = _wallBlockSize * _scale_factor;
	
    if (texSliceW < 1)
        texSliceW = 1;
    
    for (wallSliceStruct ws : wallSlices)
    {
        texX = (_wallBlockSize - ws.texOffset) * _scale_factor;
		if (ws.objType == 0)
			texH = _wallBlockSize * _scale_factor * 2;
		
        TextureMap(ws.texture,
                   texX, 0,
                   texSliceW, texH,
                   ws.w, ws.h,
                   ws.x, ws.y);
        
//        if (debug)
//            drawLine(ws.x, ws.y, ws.x, ws.y + ws.h / 4);
		
        //shade
        distanceShader ds;
		ds.r = ds.g = ds.b = 0;
        ds.a = (ws.distance / 1.5f > MAX_SHADE) ? MAX_SHADE : ws.distance / 1.5f ;
        fillRect(ws.x, ws.y, ws.w, ws.h, ds);
		
//		if (ws.isWestLook)
//		{
//			ds.r = ds.g = ds.b = DARK_SHADE;
//			ds.a = 100;
//			fillRect(ws.x, ws.y, ws.w, ws.h, ds);
//		}
    }
    
    if (debug)
    {
        drawLine(wallSlices[0].x,
                 wallSlices[0].y + wallSlices[0].h / 1.2,
                 wallSlices[0].x,
                 wallSlices[0].y + wallSlices[0].h);

        drawLine(wallSlices[wallSlices.size()-1].x + wallSlices[wallSlices.size()-1].w,
                 wallSlices[wallSlices.size()-1].y + wallSlices[wallSlices.size()-1].h / 2,
                 wallSlices[wallSlices.size()-1].x + wallSlices[wallSlices.size()-1].w,
                 wallSlices[wallSlices.size()-1].y + wallSlices[wallSlices.size()-1].h);
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
	
	SDL_DestroyTexture(Message);
	SDL_FreeSurface(surfaceMessage);
}
