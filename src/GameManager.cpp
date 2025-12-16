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
    if (!LoadMazeFromFile(maze, mazePath)) {
        return false;
    }

    screenWidth = maze.cols * maze.tileSize;
    screenHeight = maze.rows * maze.tileSize;

    InitWindow(screenWidth, screenHeight, "Maze Game");
    SetTargetFPS(60);

    if (!LoadStartImage()) {
        return false;
    }
    LoadMazeTextures(maze, imagePath);

    return true;
}

// Shift键循环切换：隐藏 → DFS → BFS → 隐藏
void GameManager::SwitchPathState() {
    if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
        if (pathState == PathState::HIDE) {
            pathState = PathState::SHOW_DFS;
        } else if (pathState == PathState::SHOW_DFS) {
            pathState = PathState::SHOW_BFS;
        } else {
            pathState = PathState::HIDE;
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
        if (IsKeyPressed(KEY_SPACE)) {
            isMazeScene = !isMazeScene;
        }

        if (isMazeScene) {
            SwitchPathState();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!isMazeScene) {
            DrawStartScene();
        } else {
            DrawMaze(maze, pathState);
        }

        DrawText(format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
        EndDrawing();
    }
}

void GameManager::Cleanup() {
    UnloadTexture(startImage);
    UnloadMazeTextures(maze);
    CloseWindow();
}