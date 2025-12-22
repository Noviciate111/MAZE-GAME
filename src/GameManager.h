#pragma once
#include "Maze.h"   // 包含迷宫头文件，获取PathAlgorithm枚举
#include "Player.h"
#include "Enemy.h"
#include <raylib.h>
#include <string>

using namespace std;

class GameManager {
private:
    Maze maze;
    Player player;
    Texture2D startImage;    
    Texture2D victoryImage;
    Texture2D gameOverImage;
    bool isMazeScene = false;
    PathAlgorithm currentAlgo = PathAlgorithm::NONE; // 修正：用PathAlgorithm枚举
    const string imagePath;  // 图片资源路径
    const string mazePath;   // 迷宫数据路径
    int screenWidth = 0;
    int screenHeight = 0;
    bool isVictory = false;
    bool isGameOver = false;
    int lavaCount = 0;
    bool useRandomMaze; // 新增：是否使用随机迷宫
    int randomMazeRows; // 随机迷宫行数（建议奇数）
    int randomMazeCols; // 随机迷宫列数（建议奇数）

    Enemy* enemy;                // 史莱姆敌人
    int enemyCollisionCount;     // 敌人碰撞计数（0/1/2）
     float enemyCollisionCooldown; // 碰撞冷却时间（秒）
    float enemyCollisionTimer;    // 碰撞冷却计时器
    
    bool LoadStartImage();
    void DrawGameStatus();
    // 绘制初始界面
    void DrawStartScene();
    // Shift键切换路径算法
    void SwitchPathAlgorithm();
    // 加载胜利图片
    bool LoadVictoryImage();
    bool LoadGameOverImage();
    bool isOnLava = false;
    bool isRandomMode = false;
    //检测玩家与敌人碰撞
    bool CheckPlayerEnemyCollision();
    //玩家被弹开逻辑
    void KnockbackPlayer(int tileX, int tileY, int enemyTileX, int enemyTileY);

public:
    GameManager(const string& imgPath = "../../resources/images/", 
                const string& mzPath = "../../resources/data/maze0.txt");
    // 重载构造函数：支持传入随机迷宫参数
    GameManager(const string& imgPath = "../../resources/images/", 
                const string& mzPath = "../../resources/data/maze0.txt",
                bool randomMaze = false, int rows = 15, int cols = 15);  
    // GameManager(const string& imgPath, const string& mzPath, bool randomMaze, int rows, int cols);
    ~GameManager();
    // 游戏初始化
    bool Init();
    // 游戏主循环
    void Run();
    // 清理资源
    void Cleanup();
};