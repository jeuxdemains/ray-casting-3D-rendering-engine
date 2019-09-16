//
//  RendererHelperFunctions.cpp
//  Blast
//
//  Created by Yuriy Georgiev on 20.07.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#include "RendererHelper.hpp"


float RendererHelper::objDistFromCamera(double x, double xTo, double y, double yTo, double angle)
{
    double distX = fabs((x - xTo) * cosf(angle));
    double distY = fabs((y - yTo) * sinf(angle));
    double totalDist = fabs(distX + distY);
    
    return totalDist;
};

