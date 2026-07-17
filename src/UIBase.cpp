#include "UIBase.h"
UIButton::UIButton(float x, float y, float w, float h, const std::wstring& txt, int id, sf::Font& font)
{
    rect.setPosition(x, y);
    rect.setSize({w, h});
    rect.setFillColor(sf::Color(40, 60, 110));
    rect.setOutlineColor(sf::Color(120, 160, 255));
    rect.setOutlineThickness(1.f);
    text.setFont(font);
    text.setString(txt);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    text.setPosition(x + 10, y + 8);
    btnId = id;
    isHover = false;
}
bool UIButton::isMouseOver(sf::Vector2f mousePos) const
{
    return rect.getGlobalBounds().contains(mousePos);
}
UIBase::UIBase(float panelW, float panelH, float offsetX, float offsetY)
{
    m_bgPanel.setSize({panelW, panelH});
    m_bgPanel.setPosition(offsetX, offsetY);
    m_bgPanel.setFillColor(sf::Color(20, 22, 35, 230));
    m_bgPanel.setOutlineColor(sf::Color(80, 120, 220));
    m_bgPanel.setOutlineThickness(2.f);
}
void UIBase::clearButtons()
{
    m_buttons.clear();
}
void UIBase::addButton(float x, float y, float w, float h, const std::wstring& txt, int id, sf::Font& font)
{
    m_buttons.emplace_back(x, y, w, h, txt, id, font);
}
void UIBase::updateButtonHover(sf::Vector2f mousePos)
{
    for (auto& btn : m_buttons)
    {
        btn.isHover = btn.isMouseOver(mousePos);
        if (btn.isHover)
            btn.rect.setFillColor(sf::Color(70, 110, 190));
        else
            btn.rect.setFillColor(sf::Color(40, 60, 110));
    }
}