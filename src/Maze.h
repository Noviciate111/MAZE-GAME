#pragma once
#include <raylib.h>
#include <vector>
#include <string>

using namespace std;

struct Maze {
    int rows = 0;
    int cols = 0;
    vector<vector<int>> data;
    int tileSize = 50;  // 单个地块像素大小

    // 纹理对象
    Texture2D texStart;
    Texture2D texEnd;
    Texture2D texFloor;
    Texture2D texWall;
    Texture2D texGrass;
    Texture2D texLava;
};

// 迷宫文件加载函数
bool LoadMazeFromFile(Maze& maze, const string& filePath);
// 迷宫纹理加载函数
void LoadMazeTextures(Maze& maze, const string& imagePath);
// 迷宫绘制函数
void DrawMaze(const Maze& maze);
// 迷宫纹理卸载函数
void UnloadMazeTextures(const Maze& maze);
