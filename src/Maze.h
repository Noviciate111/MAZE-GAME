#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include <stack>
#include <utility>

using namespace std;

// 路径显示状态
enum class PathState {
    HIDE,  // 隐藏路径
    SHOW_DFS // 显示DFS路径
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

    // DFS路径相关
    pair<int, int> startPos;  // 起点坐标 (x,y) -> (列,行)
    pair<int, int> endPos;    // 终点坐标 (x,y)
    vector<pair<int, int>> dfsPath; // DFS路径节点
};

// 迷宫文件加载
bool LoadMazeFromFile(Maze& maze, const string& filePath);
// 迷宫纹理加载
void LoadMazeTextures(Maze& maze, const string& imagePath);
// 迷宫绘制（含DFS路径）
void DrawMaze(const Maze& maze, PathState pathState);
// 迷宫纹理卸载
void UnloadMazeTextures(const Maze& maze);
// 查找迷宫起点和终点
void FindStartEnd(Maze& maze);
// DFS路径查找实现
void DFSFindPath(Maze& maze);
// 绘制路径说明文字
void DrawDFSPathInfo(PathState pathState);
