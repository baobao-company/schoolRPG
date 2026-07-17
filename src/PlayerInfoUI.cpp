#include "PlayerInfoUI.h"
#include "../core/GameManager.h"
#include "../core/Player.h"
PlayerInfoUI::PlayerInfoUI() : UIBase(900, 580, 150, 70)
{}
void PlayerInfoUI::render(sf::RenderWindow& win, sf::Font& font, void* playerData)
{
    Player* p = static_cast<Player*>(playerData);
    win.draw(m_bgPanel);
    clearButtons();
    addButton(380, 480, 140, 45, L"返回主菜单", 0, font);
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(22);
    infoText.setFillColor(sf::Color::White);
    infoText.setString(p->getInfoText());
    infoText.setPosition(200, 100);
    win.draw(infoText);
    for (auto& btn : m_buttons)
    {
        win.draw(btn.rect);
        win.draw(btn.text);
    }
}
void PlayerInfoUI::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mouse);
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        for (auto& btn : m_buttons)
        {
            if (btn.isMouseOver(mouse) && btn.btnId == 0)
            {
                gm->setState(GameState::MAIN_MENU);
                break;
            }
        }
    }
}