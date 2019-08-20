//
//  main.cpp
//  Blast
//
//  Created by Yuriy Georgiev on 19.06.19.
//  Copyright © 2019 Yuriy Georgiev. All rights reserved.
//

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720

#include <iostream>
#include "Game.hpp"
#include "Renderer.hpp"
#include "Map.hpp"

int main(int argc, const char * argv[]) {
    
    Game *game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    delete game;
    return 0;
}
