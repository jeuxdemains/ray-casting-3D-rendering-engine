//
//  RendererHelperFunctions.hpp
//  Blast
//
//  Created by Yuriy Georgiev on 20.07.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#ifndef RendererHelperFunctions_hpp
#define RendererHelperFunctions_hpp

#include <stdio.h>
#include <cmath>

class RendererHelperFunctions
{
public:
    float objDistFromCamera(double x, double xTo, double y, double yTo, double angle);
    bool isRayHitsCorner(float mapPosX, float mapPosY, float mapPosX2, float mapPosY2, float rayXTo, float rayYTo);
};


#endif /* RendererHelperFunctions_hpp */
