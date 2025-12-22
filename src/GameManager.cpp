#include "GameManager.h"
#include <format>
#include <raylib.h>

GameManager::GameManager(const string& imgPath, const string& mzPath) 
    : imagePath(imgPath), mazePath(mzPath), enemy(nullptr), enemyCollisionCount(0), isRandomMode(false), enemyCollisionCooldown(0.5f), enemyCollisionTimer(0.0f) {}

// 重载构造函数
GameManager::GameManager(const string& imgPath, const string& mzPath, bool randomMaze, int rows, int cols) 
    : imagePath(imgPath), mazePath(mzPath), useRandomMaze(randomMaze), randomMazeRows(rows), randomMazeCols(cols), isRandomMode(randomMaze), enemy(nullptr), enemyCollisionCount(0), enemyCollisionCooldown(0.5f), enemyCollisionTimer(0.0f) {} 
// 初始化模式标记

GameManager::~GameManager() {
    if(enemy != nullptr) {
        enemy->Unload();
        delete enemy;
    }
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

    DrawText(TextFormat("Enemy Collision: %d", enemyCollisionCount), 10, 60, 20, ORANGE);

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
    if (useRandomMaze) {
        // 模式1：生成随机迷宫（行数、列数建议设为奇数）
        GenerateRandomMaze(maze, randomMazeRows, randomMazeCols, 50);
        TraceLog(LOG_INFO, "Generated random maze: %dx%d", randomMazeRows, randomMazeCols);
    } else {
        // 加载迷宫数据
        if (!LoadMazeFromFile(maze, mazePath)) {
        return false;
        }
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
    // 初始化敌人
    if(enemy == nullptr) {
        enemy = new Enemy(maze.tileSize, maze.endPos);
        if(!enemy->LoadTexture(imagePath)) {
            TraceLog(LOG_ERROR, "Failed to load enemy texture!");
        return false;
        }
    }else {
        enemy->Reset(maze.endPos, maze.tileSize);
    }
    enemyCollisionCount = 0;
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
        } else if (currentAlgo == PathAlgorithm::DIJKSTRA) {
            currentAlgo = PathAlgorithm::LAVA_DIJKSTRA;
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
        enemyCollisionTimer += GetFrameTime();
        // ===== 1. 检测 Ctrl+R：加载 maze0.txt 文件迷宫 =====
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R)) {
            // 步骤1：清理旧迷宫资源
            UnloadMazeTextures(maze);
            // 清空所有路径
            maze.dfsPath.clear();
            maze.bfsPath.clear();
            maze.dijkstraPath.clear();
            maze.lavaShortestPath.clear();
            // 步骤2：加载 maze0.txt 文件
            if (LoadMazeFromFile(maze, mazePath)) {
                TraceLog(LOG_INFO, "Loaded maze from file: %s", mazePath.c_str());
                // 更新窗口尺寸（适配新迷宫）
                screenWidth = maze.cols * maze.tileSize;
                screenHeight = maze.rows * maze.tileSize;
                SetWindowSize(screenWidth, screenHeight);
                // 重新加载迷宫纹理
                LoadMazeTextures(maze, imagePath);
                // 重置玩家位置和游戏状态
                player.Reset(maze);
                if(enemy) enemy->Reset(maze.endPos, maze.tileSize);
                isVictory = false;
                isGameOver = false;
                lavaCount = 0;
                isOnLava = false;
                // 切换为文件模式（禁用随机迷宫的 R 键刷新）
                enemyCollisionCount = 0;
                isRandomMode = false;
            } else {
                TraceLog(LOG_ERROR, "Failed to load maze0.txt!");
            }
        }
        // 在 Run 方法的 Ctrl+R 逻辑下方添加
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)) {
            GenerateRandomMaze(maze, randomMazeRows, randomMazeCols, 50);
            screenWidth = maze.cols * maze.tileSize;
            screenHeight = maze.rows * maze.tileSize;
            SetWindowSize(screenWidth, screenHeight);
            LoadMazeTextures(maze, imagePath);
            player.Reset(maze);
            if(enemy) enemy->Reset(maze.endPos, maze.tileSize);
            isVictory = false;
            isGameOver = false;
            lavaCount = 0;
            isOnLava = false;
            enemyCollisionCount = 0;
            isRandomMode = true; // 切回随机模式
        }
        // ===== 2. 原有 R 键：仅在随机模式下生成新随机迷宫 =====
        if (isRandomMode && IsKeyPressed(KEY_R)) {
            GenerateRandomMaze(maze, randomMazeRows, randomMazeCols, 50);
            player.Reset(maze);
            if(enemy) enemy->Reset(maze.endPos, maze.tileSize);
            isVictory = false;
            isGameOver = false;
            lavaCount = 0;
            isOnLava = false;
            enemyCollisionCount = 0;
        }
        // 空格键：场景切换 / 胜利/失败后重置游戏
        if (IsKeyPressed(KEY_SPACE)) {
            if (isVictory || isGameOver) {
                // 重置游戏状态
                isVictory = false;
                isGameOver = false;
                lavaCount = 0;
                enemyCollisionCount = 0;
                // 重置玩家位置（需确保 Player 类有 Reset 方法）
                player.Reset(maze);
                if(enemy) enemy->Reset(maze.endPos, maze.tileSize);
            } else {
                isMazeScene = !isMazeScene;
            }
        }

        // 迷宫场景且游戏未结束时，才允许移动和切换路径
        if (isMazeScene && !isVictory && !isGameOver) {
            SwitchPathAlgorithm();
            player.Move(maze);

            float playerSpeed = (float)maze.tileSize / 0.2f; // 假设玩家0.2秒移动一格
            enemy->Update(maze, playerSpeed);
            // 在 Run 方法的检测分支中修改
            auto [tileX, tileY] = player.GetCurrentTile();
            // 1. 检测熔岩：仅当 进入熔岩时 计数
            bool currOnLava = (maze.data[tileY][tileX] == 3);
            if (currOnLava && !isOnLava) {
                lavaCount++;
                TraceLog(LOG_INFO, TextFormat("踩中熔岩！计数：%d", lavaCount));
                if (lavaCount >=1 && enemyCollisionCount >=1) {
                    isGameOver = true;
                }
            }
            isOnLava = currOnLava; // 更新上一帧状态
            if (CheckPlayerEnemyCollision()&& enemyCollisionTimer >= enemyCollisionCooldown) {
                enemyCollisionCount++;
                enemyCollisionTimer = 0.0f;
                TraceLog(LOG_INFO, TextFormat("碰到敌人！计数：%d", enemyCollisionCount));
                if (enemyCollisionCount == 1) {
                    int enemyTileX = (int)(enemy->position.x -(maze.tileSize - enemy->frameWidth) /2) / maze.tileSize;
                    int enemyTileY = (int)(enemy->position.y -(maze.tileSize - enemy->frameHeight) /2) / maze.tileSize;
                    KnockbackPlayer(tileX, tileY, enemyTileX, enemyTileY);
                }
                if (enemyCollisionCount >= 2 || (lavaCount >= 1 && enemyCollisionCount >= 1)) {
                    isGameOver = true;
                }
            }
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
            enemy->Draw();
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

bool GameManager::CheckPlayerEnemyCollision() {
    // 玩家碰撞盒
    Vector2 playerPos = player.GetPosition();
    int pWidth = player.GetFrameWidth();
    int pHeight = player.GetFrameHeight();
    Rectangle playerRect = {
        playerPos.x, playerPos.y,
        (float)pWidth, (float)pHeight
    };
    // 敌人碰撞盒
    Rectangle enemyRect = {
        enemy->position.x, enemy->position.y,
        (float)enemy->frameWidth, (float)enemy->frameHeight
    };
    return CheckCollisionRecs(playerRect, enemyRect);
}

void GameManager::KnockbackPlayer(int playerTileX, int playerTileY, int enemyTileX, int enemyTileY) {
    int dx = playerTileX - enemyTileX;
    int dy = playerTileY - enemyTileY;

    // 归一化方向
    if (abs(dx) > abs(dy)) {
        dx = dx > 0 ? 1 : -1;
        dy = 0;
    } else {
        dy = dy > 0 ? 1 : -1;
        dx = 0;
    }

    float offsetX = (float)(maze.tileSize - player.GetFrameWidth()) / 2;
    float offsetY = (float)(maze.tileSize - player.GetFrameHeight()) / 2;

    // 尝试弹开2格
    int newTileX = playerTileX + dx * 2;
    int newTileY = playerTileY + dy * 2;
    if (newTileX >= 0 && newTileX < maze.cols && newTileY >= 0 && newTileY < maze.rows && maze.data[newTileY][newTileX] != 1) {
        Vector2 newPos = { (float)(newTileX * maze.tileSize) + offsetX, (float)(newTileY * maze.tileSize) + offsetY };
        player.SetPosition(newPos);
    }
    // 修正：检测1格弹开的条件（直接用playerTileX+dx，而非newTileX-dx）
    else if (playerTileX + dx >= 0 && playerTileX + dx < maze.cols && playerTileY + dy >= 0 && playerTileY + dy < maze.rows && maze.data[playerTileY + dy][playerTileX + dx] != 1) {
        newTileX = playerTileX + dx;
        newTileY = playerTileY + dy;
        Vector2 newPos = { (float)(newTileX * maze.tileSize) + offsetX, (float)(newTileY * maze.tileSize) + offsetY };
        player.SetPosition(newPos);
    }
    // 无法弹开，玩家可自行离开
}

void GameManager::Cleanup() {
    UnloadTexture(startImage);
    UnloadTexture(victoryImage);
    UnloadTexture(gameOverImage);
    UnloadMazeTextures(maze);
    player.Unload();
    CloseWindow();
}