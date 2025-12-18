#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <map>
#include <climits>

using namespace std;

// 路径算法类型枚举
enum class PathAlgorithm {
    NONE,
    DFS,
    BFS,
    DIJKSTRA,
    LAVA_DIJKSTRA
};

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

    // 路径相关
    pair<int, int> startPos;  // 起点坐标 (x,y)
    pair<int, int> endPos;    // 终点坐标 (x,y)
    vector<pair<int, int>> dfsPath;   // DFS路径
    vector<pair<int, int>> bfsPath;   // BFS路径
    vector<pair<int, int>> dijkstraPath; // Dijkstra路径
    vector<pair<int, int>> lavaShortestPath; // 熔岩最短路径
};

// 迷宫文件加载函数
bool LoadMazeFromFile(Maze& maze, const string& filePath);
// 迷宫纹理加载函数
void LoadMazeTextures(Maze& maze, const string& imagePath);
// 迷宫绘制函数
void DrawMaze(const Maze& maze, PathAlgorithm currentAlgo);
// 迷宫纹理卸载函数
void UnloadMazeTextures(const Maze& maze);
// 查找起点和终点
void FindStartEnd(Maze& maze);
// DFS找路径
void DFSFindPath(Maze& maze);
// BFS找路径
void BFSFindPath(Maze& maze);
// Dijkstra找最短路径
void DijkstraFindPath(Maze& maze);
// 熔岩最短路径查找
void LavaDijkstraFindPath(Maze& maze);
// 绘制路径说明文字
void DrawPathInfo(PathAlgorithm currentAlgo);
//
void GenerateRandomMaze(Maze& maze, int rows, int cols, int tileSize);
