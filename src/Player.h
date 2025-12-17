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
    Vector2 position;       // 玩家当前像素位置
    Vector2 moveStartPos;   // 移动起点
    Vector2 moveTargetPos;  // 移动终点
    int tileSize;           // 地块大小，与迷宫一致
    int frameWidth;         // 单帧宽度
    int frameHeight;        // 单帧高度
    int currFrame;          // 当前动画帧
    float frameTimer;       // 帧切换计时器
    float frameTime;        // 单帧持续时间
    Direction dir;          // 当前移动方向
    float moveCooldown;     // 单次移动的冷却时间（秒）
    float moveTimer;        // 移动冷却计时器
    // 新增平滑移动变量
    bool isMoving;          // 是否处于移动中
    float moveProgress;     // 移动进度 [0,1]
    float baseMoveDuration; // 基础单格移动时长
    float currMoveDuration;// 当前移动时长（可根据地形调整）

    Direction lastDir = Direction::DOWN;     // 上一次有效移动方向（用于静止时朝向）
    // 根据方向设置精灵图行索引
    int GetSpriteRowByDir();
    // 检查移动是否合法（非墙）
    bool IsMoveValid(int newTileX, int newTileY, const Maze& maze);
    // 新增：获取地形对应的移动时长
    float GetMoveDurationByTile(const Maze& maze, int tileX, int tileY);

public:
    Player();
    ~Player();

    // 初始化：加载纹理+设置初始位置（迷宫起点）
    bool Init(const Maze& maze, const string& imagePath);
    // 玩家移动逻辑（平滑插值版）
    void Move(const Maze& maze);
    // 绘制玩家（带动画）
    void Draw();
    // 卸载纹理
    void Unload();
    // 新增：获取当前格子坐标（对外接口，用于检测终点/熔岩）
    pair<int, int> GetCurrentTile() const;
};