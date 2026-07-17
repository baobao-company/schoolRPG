#ifndef UI_BASE_H
#define UI_BASE_H
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
class GameManager;
struct UIButton
{
    sf::RectangleShape rect;
    sf::Text text;
    int btnId;
    bool isHover;
    UIButton(float x, float y, float w, float h, const std::wstring& txt, int id, sf::Font& font);
    bool isMouseOver(sf::Vector2f mousePos) const;
};
class UIBase
{
protected:
    sf::RectangleShape m_bgPanel;
    std::vector<UIButton> m_buttons;
public:
    UIBase(float panelW, float panelH, float offsetX = 100, float offsetY = 50);
    virtual ~UIBase() = default;
    virtual void render(sf::RenderWindow& win, sf::Font& font, void* playerData) = 0;
    virtual void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) = 0;
    void clearButtons();
    void addButton(float x, float y, float w, float h, const std::wstring& txt, int id, sf::Font& font);
    void updateButtonHover(sf::Vector2f mousePos);
};
#endif