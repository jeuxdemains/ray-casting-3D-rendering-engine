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

class Map {
private:
    vector<string> map
	{
        {"******************************%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"},
        {"*...........................#*%............................%"},
		{"*...........................#*%...........$$$..............%"},
		{"*...........................#*%...........$%$..............%"},
		{"*...........................#*%...........$$$..............%"},
		{"*...........................@*%............................%"},
        {"*...........................#*%............................%"},
		{"*******************.....*******%%%%%%%%%%%%%%%%%%.....%%%%%%"},
		{"*******************.....*******%%%%%%%%%%%%%%%%%%.....%%%%%%"},
        {"*................@#.........#**...........................#*"},
        {"*.................#..........**...........................#*"},
		{"*................*#..........**......*****************..**#*"},
		{"*...........##@####..................*....................#*"},
		{"*....................................*....................#*"},
        {"*@...........................*********....................#*"},
		{"*@@@@........................#.............................*"},
		{"*@@..........................#............###..............*"},
		{"*............................#............#*#..............*"},
		{"*.....#...#...@...........................@############....*"},
        {"*..........................................................*"},
        {"*..........................................................*"},
		{"**....@...*...#................******************.....******"},
		{"**............................#******************.....******"},
        {"*...........................#**..........@................@*"},
		{"*.....#...#...#..............**..........@................@*"},
		{"*............................**....@.....@......@@@@@@....@*"},
		{"*..................................@.....@......@....@....@*"},
		{"*..................................@............@.........@*"},
		{"*@..........................#**....@............@.........@*"},
        {"************************************************************"}
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
    
    float fMapBlockSize = 64.0f / 1 / 10;
    float fMapHudSizeW = 100.0f;
    float fMapHudSizeH = 100.0f;
};

#endif /* Map_hpp */
