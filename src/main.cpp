#ifdef _WIN32
#include <windows.h>
#endif
#include <SFML/Graphics.hpp>
#include "src/core/GameManager.h"
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    sf::RenderWindow window(sf::VideoMode(1280, 720), L"校园RPG");
    window.setFramerateLimit(60);
    sf::Font globalFont;
    std::string fontPath = "res/fonts/msyh.ttc";
    if (!globalFont.loadFromFile(fontPath))
    {
        MessageBoxA(nullptr, "字体文件缺失 res/fonts/msyh.ttc", "资源错误", MB_ICONERROR);
        return -1;
    }
    GameManager game;
    sf::Event event;
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
           if (event.type == sf::Event::Closed)
            {
                game.doSave();
                window.close();
            }
            game.handleSFEvent(event, window);
        }
        window.clear(sf::Color(15,15,25));
        game.renderAll(window, globalFont);
        window.display();
    }
    return 0;
}
