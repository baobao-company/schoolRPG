#include "GameManager.h"
#include <iostream>
#include <cstdlib>

//主函数
int main()
 {
    // 设置控制台编码（Windows）
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif
    // 创建游戏管理器
    GameManager game;
    // 运行游戏
    game.run();
    return 0;
}