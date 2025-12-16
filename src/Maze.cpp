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
    // 查找起点终点并生成双路径
    FindStartEnd(maze);
    DFSFindPath(maze);
    BFSFindPath(maze);
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

// DFS 递归辅助函数
bool DFSHelper(Maze& maze, int x, int y, vector<vector<bool>>& visited, vector<pair<int, int>>& path) {
    if (x < 0 || x >= maze.cols || y < 0 || y >= maze.rows || maze.data[y][x] == 1 || visited[y][x]) {
        return false;
    }

    visited[y][x] = true;
    path.push_back({x, y});

    if (x == maze.endPos.first && y == maze.endPos.second) {
        return true;
    }

    // 上下左右遍历
    if (DFSHelper(maze, x-1, y, visited, path) ||
        DFSHelper(maze, x+1, y, visited, path) ||
        DFSHelper(maze, x, y-1, visited, path) ||
        DFSHelper(maze, x, y+1, visited, path)) {
        return true;
    }

    path.pop_back(); // 回溯
    return false;
}

void DFSFindPath(Maze& maze) {
    vector<vector<bool>> visited(maze.rows, vector<bool>(maze.cols, false));
    DFSHelper(maze, maze.startPos.first, maze.startPos.second, visited, maze.dfsPath);
}

// BFS 路径查找
void BFSFindPath(Maze& maze) {
    vector<vector<bool>> visited(maze.rows, vector<bool>(maze.cols, false));
    vector<vector<pair<int, int>>> parent(maze.rows, vector<pair<int, int>>(maze.cols, {-1, -1}));
    queue<pair<int, int>> q;

    q.push(maze.startPos);
    visited[maze.startPos.second][maze.startPos.first] = true;

    // 方向数组
    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();

        if (curr == maze.endPos) break;

        for (auto& dir : dirs) {
            int nx = curr.first + dir[0];
            int ny = curr.second + dir[1];
            if (nx >=0 && nx < maze.cols && ny >=0 && ny < maze.rows && maze.data[ny][nx] != 1 && !visited[ny][nx]) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    // 回溯生成BFS路径
    for (pair<int, int> p = maze.endPos; p != make_pair(-1, -1); p = parent[p.second][p.first]) {
        maze.bfsPath.push_back(p);
    }
    reverse(maze.bfsPath.begin(), maze.bfsPath.end());
}

// 绘制路径通用函数
void DrawPath(const Maze& maze, const vector<pair<int, int>>& path, Color color) {
    for (auto& p : path) {
        int x = p.first * maze.tileSize;
        int y = p.second * maze.tileSize;
        DrawRectangle(x + 5, y + 5, maze.tileSize - 10, maze.tileSize - 10, Fade(color, 0.6f));
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

    // 根据状态绘制对应路径
    if (pathState == PathState::SHOW_DFS && !maze.dfsPath.empty()) {
        DrawPath(maze, maze.dfsPath, RED);
    } else if (pathState == PathState::SHOW_BFS && !maze.bfsPath.empty()) {
        DrawPath(maze, maze.bfsPath, BLUE);
    }

    // 绘制路径说明
    DrawPathInfo(pathState);
}

void DrawPathInfo(PathState pathState) {
    int x = GetScreenWidth() - 180;
    int y = 10;
    int fontSize = 14;

    if (pathState == PathState::SHOW_DFS) {
        DrawText("Depth-First Search", x, y, fontSize, RED);
    } else if (pathState == PathState::SHOW_BFS) {
        DrawText("Breadth-First Search", x, y, fontSize, BLUE);
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