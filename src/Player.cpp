#include "Player.h"
#include <raylib.h>

Player::Player() : 
    currFrame(0), frameTimer(0.0f), frameTime(0.12f), dir(Direction::NONE),
    moveCooldown(0.0f), moveTimer(0.0f), isMoving(false), moveProgress(0.0f),
    baseMoveDuration(0.2f), currMoveDuration(0.2f) {}

Player::~Player() {
    Unload();
}

int Player::GetSpriteRowByDir() {
    // 有当前移动方向时用 dir，无方向时用最后一次有效朝向 lastDir
    Direction useDir = (dir != Direction::NONE) ? dir : lastDir;
    switch (useDir) {
        case Direction::UP: return 3;
        case Direction::DOWN: return 0;
        case Direction::LEFT: return 1;
        case Direction::RIGHT: return 2;
        default: return 0;
    }
}

bool Player::IsMoveValid(int newTileX, int newTileY, const Maze& maze) {
    // 边界检测 + 非墙检测
    if (newTileX < 0 || newTileX >= maze.cols || newTileY < 0 || newTileY >= maze.rows) {
        return false;
    }
    return maze.data[newTileY][newTileX] != 1;
}

// 新增：根据地形类型返回移动时长（草地3倍速慢，熔岩/地板正常）
float Player::GetMoveDurationByTile(const Maze& maze, int tileX, int tileY) {
    int tileType = maze.data[tileY][tileX];
    if (tileType == 2) { // 草地
        return baseMoveDuration * 3.0f;
    }
    return baseMoveDuration; // 其他地形正常速度
}

// 新增：获取当前格子坐标
pair<int, int> Player::GetCurrentTile() const {
    int tileX = (int)(position.x - (tileSize - frameWidth) / 2) / tileSize;
    int tileY = (int)(position.y - (tileSize - frameHeight) / 2) / tileSize;
    return {tileX, tileY};
}

bool Player::Init(const Maze& maze, const string& imagePath) {
    tileSize = maze.tileSize;
    // 加载玩家精灵图（4行3列）
    Image img = LoadImage((imagePath + "character.png").c_str());
    if (img.data == nullptr) {
        TraceLog(LOG_ERROR, "Failed to load player texture: character.png");
        return false;
    }
    texPlayer = LoadTextureFromImage(img);
    UnloadImage(img);

    // 计算单帧大小
    frameWidth = texPlayer.width / 3;
    frameHeight = texPlayer.height / 4;

    // 设置初始位置（居中对齐格子）
    float offsetX = (float)(tileSize - frameWidth) / 2;
    float offsetY = (float)(tileSize - frameHeight) / 2;
    position.x = (float)(maze.startPos.first * tileSize + offsetX);
    position.y = (float)(maze.startPos.second * tileSize + offsetY);
    moveStartPos = position;
    moveTargetPos = position;

    // 基于格子大小初始化基础移动时长（可选：按像素速度计算）
    baseMoveDuration = (float)tileSize / 150.0f; // 150px/s 速度
    currMoveDuration = baseMoveDuration;

    return true;
}

void Player::Move(const Maze& maze) {
    // 移动中时，执行插值计算
    if (isMoving) {
        // 累加移动进度
        moveProgress += GetFrameTime() / currMoveDuration;
        if (moveProgress < 0.0f) moveProgress = 0.0f;
        else if (moveProgress > 1.0f) moveProgress = 1.0f;

        // 线性插值计算当前位置
        position.x = moveStartPos.x + (moveTargetPos.x - moveStartPos.x) * moveProgress;
        position.y = moveStartPos.y + (moveTargetPos.y - moveStartPos.y) * moveProgress;

        // 移动动画帧切换
        frameTimer += GetFrameTime();
        if (frameTimer >= frameTime) {
            frameTimer = 0.0f;
            currFrame = (currFrame + 1) % 3;
        }

        // 移动完成
        if (moveProgress >= 1.0f) {
            isMoving = false;
            moveProgress = 0.0f;
            position = moveTargetPos; // 强制对齐终点
            currFrame = 1; // 静止帧
        }
        return; // 移动中不响应新输入
    }

    // 非移动状态，检测输入
    int tileX = GetCurrentTile().first;
    int tileY = GetCurrentTile().second;
    int newTileX = tileX;
    int newTileY = tileY;
    dir = Direction::NONE;

    if (IsKeyDown(KEY_UP)) {
        newTileY--;
        dir = Direction::UP;
    } else if (IsKeyDown(KEY_DOWN)) {
        newTileY++;
        dir = Direction::DOWN;
    } else if (IsKeyDown(KEY_LEFT)) {
        newTileX--;
        dir = Direction::LEFT;
    } else if (IsKeyDown(KEY_RIGHT)) {
        newTileX++;
        dir = Direction::RIGHT;
    }

    // 合法移动判断
    if (dir != Direction::NONE && IsMoveValid(newTileX, newTileY, maze)) {
        isMoving = true;
        moveStartPos = position;
        // 计算目标位置（格子居中对齐）
        float offsetX = (float)(tileSize - frameWidth) / 2;
        float offsetY = (float)(tileSize - frameHeight) / 2;
        moveTargetPos.x = (float)(newTileX * tileSize + offsetX);
        moveTargetPos.y = (float)(newTileY * tileSize + offsetY);
        // 获取地形对应的移动时长
        currMoveDuration = GetMoveDurationByTile(maze, newTileX, newTileY);
        // 重置动画状态
        frameTimer = 0.0f;
        currFrame = 0;
        lastDir = dir;
    } 
    // else if (dir == Direction::NONE) {
    //     // 静止时重置帧
    //     currFrame = 1;
    //     frameTimer = 0.0f;
    // }
}

void Player::Draw() {
    Rectangle frameRect = {
        (float)(currFrame * frameWidth),
        (float)(GetSpriteRowByDir() * frameHeight),
        (float)frameWidth,
        (float)frameHeight
    };
    DrawTextureRec(texPlayer, frameRect, position, WHITE);
}

void Player::Unload() {
    UnloadTexture(texPlayer);
}