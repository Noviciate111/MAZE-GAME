#pragma once
#include "Maze.h"
#include <raylib.h>
#include <string>

using namespace std;

class GameManager {
private:
    Maze maze;
    Texture2D startImage;
    bool isMazeScene = false;
    PathState pathState = PathState::HIDE;
    const string imagePath;
    const string mazePath;
    int screenWidth = 0;
    int screenHeight = 0;

    bool LoadStartImage();
    void DrawStartScene();
    void SwitchPathState(); // Shift键循环切换路径状态

public:
    GameManager(const string& imgPath = "../../resources/images/", 
                const string& mzPath = "../../resources/data/maze0.txt");
    ~GameManager();
    bool Init();
    void Run();
    void Cleanup();
};