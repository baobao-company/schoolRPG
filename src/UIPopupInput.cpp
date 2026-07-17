#include "UIPopupInput.h"
#include <cctype>

// 基类UIBase构造参数：面板宽、高、偏移X、偏移Y
// 弹窗居中计算：窗口1280*720
UIPopupInput::UIPopupInput(float popW, float popH)
    : UIBase(popW, popH, (1280 - popW) / 2.f, (720 - popH) / 2.f),
      m_mode(POP_NUM),
      m_submit(false),
      m_cancel(false)
{
}

void UIPopupInput::setPopup(PopupMode mode, const std::wstring& title)
{
    m_mode = mode;
    m_title = title;
    clearInput();
    resetFlag();
}

void UIPopupInput::clearInput()
{
    m_inputText.clear();
}

std::wstring UIPopupInput::getInputStr() const
{
    return m_inputText;
}

int UIPopupInput::getInputNum() const
{
    int res = 0;
    try
    {
        std::string s(m_inputText.begin(), m_inputText.end());
        res = std::stoi(s);
    }
    catch (...)
    {
        res = 0;
    }
    return res;
}

bool UIPopupInput::isSubmit() const
{
    return m_submit;
}

bool UIPopupInput::isCancel() const
{
    return m_cancel;
}

void UIPopupInput::resetFlag()
{
    m_submit = false;
    m_cancel = false;
}

void UIPopupInput::inputChar(wchar_t c)
{
    if (m_mode == POP_NUM && !isdigit(static_cast<char>(c)))
        return;
    if (m_inputText.size() > 16)
        return;
    m_inputText += c;
}

void UIPopupInput::backspace()
{
    if (!m_inputText.empty())
        m_inputText.pop_back();
}

void UIPopupInput::render(sf::RenderWindow& win, sf::Font& font, void* data)
{
    win.draw(m_bgPanel);
    clearButtons();
    addButton(130, 420, 160, 45, L"确认", 1, font);
    addButton(330, 420, 160, 45, L"取消", 2, font);

    sf::Text t1, t2;
    t1.setFont(font);
    t1.setFillColor(sf::Color::White);
    t1.setCharacterSize(22);
    t1.setString(m_title);
    t1.setPosition(100, 80);

    t2.setFont(font);
    t2.setFillColor(sf::Color::Cyan);
    t2.setCharacterSize(24);
    t2.setString(L"输入：" + m_inputText);
    t2.setPosition(100, 150);

    win.draw(t1);
    win.draw(t2);
    for (auto& btn : m_buttons)
    {
        win.draw(btn.rect);
        win.draw(btn.text);
    }
}

void UIPopupInput::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    // 修复sf::TextEntered不存在，正确是e.type == sf::Event::TextEntered
    if (e.type == sf::Event::TextEntered)
    {
        wchar_t c = static_cast<wchar_t>(e.text.unicode);
        if (c == L'\b')
            backspace();
        else if (c != L'\r')
            inputChar(c);
    }
    // 鼠标移动
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mp);
    }
    // 鼠标左键按下
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mp = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        for (auto& btn : m_buttons)
        {
            if (btn.isMouseOver(mp))
            {
                if (btn.btnId == 1)
                    m_submit = true;
                if (btn.btnId == 2)
                    m_cancel = true;
                break;
            }
        }
    }
}