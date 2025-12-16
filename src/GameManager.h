#pragma once
#include "Maze.h"
#include <raylib.h>
#include <string>

using namespace std;

class GameManager {
private:
    Maze maze;
    Texture2D startImage;  // 初始界面图片
    bool isMazeScene = false;
    PathAlgorithm currentAlgo = PathAlgorithm::NONE; // 当前显示的路径算法
    const string imagePath;
    const string mazePath;
    int screenWidth = 0;
    int screenHeight = 0;

    // 加载初始界面图片
    bool LoadStartImage();
    // 绘制初始界面
    void DrawStartScene();
    // 切换路径算法（Shift键循环切换）
    void SwitchPathAlgorithm();

public:
    GameManager(const string& imgPath = "../../resources/images/", 
                const string& mzPath = "../../resources/data/maze0.txt");
    ~GameManager();

    // 游戏初始化
    bool Init();
    // 游戏主循环
    void Run();
    // 资源清理
    void Cleanup();
};