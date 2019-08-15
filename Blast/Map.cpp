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

vector<string> Map::currentMap()
{
    return this->map;
}

Map::MapCoords Map::posToCrd(float x, float y)
{
    Map::MapCoords nMapXYCoords;
    
    nMapXYCoords.x = int(x / this->fMapBlockW);
    nMapXYCoords.y = int(y / this->fMapBlockH);
    
    return nMapXYCoords;
}

Map::MapXYPos Map::crdToPos(int x, int y)
{
    Map::MapXYPos fMapXYPos;
    
    fMapXYPos.x = float(x * this->fMapBlockW);
    fMapXYPos.y = float(x * this->fMapBlockH);
    
    return fMapXYPos;
}
