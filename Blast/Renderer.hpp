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
#include "RendererHelper.hpp"

using namespace std;

class Renderer {
private:
	const float _resolution_factor = 1.0f; //12.0f;
	const int _scale_factor = 1;
	const float _wallBlockSize = 64.0f / _scale_factor;
	
    SDL_Renderer* _renderer;
    SDL_Texture *_ceilTexture = nullptr, *_floorTexture = nullptr;
	vector<SDL_Texture*> _wallTextures, _boxTextures;
    TTF_Font* _font = nullptr;
    
    struct distShader {
        int r, g, b, a;
    };
    
public:
    bool debug = false;
    int screenW, screenH;
    
    struct intercepts {
        double x, y, xTo, yTo, distance, rayAngle;
        int mapX, mapY, objType, rayIndex;
		SDL_Texture* texture = nullptr;
    };
    
    struct wallSliceStruct
    {
        int x,y,w,h;
		bool isWestLook, isNorthLook;
        double texOffset, distance;
        int objType;
		SDL_Texture* texture = nullptr;
    };
    
    struct linePoints {
        double x, y, xTo, yTo, distance;
        int rayIndex;
    };
    
    vector<linePoints> mapRays;
    
    //functions
    Renderer(int winW, int winH, SDL_Renderer *renderer);
    ~Renderer();
    
    void renderFrame();
    void prepareRender();
	void loadAssets();
	
    void drawMap(vector<string> map, vector<linePoints> mapRays,
				 double fPlayerX, double fPlayerY,
				 double fPlayerA, double fMovDir);
	
    vector<vector<intercepts>>
	castRays(double x, double y,
			 double angle, vector<string> map,
			 const char scanChar = '*');
	
	linePoints addMapDebugRay(float x, float y,
							  float xTo, float yTo,
							  int scrnClmn, float angle);
	
    void RenderScene(vector<intercepts> interceptions);
	
	SDL_Texture* loadTexture(const char* texturePath);
	
    void TextureMap(SDL_Texture *texture,
					double srcX, double srcY,
					double srcW, double srcH,
					double destW, double destH,
					double destX, double destY);
	
    void drawCeil();
    void drawFloor();
    void DrawWallTexture(vector<wallSliceStruct> wallSlices);
	
    double CalcTextureOffset(int mapX, int mapY,
							 double xTo, double yTo);
	
	double DistanceFactor(double fDistance);
    
    //primitives
    void drawLine(double x, double y,
				  double xTo, double yTo,
				  int r = 255, int g = 255, int b = 255);
	
    void fillRect(double x, double y,
				  double w, double h,
				  distShader shaderColor);
	
    void drawRect(double x, double y,
				  double w, double h,
				  distShader shaderColor);
	
    void drawText(const char* text,
				  double x, double y,
				  double w, double h,
				  Uint8 r, Uint8 g, Uint8 b);
    
    //coloring
    distShader calcDistShaderGrayscale(double distance, int color = 255);
    distShader calcDistShaderColor(double distance, int r, int g, int b, int a);
};


#endif /* Renderer_hpp */
