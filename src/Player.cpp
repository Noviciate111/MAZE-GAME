#include "Player.h"
#include <raylib.h>

Player::Player() : currFrame(0), frameTimer(0.0f), frameTime(0.3f), dir(Direction::NONE),moveCooldown(0.2f), moveTimer(0.0f) {}
// moveCooldown=0.2秒，可按需调整

Player::~Player() {
    Unload();
}

int Player::GetSpriteRowByDir() {
    switch (dir) {
        case Direction::UP: return 3;
        case Direction::DOWN: return 0;
        case Direction::LEFT: return 1;
        case Direction::RIGHT: return 2;
        default: return 0; // 默认向下
    }
}

bool Player::IsMoveValid(int newTileX, int newTileY, const Maze& maze) {
    // 边界检测 + 非墙检测
    if (newTileX < 0 || newTileX >= maze.cols || newTileY < 0 || newTileY >= maze.rows) {
        return false;
    }
    return maze.data[newTileY][newTileX] != 1;
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

    // 设置初始位置（迷宫起点）
    position.x = (float)(maze.startPos.first * tileSize + (tileSize - frameWidth) / 2);
    position.y = (float)(maze.startPos.second * tileSize + (tileSize - frameHeight) / 2);

    return true;
}

void Player::Move(const Maze& maze) {
    // 累计冷却计时器
    moveTimer += GetFrameTime();

    int tileX = (int)position.x / tileSize;
    int tileY = (int)position.y / tileSize;
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

    // 只有冷却时间结束，且移动合法时，才执行移动
    if (dir != Direction::NONE && IsMoveValid(newTileX, newTileY, maze) && moveTimer >= moveCooldown) {
        position.x = (float)(newTileX * tileSize + (tileSize - frameWidth) / 2);
        position.y = (float)(newTileY * tileSize + (tileSize - frameHeight) / 2);

        // 重置冷却计时器
        moveTimer = 0.0f;

        // 动画帧切换
        frameTimer += GetFrameTime();
        if (frameTimer >= frameTime) {
            frameTimer = 0.0f;
            currFrame = (currFrame + 1) % 3;
        }
    } else if (dir == Direction::NONE) {
        // 静止时重置帧和计时器
        currFrame = 0;
        frameTimer = 0.0f;
    }
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
