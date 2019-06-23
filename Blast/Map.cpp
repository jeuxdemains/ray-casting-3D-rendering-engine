//
//  Map.cpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#include "Map.hpp"

using namespace std;

Map::Map()
{
    fMapHudSizeW = map[0].size() * fMapBlockW;
    fMapHudSizeH = map.size() * fMapBlockH;
}

Map::~Map()
{
    
}

vector<string> Map::getCurrentMap()
{
    return this->map;
}

Map::MapCoords Map::posToCoordinate(float x, float y)
{
    Map::MapCoords nMapXYCoords;
    nMapXYCoords.x = floor(x / this->fMapBlockW);
    nMapXYCoords.y = floor(y / this->fMapBlockH);
    
    return nMapXYCoords;
}
