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

bool GameManager::LoadVictoryImage() {
    Image img = LoadImage((imagePath + "game victory.png").c_str());
    if (img.data == nullptr) {
        TraceLog(LOG_ERROR, "Failed to load victory image!");
        return false;
    }
    victoryImage = LoadTextureFromImage(img);
    UnloadImage(img);
    return true;
}

void GameManager::DrawGameStatus() {
    // 绘制熔岩计数
    DrawText(TextFormat("Lava Count: %d", lavaCount), 10, 30, 20, RED);

    // 绘制胜利图片
    if (isVictory) {
        int imgX = (screenWidth - victoryImage.width) / 2;
        int imgY = (screenHeight - victoryImage.height) / 2;
        DrawTexture(victoryImage, imgX, imgY, WHITE);
    }
    // 绘制失败图片
    if (isGameOver) {
        int imgX = (screenWidth - gameOverImage.width) / 2;
        int imgY = (screenHeight - gameOverImage.height) / 2;
        DrawTexture(gameOverImage, imgX, imgY, WHITE);
    }
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
    if (!LoadVictoryImage()) {
        return false;
    } // 加载胜利图片
    if (!LoadGameOverImage()) {
        return false;
    } // 加载失败图片
    LoadMazeTextures(maze, imagePath);

    // 初始化玩家
    if (!player.Init(maze, imagePath)) {
        return false;
    }

    return true;
}

// Shift键循环切换路径算法（匹配PathAlgorithm枚举值）
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
        // 空格键：场景切换 / 胜利/失败后重置游戏
        if (IsKeyPressed(KEY_SPACE)) {
            if (isVictory || isGameOver) {
                // 重置游戏状态
                isVictory = false;
                isGameOver = false;
                lavaCount = 0;
                // 重置玩家位置（需确保 Player 类有 Reset 方法）
                player.Reset(maze);
            } else {
                isMazeScene = !isMazeScene;
            }
        }

        // 迷宫场景且游戏未结束时，才允许移动和切换路径
        if (isMazeScene && !isVictory && !isGameOver) {
            SwitchPathAlgorithm();
            player.Move(maze);

            // 在 Run 方法的检测分支中修改
            auto [tileX, tileY] = player.GetCurrentTile();
            // 1. 检测熔岩：仅当 进入熔岩时 计数
            bool currOnLava = (maze.data[tileY][tileX] == 3);
            if (currOnLava && !isOnLava) {
                lavaCount++;
                TraceLog(LOG_INFO, TextFormat("踩中熔岩！计数：%d", lavaCount));
                if (lavaCount >= 2) {
                    isGameOver = true;
                }
            }
            isOnLava = currOnLava; // 更新上一帧状态
            // 2. 检测终点（原有逻辑不变）
            if (tileX == maze.endPos.first && tileY == maze.endPos.second) {
                isVictory = true;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!isMazeScene) {
            DrawStartScene();
        } else {
            DrawMaze(maze, currentAlgo);
            player.Draw();
            // 调用绘制状态的方法（显示熔岩计数和胜利图片）
            DrawGameStatus();
        }

        DrawText(TextFormat("FPS: %d", GetFPS()), 5, 5, 20, BLACK);
        EndDrawing();
    }
}

bool GameManager::LoadGameOverImage() {
    Image img = LoadImage((imagePath + "game over.png").c_str());
    if (img.data == nullptr) {
        TraceLog(LOG_ERROR, "Failed to load game over image!");
        return false;
    }
    gameOverImage = LoadTextureFromImage(img);
    UnloadImage(img);
    return true;
}

void GameManager::Cleanup() {
    UnloadTexture(startImage);
    UnloadTexture(victoryImage);
    UnloadTexture(gameOverImage);
    UnloadMazeTextures(maze);
    player.Unload();
    CloseWindow();
}