#include "GameManager.h"
#include <raylib.h>

int main() {
    // 增加资源路径合法性检测
    const string imgPath = "../../resources/images/";
    const string mazePath = "../../resources/data/maze0.txt";
    // 初始化raylib上下文（提前初始化，避免GameManager中初始化异常）
    SetTraceLogLevel(LOG_INFO); // 设置日志级别，便于调试

    GameManager game(imgPath, mazePath, true, 15, 15);
    if (!game.Init()) {
        TraceLog(LOG_ERROR, "Game initialization failed!");
        // 确保窗口未初始化时不调用CloseWindow
        if (IsWindowReady()) {
            CloseWindow();
        }
        return -1;
    }
    game.Run();
    game.Cleanup();

    return 0;
}