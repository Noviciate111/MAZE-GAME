#pragma once
#include "Maze.h"
#include <raylib.h>
#include <string>
#include <utility>
#include "Player.h"

using namespace std;

class Enemy {
public:
    // 敌人状态
    Vector2 position;          // 像素坐标
    Texture2D tex;             // 史莱姆纹理（精灵图）
    float moveSpeed;           // 移动速度（像素/秒）
    float moveTimer;           // 移动间隔计时器
    float moveInterval;        // 每次移动间隔（秒）
    pair<int, int> homePos;    // 终点坐标（敌人活动中心）
    int maxRange;              // 最大活动范围（格子数）
    int tileSize;              // 迷宫格子大小
    int spawnRange = 6;        // 生成时随机范围（格子数）
    int maxMoveRange = 7;      // 最大移动范围（格子数）

    // 动画相关成员（与玩家一致）
    int frameWidth;            // 单帧宽度
    int frameHeight;           // 单帧高度
    int currFrame;             // 当前动画帧
    float frameTimer;          // 帧切换计时器
    float frameTime;           // 单帧持续时间
    Direction dir;             // 当前移动方向
    Direction lastDir;         // 上一次移动方向（静止时朝向）
    bool isMoving;             // 是否处于移动中
    float moveProgress;        // 移动进度 [0,1]
    float baseMoveDuration;    // 基础单格移动时长
    Vector2 moveStartPos;      // 移动起点
    Vector2 moveTargetPos;     // 移动终点

    // 构造函数
    Enemy(int tileSize, pair<int, int> endPos);
    // 加载纹理（精灵图）
    bool LoadTexture(const string& imagePath);
    // 更新敌人位置+动画（随机移动，限制在终点附近）
    void Update(const Maze& maze, float playerSpeed);
    // 绘制敌人（带动画）
    void Draw();
    // 重置敌人位置（新迷宫生成时调用）
    void Reset(pair<int, int> newEndPos, int newTileSize);
    // 卸载纹理
    void Unload();

private:
    // 检测移动是否合法（非墙+在活动范围内，无视岩浆/草丛）
    bool IsMoveValid(int x, int y, const Maze& maze);
    // 将格子坐标转换为像素坐标（居中对齐）
    Vector2 TileToPixel(int tileX, int tileY);
    // 根据方向获取精灵图行索引
    int GetSpriteRowByDir();
    // 随机选择移动方向
    Direction GetRandomDir();
};