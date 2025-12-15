#include "Maze.h"
#include <fstream>
#include <sstream>
#include <raylib.h>
#include <algorithm>
#include <vector>

bool LoadMazeFromFile(Maze& maze, const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open maze file!");
        return false;
    }

    string line;
    // 读取首行行列数
    if (getline(file, line)) {
        istringstream iss(line);
        if (!(iss >> maze.rows >> maze.cols)) {
            TraceLog(LOG_ERROR, "Maze file first line format error!");
            return false;
        }
    } else {
        TraceLog(LOG_ERROR, "Maze file is empty!");
        return false;
    }

    // 读取迷宫数据
    maze.data.resize(maze.rows, vector<int>(maze.cols));
    for (int i = 0; i < maze.rows; i++) {
        if (!getline(file, line)) {
            TraceLog(LOG_WARNING, "Maze data is incomplete!");
            break;
        }
        istringstream iss(line);
        for (int j = 0; j < maze.cols; j++) {
            iss >> maze.data[i][j];
        }
    }

    file.close();
    // 查找起点终点并生成DFS路径
    FindStartEnd(maze);
    DFSFindPath(maze);
    return true;
}

void LoadMazeTextures(Maze& maze, const string& imagePath) {
    maze.texStart = LoadTexture((imagePath + "start.png").c_str());
    maze.texEnd = LoadTexture((imagePath + "end.png").c_str());
    maze.texFloor = LoadTexture((imagePath + "floor.png").c_str());
    maze.texWall = LoadTexture((imagePath + "wall.png").c_str());
    maze.texGrass = LoadTexture((imagePath + "grass.png").c_str());
    maze.texLava = LoadTexture((imagePath + "lava.png").c_str());
}

void FindStartEnd(Maze& maze) {
    for (int i = 0; i < maze.rows; i++) {
        for (int j = 0; j < maze.cols; j++) {
            if (maze.data[i][j] == -1) maze.startPos = {j, i};
            if (maze.data[i][j] == -2) maze.endPos = {j, i};
        }
    }
}

// DFS递归辅助函数
bool DFSHelper(Maze& maze, int x, int y, vector<vector<bool>>& visited, vector<pair<int, int>>& path) {
    // 越界、墙、已访问则返回false
    if (x < 0 || x >= maze.cols || y < 0 || y >= maze.rows || maze.data[y][x] == 1 || visited[y][x]) {
        return false;
    }

    visited[y][x] = true;
    path.push_back({x, y});

    // 到达终点
    if (x == maze.endPos.first && y == maze.endPos.second) {
        return true;
    }

    // 上下左右四个方向遍历（左→右→上→下）
    if (DFSHelper(maze, x - 1, y, visited, path) || 
        DFSHelper(maze, x + 1, y, visited, path) || 
        DFSHelper(maze, x, y - 1, visited, path) || 
        DFSHelper(maze, x, y + 1, visited, path)) {
        return true;
    }

    // 回溯：移除当前节点
    path.pop_back();
    return false;
}

// DFS路径查找主函数
void DFSFindPath(Maze& maze) {
    vector<vector<bool>> visited(maze.rows, vector<bool>(maze.cols, false));
    DFSHelper(maze, maze.startPos.first, maze.startPos.second, visited, maze.dfsPath);
}

// 绘制DFS路径（红色半透明色块）
void DrawDFSPath(const Maze& maze) {
    for (auto& p : maze.dfsPath) {
        int x = p.first * maze.tileSize;
        int y = p.second * maze.tileSize;
        // 路径色块内缩5像素，避免覆盖地块边缘
        DrawRectangle(x + 5, y + 5, maze.tileSize - 10, maze.tileSize - 10, Fade(RED, 0.6f));
    }
}

void DrawMaze(const Maze& maze, PathState pathState) {
    // 绘制迷宫基础纹理
    for (int i = 0; i < maze.rows; i++) {
        for (int j = 0; j < maze.cols; j++) {
            int x = j * maze.tileSize;
            int y = i * maze.tileSize;
            Rectangle src = {0, 0, (float)maze.tileSize, (float)maze.tileSize};
            Rectangle dst = {(float)x, (float)y, (float)maze.tileSize, (float)maze.tileSize};

            switch (maze.data[i][j]) {
                case -1: DrawTexturePro(maze.texStart, src, dst, {0,0}, 0, WHITE); break;
                case -2: DrawTexturePro(maze.texEnd, src, dst, {0,0}, 0, WHITE); break;
                case 0: DrawTexturePro(maze.texFloor, src, dst, {0,0}, 0, WHITE); break;
                case 1: DrawTexturePro(maze.texWall, src, dst, {0,0}, 0, WHITE); break;
                case 2: DrawTexturePro(maze.texGrass, src, dst, {0,0}, 0, WHITE); break;
                case 3: DrawTexturePro(maze.texLava, src, dst, {0,0}, 0, WHITE); break;
                default: DrawRectangle(x, y, maze.tileSize, maze.tileSize, GRAY); break;
            }
        }
    }

    // 显示DFS路径
    if (pathState == PathState::SHOW_DFS && !maze.dfsPath.empty()) {
        DrawDFSPath(maze);
    }

    // 绘制右上角路径说明
    DrawDFSPathInfo(pathState);
}

// 绘制DFS路径说明文字
void DrawDFSPathInfo(PathState pathState) {
    int x = GetScreenWidth() - 180;
    int y = 10;
    int fontSize = 14;

    if (pathState == PathState::SHOW_DFS) {
        DrawText("Depth-First Search", x, y, fontSize, RED);
    } else {
        DrawText("Hide Path", x, y, fontSize, GRAY);
    }
}

void UnloadMazeTextures(const Maze& maze) {
    UnloadTexture(maze.texStart);
    UnloadTexture(maze.texEnd);
    UnloadTexture(maze.texFloor);
    UnloadTexture(maze.texWall);
    UnloadTexture(maze.texGrass);
    UnloadTexture(maze.texLava);
}
