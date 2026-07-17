#ifndef UI_POPUP_INPUT_H
#define UI_POPUP_INPUT_H
#include "UIBase.h"
#include <string>
// 必须引入SFML头
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include "../core/GameManager.h"

enum PopupMode
{
    POP_NUM,    // 数字输入
    POP_TEXT    // 文字输入
};

class UIPopupInput : public UIBase
{
private:
    PopupMode m_mode;
    std::wstring m_title;
    std::wstring m_inputText;
    bool m_submit;
    bool m_cancel;
public:
    // 唯一合法构造：宽、高，居中弹窗
    UIPopupInput(float popW, float popH);
    void setPopup(PopupMode mode, const std::wstring& title);
    void clearInput();
    std::wstring getInputStr() const;
    int getInputNum() const;
    bool isSubmit() const;
    bool isCancel() const;
    void resetFlag();
    void inputChar(wchar_t c);
    void backspace();
    void render(sf::RenderWindow& win, sf::Font& font, void* data) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
};
#endif