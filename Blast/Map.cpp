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
    fMapHudSizeW = map[0].size() * fMapBlockSize;
    fMapHudSizeH = map.size() * fMapBlockSize;
}

Map::~Map()
{
    
}

vector<string> Map::currentMap()
{
    return this->map;
}

Map::MapCoords Map::posToCrd(float x, float y)
{
    Map::MapCoords nMapXYCoords;
    
    nMapXYCoords.x = int(x / this->fMapBlockSize);
    nMapXYCoords.y = int(y / this->fMapBlockSize);
    
    return nMapXYCoords;
}

Map::MapXYPos Map::crdToPos(int x, int y)
{
    Map::MapXYPos fMapXYPos;
    
    fMapXYPos.x = float(x * this->fMapBlockSize);
    fMapXYPos.y = float(x * this->fMapBlockSize);
    
    return fMapXYPos;
}
