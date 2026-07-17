#ifndef TASK_UI_H
#define TASK_UI_H
#include "UIBase.h"
#include <vector>
class Task;
class TaskUI : public UIBase
{
private:
    std::wstring m_tipMsg;
    // 视图模式 0=全部任务 1=可接取 2=进行中详情 3=可提交领奖
    int m_viewMode;
    // 和BattleUI统一：最大显示行数，防止文字/按钮出界
    const int MAX_TASK_LINE = 13;
    // 复刻战斗日志文本截断函数
    std::wstring clipTextByLine(const std::wstring& fullText);
public:
    TaskUI();
    void render(sf::RenderWindow& win, sf::Font& font, void* playerData) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
};
#endif