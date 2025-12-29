#include "Enemy.h"
#include <raylib.h>
#include <random>
#include <ctime>
#include <cmath>

using namespace std;

// 静态随机数生成器
static mt19937 rng((unsigned int)time(nullptr));

Enemy::Enemy(int tileSize, pair<int, int> endPos) {
    this->tileSize = tileSize;
    this->homePos = endPos;
    this->moveInterval = 1.0f;    // 敌人移动决策间隔
    this->moveTimer = 0.0f;
    this->moveSpeed = 0.0f;

    // 动画初始化
    this->frameWidth = 0;
    this->frameHeight = 0;
    this->currFrame = 1;          // 初始静止帧
    this->frameTimer = 0.0f;
    this->frameTime = 0.12f;      // 单帧持续时间
    this->dir = Direction::NONE;
    this->lastDir = Direction::DOWN;
    this->isMoving = false;
    this->moveProgress = 0.0f;
    this->baseMoveDuration = 0.3f; // 敌人基础移动时长（比玩家稍慢）

    // 随机生成初始位置
    uniform_int_distribution<int> rangeDist(-spawnRange, spawnRange);
    int tileX = endPos.first;
    int tileY = endPos.second;

    while (true) {
        int dx = rangeDist(rng);
        int dy = rangeDist(rng);
        int newX = tileX + dx;
        int newY = tileY + dy;
        if (abs(dx) <= spawnRange && abs(dy) <= spawnRange && newX >= 0 && newY >= 0) {
            this->position = TileToPixel(newX, newY);
            this->moveStartPos = position;
            this->moveTargetPos = position;
            break;
        }
    }
}

bool Enemy::LoadTexture(const string& imagePath) {
    // 加载史莱姆精灵图（假设为4行3列，与玩家格式一致）
    Image img = LoadImage((imagePath + "slime.png").c_str());
    if (img.data == nullptr) {
        TraceLog(LOG_ERROR, "Failed to load slime texture!");
        return false;
    }
    tex = LoadTextureFromImage(img);
    UnloadImage(img);

    // 计算单帧大小（精灵图4行3列）
    frameWidth = tex.width / 3;
    frameHeight = tex.height / 4;
    return tex.id != 0;
}

// 随机选择移动方向
Direction Enemy::GetRandomDir() {
    uniform_int_distribution<int> dirDist(0, 3);
    switch (dirDist(rng)) {
        case 0: return Direction::UP;
        case 1: return Direction::DOWN;
        case 2: return Direction::LEFT;
        case 3: return Direction::RIGHT;
        default: return Direction::DOWN;
    }
}

// 根据方向获取精灵图行索引（与玩家逻辑一致）
int Enemy::GetSpriteRowByDir() {
    Direction useDir = (dir != Direction::NONE) ? dir : lastDir;
    switch (useDir) {
        case Direction::UP: return 3;
        case Direction::DOWN: return 0;
        case Direction::LEFT: return 1;
        case Direction::RIGHT: return 2;
        default: return 0;
    }
}

void Enemy::Update(const Maze& maze, float playerSpeed) {
    // 敌人速度 = 玩家速度 * 2/3
    this->moveSpeed = playerSpeed * 2.0f / 3.0f;

    // 移动中时执行平滑插值
    if (isMoving) {
        moveProgress += GetFrameTime() / baseMoveDuration;
        moveProgress = clamp(moveProgress, 0.0f, 1.0f);

        // 线性插值计算当前位置
        position.x = moveStartPos.x + (moveTargetPos.x - moveStartPos.x) * moveProgress;
        position.y = moveStartPos.y + (moveTargetPos.y - moveStartPos.y) * moveProgress;

        // 动画帧切换
        frameTimer += GetFrameTime();
        if (frameTimer >= frameTime) {
            frameTimer = 0.0f;
            currFrame = (currFrame + 1) % 3;
        }

        // 移动完成
        if (moveProgress >= 1.0f) {
            isMoving = false;
            moveProgress = 0.0f;
            position = moveTargetPos;
            currFrame = 1; // 静止帧
            dir = Direction::NONE;
        }
        return;
    }

    // 非移动状态：间隔时间到后随机决策移动
    moveTimer += GetFrameTime();
    if (moveTimer < moveInterval) return;
    moveTimer = 0.0f;

    // 获取当前格子坐标
    int currTileX = (int)(position.x - (tileSize - frameWidth) / 2) / tileSize;
    int currTileY = (int)(position.y - (tileSize - frameHeight) / 2) / tileSize;
    int newTileX = currTileX;
    int newTileY = currTileY;

    // 随机选择移动方向
    dir = GetRandomDir();
    switch (dir) {
        case Direction::UP: newTileY--; break;
        case Direction::DOWN: newTileY++; break;
        case Direction::LEFT: newTileX--; break;
        case Direction::RIGHT: newTileX++; break;
        default: return;
    }

    // 检测移动合法性（仅墙和活动范围限制，无视岩浆/草丛）
    if (IsMoveValid(newTileX, newTileY, maze)) {
        isMoving = true;
        moveStartPos = position;
        moveTargetPos = TileToPixel(newTileX, newTileY);
        // 重置动画状态
        frameTimer = 0.0f;
        currFrame = 0;
        lastDir = dir;
    } else {
        dir = Direction::NONE; // 方向无效，重置
    }
}

void Enemy::Draw() {
    // 绘制精灵图当前帧（与玩家绘制逻辑一致）
    Rectangle frameRect = {
        (float)(currFrame * frameWidth),
        (float)(GetSpriteRowByDir() * frameHeight),
        (float)frameWidth,
        (float)frameHeight
    };
    DrawTextureRec(tex, frameRect, position, WHITE);
}

void Enemy::Reset(pair<int, int> newEndPos, int newTileSize,  const Maze& maze) {
    this->homePos = newEndPos;
    this->tileSize = newTileSize;

    // 重新随机生成位置
    uniform_int_distribution<int> rangeDist(-spawnRange, spawnRange);
    int tileX = newEndPos.first;
    int tileY = newEndPos.second;

    while (true) {
        int dx = rangeDist(rng);
        int dy = rangeDist(rng);
        int newX = tileX + dx;
        int newY = tileY + dy;
        if (abs(dx) <= spawnRange && abs(dy) <= spawnRange && newX >= 0 && newY >= 0) {
            this->position = TileToPixel(newX, newY);
            this->moveStartPos = position;
            this->moveTargetPos = position;
            break;
        }
    }

    // 重置动画状态
    dir = Direction::NONE;
    lastDir = Direction::DOWN;
    isMoving = false;
    moveProgress = 0.0f;
    currFrame = 1;
}

void Enemy::Unload() {
    UnloadTexture(tex);
}

bool Enemy::IsMoveValid(int x, int y, const Maze& maze) {
    // 1. 边界检测
    if (x < 0 || x >= maze.cols || y < 0 || y >= maze.rows) return false;
    // 2. 墙体检测（仅墙阻挡，岩浆/草丛无影响）
    if (maze.data[y][x] == 1) return false;
    // 3. 活动范围检测（终点7格内）
    if (abs(x - homePos.first) > maxMoveRange || abs(y - homePos.second) > maxMoveRange) return false;
    return true;
}

Vector2 Enemy::TileToPixel(int tileX, int tileY) {
    // 与玩家一致的居中对齐逻辑
    float offsetX = (float)(tileSize - frameWidth) / 2;
    float offsetY = (float)(tileSize - frameHeight) / 2;
    return {
        (float)(tileX * tileSize) + offsetX,
        (float)(tileY * tileSize) + offsetY
    };
}