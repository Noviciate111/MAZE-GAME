#pragma once
#include "Maze.h"   // 包含迷宫头文件，获取PathAlgorithm枚举
#include "Player.h"
#include <raylib.h>
#include <string>

using namespace std;

class GameManager {
private:
    Maze maze;
    Player player;           // 玩家对象
    Texture2D startImage;    // 初始界面图片
    bool isMazeScene = false;
    PathAlgorithm currentAlgo = PathAlgorithm::NONE; // 修正：用PathAlgorithm枚举
    const string imagePath;  // 图片资源路径
    const string mazePath;   // 迷宫数据路径
    int screenWidth = 0;
    int screenHeight = 0;

    // 加载初始界面图片
    bool LoadStartImage();
    // 绘制初始界面
    void DrawStartScene();
    // Shift键切换路径算法
    void SwitchPathAlgorithm();

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