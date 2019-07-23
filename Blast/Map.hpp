//
//  Map.hpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#ifndef Map_hpp
#define Map_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

#define MAP_HUD_W 100
#define MAP_HUD_H 100

class Map {
private:
    vector<string> map {
        {"******************************"},
        {"*............................*"},
        {"*............................*"},
        {"*............*...............*"},
        {"*............................*"},
        {"*............................*"},
        {"*............................*"},
        {"******************************"}
    };
    
public:
    struct MapCoords {
        int x;
        int y;
    };
    
    struct MapXYPos {
        float x;
        float y;
    };
    
    Map();
    ~Map();
    
    vector<string> currentMap();
    
    Map::MapCoords posToCrd(float x, float y);
    Map::MapXYPos crdToPos(int x, int y);
    
    float fMapBlockW = 64 / 10;
    float fMapBlockH = 64 / 10;
    float fMapHudSizeW = MAP_HUD_W;
    float fMapHudSizeH = MAP_HUD_H;
};

#endif /* Map_hpp */
