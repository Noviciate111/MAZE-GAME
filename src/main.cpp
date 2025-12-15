#include <raylib.h>
#include <format>

int main(void) {
    // 初始化窗口（800x450分辨率，标题"Game"）
    InitWindow(800, 450, "Game");

    Image logo = LoadImage("../../resources/images/imastart.png");
    // 将图片上传到显存生成纹理
    Texture2D texture = LoadTextureFromImage(logo);
    // 释放CPU内存中的图片数据（显存已备份）
    UnloadImage(logo);

    // 计算图片屏幕正中央的位置
    Vector2 position;
    position.x = GetScreenWidth() / 2 - texture.width / 2;
    position.y = GetScreenHeight() / 2 - texture.height / 2;

    // 游戏主循环（窗口未关闭时持续运行）
    while (!WindowShouldClose()) {
        // 开始绘制帧
        BeginDrawing();
        // 清空背景为白色
        ClearBackground(RAYWHITE);

        // 绘制居中的图片
        DrawTexture(texture, position.x, position.y, WHITE);
        // 显示FPS帧率
        DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);

        // 结束当前帧绘制
        EndDrawing();
    }

    // 释放显存中的纹理资源
    UnloadTexture(texture);
    // 关闭窗口
    CloseWindow();

    return 0;
}