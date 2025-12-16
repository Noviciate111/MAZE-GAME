#pragma once
#include "Maze.h"
#include <raylib.h>
#include <string>

using namespace std;

// 移动方向枚举
enum class Direction {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Player {
private:
    Texture2D texPlayer;    // 玩家纹理（精灵图）
    Vector2 position;       // 玩家像素位置
    int tileSize;           // 地块大小，与迷宫一致
    int frameWidth;         // 单帧宽度
    int frameHeight;        // 单帧高度
    int currFrame;          // 当前动画帧
    float frameTimer;       // 帧切换计时器
    float frameTime;        // 单帧持续时间
    Direction dir;          // 当前移动方向
    // 新增移动冷却相关
    float moveCooldown;    // 单次移动的冷却时间（秒）
    float moveTimer;       // 移动冷却计时器
    // 根据方向设置精灵图行索引
    int GetSpriteRowByDir();
    // 检查移动是否合法（非墙）
    bool IsMoveValid(int newTileX, int newTileY, const Maze& maze);

public:
    Player();
    ~Player();

    // 初始化：加载纹理+设置初始位置（迷宫起点）
    bool Init(const Maze& maze, const string& imagePath);
    // 玩家移动逻辑
    void Move(const Maze& maze);
    // 绘制玩家（带动画）
    void Draw();
    // 卸载纹理
    void Unload();
};
