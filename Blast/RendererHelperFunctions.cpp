//
//  RendererHelperFunctions.cpp
//  Blast
//
//  Created by Yuriy Georgiev on 20.07.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#include "RendererHelperFunctions.hpp"


float RendererHelperFunctions::objDistFromCamera(double x, double xTo, double y, double yTo, double angle)
{
    double distX = fabs((x - xTo) * cosf(angle));
    double distY = fabs((y - yTo) * sinf(angle));
    double totalDist = fabs(distX + distY);
    
    return totalDist;
};

bool RendererHelperFunctions::isRayHitsCorner(float mapPosX, float mapPosY, float mapPosX2, float mapPosY2, float rayXTo, float rayYTo)
{
    if ((mapPosX == round(rayXTo) || mapPosX2 == round(rayXTo)) &&
        (mapPosY == round(rayYTo) || mapPosY2 == round(rayYTo)))
    {
        return true;
    }
    
    return false;
};


