#include <raylib.h>
#include <format>

int main(void) {
    // 初始化窗口：宽800、高450、标题"Game"
    InitWindow(800, 450, "Game");

    // 加载图片到内存
    Image logo = LoadImage("../../resources/images/imastart.png");
    // 将内存中的图片传输到显存（创建纹理）
    Texture2D texture = LoadTextureFromImage(logo);
    // 显存已有纹理，释放内存中的图片资源
    UnloadImage(logo);

    // 窗口未关闭时持续循环
    while (!WindowShouldClose()) {
        BeginDrawing();
        // 清空背景为白色
        ClearBackground(RAYWHITE);

        // 在屏幕正中间居中对齐
        DrawTexture(texture, (GetScreenWidth()/2) - (texture.width/2), (GetScreenHeight()/2) - (texture.height/2), WHITE);

        // 绘制FPS信息
        DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
        EndDrawing();
    }

    // 释放显存中的纹理资源
    UnloadTexture(texture);
    // 关闭窗口
    CloseWindow();
    return 0;
}


//创建窗口
// #include <raylib.h>
// #include <format>

// using namespace std;
// int main(void) {
//     // 初始化窗口（宽、高、标题）
//     InitWindow(800, 450, "Game");

//     // 游戏主循环（窗口未关闭时持续运行）
//     while (!WindowShouldClose()) {
//         BeginDrawing(); // 开始绘制

//         ClearBackground(RAYWHITE); // 清空背景为白色

//         // 绘制提示文字（内容、X坐标、Y坐标、字号、颜色）
//         DrawText("Maze Game", 320, 180, 26, BLACK);
//         DrawText("Press the Spacebar to start.", 270, 220, 18, GRAY);
        
//         // 绘制FPS信息
//         DrawText(format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);

//         EndDrawing(); // 结束绘制
//     }

//     CloseWindow(); // 关闭窗口，释放资源
//     return 0;
// }

//绘制图形
// #include <raylib.h>
// #include <format >
// int main(void){
//     InitWindow(800, 450, "Game");

//     //游戏主循环
//     while (!WindowShouldClose()){ //检测是否关闭了窗口或按下ESC键
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//     //在窗口画两个圆
//         DrawCircle(350, 300, 100, PINK);
//         DrawCircle(450, 300, 100, GREEN);

//         DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
//         EndDrawing();
//     }
//     CloseWindow(); //关闭窗口回收raylib资源
//     return 0;
// }

// //逻辑区与绘图区
// #include <raylib.h>
// #include <format>

// int main(void) {
//     // 初始化窗口：宽800、高450、标题"Game"
//     InitWindow(800, 450, "Game");
//     // 圆形的y坐标（初始值50）
//     float circle_y = 50.0F;

//     // 窗口未关闭时持续循环
//     while (!WindowShouldClose()) {
//         // 逻辑区
//         if (circle_y < 400) {
//             // 按100单位/秒的速度向下移动圆形
//             circle_y += GetFrameTime() * 100;
//         }

//         // 绘图区
//         BeginDrawing();
//         // 清空背景为白色
//         ClearBackground(RAYWHITE);
//         // 绘制粉色圆形：中心(400, circle_y)、半径50
//         DrawCircle(400, circle_y, 50, PINK);
//         // 绘制FPS信息
//         DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
//         EndDrawing();
//     }

//     // 关闭窗口
//     CloseWindow();
//     return 0;
// }

//读取并显示图片
// #include <raylib.h>
// #include <format>

// int main(void) {
//     // 初始化窗口：宽800、高450、标题"Game"
//     InitWindow(800, 450, "Game");

//     // 加载图片到内存
//     Image logo = LoadImage("../../resources/images/start.png");
//     // 将内存中的图片传输到显存（创建纹理）
//     Texture2D texture = LoadTextureFromImage(logo);
//     // 显存已有纹理，释放内存中的图片资源
//     UnloadImage(logo);

//     // 窗口未关闭时持续循环
//     while (!WindowShouldClose()) {
//         BeginDrawing();
//         // 清空背景为白色
//         ClearBackground(RAYWHITE);

//         // 在屏幕中间区域绘制两个logo（居中对齐）
//         DrawTexture(texture, 300 - texture.width/2, 225 - texture.height/2, WHITE);
//         DrawTexture(texture, 500 - texture.width/2, 225 - texture.height/2, WHITE);

//         // 绘制FPS信息
//         DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
//         EndDrawing();
//     }

//     // 释放显存中的纹理资源
//     UnloadTexture(texture);
//     // 关闭窗口
//     CloseWindow();
//     return 0;
// }

//键盘输入检测
// #include <raylib.h>
// #include <format>

// int main(void) {
//     // 初始化窗口：宽800、高450、标题"Game"
//     InitWindow(800, 450, "Game");

//     // 加载图片到内存
//     Image logo = LoadImage("../../resources/images/start.png");
//     // 将内存图片传输到显存（创建纹理）
//     Texture2D texture = LoadTextureFromImage(logo);
//     // 释放内存中的图片资源
//     UnloadImage(logo);

//     // 左边logo的位置（初始居中）
//     Vector2 position;
//     position.x = 300 - texture.width / 2;
//     position.y = 225 - texture.height / 2;

//     // 窗口未关闭时持续循环
//     while (!WindowShouldClose()) {
//         // 键盘控制左边logo移动（速度100单位/秒）
//         if (IsKeyDown(KEY_UP)) position.y -= GetFrameTime() * 100;
//         if (IsKeyDown(KEY_DOWN)) position.y += GetFrameTime() * 100;
//         if (IsKeyDown(KEY_LEFT)) position.x -= GetFrameTime() * 100;
//         if (IsKeyDown(KEY_RIGHT)) position.x += GetFrameTime() * 100;

//         // 绘图区
//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         // 绘制左边可移动的logo
//         DrawTextureV(texture, position, WHITE);
//         // 绘制右边固定的logo
//         DrawTexture(texture, 500 - texture.width / 2, 225 - texture.height / 2, WHITE);

//         // 显示FPS
//         DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
//         EndDrawing();
//     }

//     // 先释放显存资源，再关闭窗口
//     UnloadTexture(texture);
//     CloseWindow();
//     return 0;
// }

//关键帧动画
// #include <raylib.h>
// #include <format>

// int main(void) {
//     // 初始化窗口：宽800、高450、标题"Game"
//     InitWindow(800, 450, "Game");

//     // 加载角色行走图到内存
//     Image sprite_sheet = LoadImage("../../resources/images/character.png");
//     // 转换为显存纹理
//     Texture2D texture = LoadTextureFromImage(sprite_sheet);
//     // 释放内存中的图片资源
//     UnloadImage(sprite_sheet);

//     // 精灵图为4行3列，计算单帧大小
//     float character_width = texture.width / 3;
//     float character_height = texture.height / 4;
//     // 单帧区域矩形（初始为第0帧）
//     Rectangle frame_rectangle = { 0.0F, 0.0F, character_width, character_height };

//     // 角色初始位置（窗口居中）
//     Vector2 position{ 400.0F - character_width / 2, 225.0F - character_height / 2 };

//     int curr_frame = 0;          // 当前播放的关键帧序号
//     float timer = 0;             // 帧切换计时器
//     float per_frame_time = 0.3;  // 每帧停留时间（秒）

//     // 窗口主循环
//     while (!WindowShouldClose()) {
//         // 帧动画逻辑
//         timer += GetFrameTime();
//         if (timer >= per_frame_time) {
//             timer = 0;
//             curr_frame = (curr_frame + 1) % 3;  // 循环切换0-2帧
//             // 更新当前帧的矩形区域（X轴偏移）
//             frame_rectangle.x = curr_frame * character_width;
//         }

//         // 绘图区
//         BeginDrawing();
//         ClearBackground(RAYWHITE);
//         // 绘制当前帧的角色
//         DrawTextureRec(texture, frame_rectangle, position, WHITE);
//         // 显示FPS
//         DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
//         EndDrawing();
//     }

//     // 释放资源（先释纹理，再关窗口）
//     UnloadTexture(texture);
//     CloseWindow();
//     return 0;
// }
// #include <raylib.h>
// #include <format>

// int main(void) {
//     InitWindow(800, 450, "Game");

//     // 加载4行3列的角色精灵图
//     Image sprite_sheet = LoadImage("../../resources/images/character.png");
//     Texture2D texture = LoadTextureFromImage(sprite_sheet);
//     UnloadImage(sprite_sheet);

//     // 单帧大小（4行3列）
//     float frame_w = texture.width / 3;
//     float frame_h = texture.height / 4;
//     Rectangle frame_rect = { 0.0F, 0.0F, frame_w, frame_h }; // 帧区域

//     // 角色状态
//     Vector2 pos = { 400 - frame_w/2, 225 - frame_h/2 }; // 初始位置
//     int curr_frame = 0;      // 当前列（0-2）
//     int curr_dir = 0;        // 当前方向（0:下, 1:右, 2:上, 3:左）
//     float timer = 0;
//     float per_frame_time = 0.3;
//     float move_speed = 100;  // 移动速度

//     while (!WindowShouldClose()) {
//         // 1. 方向控制（切换精灵图的行）
//         if (IsKeyDown(KEY_DOWN)) {
//             curr_dir = 0;
//             pos.y += GetFrameTime() * move_speed;
//         } else if (IsKeyDown(KEY_RIGHT)) {
//             curr_dir = 2;
//             pos.x += GetFrameTime() * move_speed;
//         } else if (IsKeyDown(KEY_UP)) {
//             curr_dir = 3;
//             pos.y -= GetFrameTime() * move_speed;
//         } else if (IsKeyDown(KEY_LEFT)) {
//             curr_dir = 1;
//             pos.x -= GetFrameTime() * move_speed;
//         }

//         // 2. 帧动画（切换精灵图的列）
//         timer += GetFrameTime();
//         if (timer >= per_frame_time) {
//             timer = 0;
//             curr_frame = (curr_frame + 1) % 3; // 循环切换0-2列
//         }

//         // 3. 更新当前帧的矩形区域（行=方向，列=当前帧）
//         frame_rect.x = curr_frame * frame_w;
//         frame_rect.y = curr_dir * frame_h;

//         // 绘图
//         BeginDrawing();
//         ClearBackground(RAYWHITE);
//         DrawTextureRec(texture, frame_rect, pos, WHITE);
//         DrawText(std::format("FPS: {}", GetFPS()).c_str(), 5, 5, 20, BLACK);
//         EndDrawing();
//     }

//     UnloadTexture(texture);
//     CloseWindow();
//     return 0;
// }
