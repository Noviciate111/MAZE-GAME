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
    // 加载迷宫数据（含DFS路径生成）
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

// Shift键切换DFS路径显示/隐藏
void GameManager::SwitchDFSPathState() {
    if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
        pathState = (pathState == PathState::HIDE) ? PathState::SHOW_DFS : PathState::HIDE;
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
        // 空格键切换初始图片/迷宫场景
        if (IsKeyPressed(KEY_SPACE)) {
            isMazeScene = !isMazeScene;
        }

        // 迷宫场景下才允许切换DFS路径
        if (isMazeScene) {
            SwitchDFSPathState();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!isMazeScene) {
            DrawStartScene();
        } else {
            DrawMaze(maze, pathState);
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
