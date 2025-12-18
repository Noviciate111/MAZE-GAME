#pragma once
#include "Maze.h"   // 包含迷宫头文件，获取PathAlgorithm枚举
#include "Player.h"
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

public:
    GameManager(const string& imgPath = "../../resources/images/", 
                const string& mzPath = "../../resources/data/maze0.txt");
    ~GameManager();
    // 游戏初始化
    bool Init();
    // 游戏主循环
    void Run();
    // 清理资源
    void Cleanup();
};