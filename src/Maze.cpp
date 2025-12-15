#include "Maze.h"
#include <fstream>
#include <sstream>
#include <raylib.h>

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

void DrawMaze(const Maze& maze) {
    for (int i = 0; i < maze.rows; i++) {
        for (int j = 0; j < maze.cols; j++) {
            int x = j * maze.tileSize;
            int y = i * maze.tileSize;
            Rectangle src = { 0, 0, (float)maze.tileSize, (float)maze.tileSize };
            Rectangle dst = { (float)x, (float)y, (float)maze.tileSize, (float)maze.tileSize };

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
}

void UnloadMazeTextures(const Maze& maze) {
    UnloadTexture(maze.texStart);
    UnloadTexture(maze.texEnd);
    UnloadTexture(maze.texFloor);
    UnloadTexture(maze.texWall);
    UnloadTexture(maze.texGrass);
    UnloadTexture(maze.texLava);
}
