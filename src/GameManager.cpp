#include "GameManager.h"
#include <format>
#include <raylib.h>

GameManager::GameManager(const string& imgPath, const string& mzPath) 
    : imagePath(imgPath), mazePath(mzPath) {}

GameManager::~GameManager() {
    Cleanup();
}

bool GameManager::LoadStartImage() {
    Image logo = LoadImage((imagePath + "imastart.png").c_str());
    if (logo.data == nullptr) {
        TraceLog(LOG_ERROR, "Failed to load start image!");
        return false;
    }
    startImage = LoadTextureFromImage(logo);
    UnloadImage(logo);
    return true;
}

bool GameManager::Init() {
    // 加载迷宫数据
    if (!LoadMazeFromFile(maze, mazePath)) {
        return false;
    }

    // 计算窗口大小
    screenWidth = maze.cols * maze.tileSize;
    screenHeight = maze.rows * maze.tileSize;

    // 初始化窗口
    InitWindow(screenWidth, screenHeight, "Maze Game");
    SetTargetFPS(60);

    // 加载初始图片和迷宫纹理
    if (!LoadStartImage()) {
        return false;
    }
    LoadMazeTextures(maze, imagePath);

    return true;
}

// Shift键循环切换路径算法
void GameManager::SwitchPathAlgorithm() {
    if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
        if (currentAlgo == PathAlgorithm::NONE) {
            currentAlgo = PathAlgorithm::DFS;
        } else if (currentAlgo == PathAlgorithm::DFS) {
            currentAlgo = PathAlgorithm::BFS;
        } else if (currentAlgo == PathAlgorithm::BFS) {
            currentAlgo = PathAlgorithm::DIJKSTRA;
        } else {
            currentAlgo = PathAlgorithm::NONE;
        }
    }
}

void GameManager::DrawStartScene() {
    Vector2 logoPos = { 
        (float)(screenWidth / 2 - startImage.width / 2), 
        (float)(screenHeight / 2 - startImage.height / 2) 
    };
    DrawTexture(startImage, (int)logoPos.x, (int)logoPos.y, WHITE);
}

void GameManager::Run() {
    while (!WindowShouldClose()) {
        // 空格键切换场景
        if (IsKeyPressed(KEY_SPACE)) {
            isMazeScene = !isMazeScene;
        }

        // 迷宫场景下才允许切换路径算法
        if (isMazeScene) {
            SwitchPathAlgorithm();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!isMazeScene) {
            DrawStartScene();
        } else {
            DrawMaze(maze, currentAlgo);
        }

        // 显示FPS
        DrawText(format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
        EndDrawing();
    }
}

void GameManager::Cleanup() {
    UnloadTexture(startImage);
    UnloadMazeTextures(maze);
    CloseWindow();
}