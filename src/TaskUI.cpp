#include "TaskUI.h"
#include "../core/GameManager.h"
#include "../core/Player.h"
#include "../core/Task.h"
#include <sstream>

TaskUI::TaskUI()
    : UIBase(1020, 630, 90, 45),
    m_tipMsg(L""),
    m_viewMode(0) // 默认打开显示全部任务
{}

// 复刻BattleUI的行数截断，防止文字溢出窗口
std::wstring TaskUI::clipTextByLine(const std::wstring& fullText)
{
    std::wstringstream ss(fullText);
    std::vector<std::wstring> lines;
    std::wstring line;
    while (std::getline(ss, line))
    {
        lines.push_back(line);
    }
    // 超出最大行数，删掉顶部旧内容
    while (lines.size() > MAX_TASK_LINE)
    {
        lines.erase(lines.begin());
    }
    std::wstring out;
    for (auto& l : lines)
    {
        out += l + L"\n";
    }
    return out;
}

void TaskUI::render(sf::RenderWindow& win, sf::Font& font, void* playerData)
{
    win.clear(sf::Color(15, 15, 25));
    Player* p = static_cast<Player*>(playerData);
    TaskManager* tm = p->getTaskManager();
    win.draw(m_bgPanel);
    clearButtons();

    // 底部4个功能按钮布局
    float btnW = 170.f;
    float btnH = 50.f;
    float gap = 15.f;
    float winW = 1280.f;
    float totalWidth = btnW * 4 + gap * 3;
    float startX = (winW - totalWidth) / 2.f;
    float bottomY = 600.f;
    addButton(startX, bottomY, btnW, btnH, L"返回", 996, font);
    addButton(startX + btnW + gap, bottomY, btnW, btnH, L"接取任务", 997, font);
    addButton(startX + (btnW + gap) * 2, bottomY, btnW, btnH, L"查看详情", 998, font);
    addButton(startX + (btnW + gap) * 3, bottomY, btnW, btnH, L"提交任务", 999, font);

    // 文字与行内操作按钮排版参数
    float textX = 120.f;
    float lineYStart = 65.f;
    float lineStep = 32.f;
    float opBtnX = 730.f;
    float opBtnW = 110.f;
    float opBtnH = 28.f;

    sf::Text mainText;
    mainText.setFont(font);
    mainText.setCharacterSize(18);
    mainText.setFillColor(sf::Color::White);

    std::vector<Task*> showList;
    std::wstring viewTitle;

    // 按视图模式筛选任务
    if (m_viewMode == 0)
    {
        // 模式0：全部任务，使用截断防止溢出
        std::wstring fullTaskText = tm->getTaskList();
        mainText.setString(clipTextByLine(fullTaskText));
        mainText.setPosition(textX, lineYStart);
        win.draw(mainText);
    }
    else if (m_viewMode == 1)
    {
        // 模式1：仅可接取（状态Available，满足前置）
        viewTitle = L"====可接取任务（右侧【接取】按钮）====\n";
        std::vector<Task*> allTasks = tm->getAllTaskPtrs();
        for (Task* t : allTasks)
        {
            if (t->getStatus() == TaskStatus::Available && tm->canAccept(t->getId()))
            {
                showList.push_back(t);
            }
        }
    }
    else if (m_viewMode == 2)
    {
        // 模式2：进行中/待完成，查看详情
        viewTitle = L"====进行中任务（右侧【详情】按钮）====\n";
        std::vector<Task*> allTasks = tm->getAllTaskPtrs();
        for (Task* t : allTasks)
        {
            auto st = t->getStatus();
            if (st == TaskStatus::InProgress || st == TaskStatus::Completed)
            {
                showList.push_back(t);
            }
        }
    }
    else if (m_viewMode == 3)
    {
        // 模式3：已完成可领奖
        viewTitle = L"====可提交领奖（右侧【提交】按钮）====\n";
        std::vector<Task*> allTasks = tm->getAllTaskPtrs();
        for (Task* t : allTasks)
        {
            if (t->getStatus() == TaskStatus::Completed)
            {
                showList.push_back(t);
            }
        }
    }

    // 非全部视图：渲染标题 + 每行任务+操作按钮
    if (m_viewMode != 0)
    {
        // 限制条目最大数量，防止按钮超出界面
        while (showList.size() > MAX_TASK_LINE)
        {
            showList.pop_back();
            m_tipMsg = L"条目过多，仅展示前" + std::to_wstring(MAX_TASK_LINE) + L"条";
        }

        std::wstringstream ss;
        ss << viewTitle;
        mainText.setString(ss.str());
        mainText.setPosition(textX, lineYStart);
        win.draw(mainText);

        float curY = lineYStart + lineStep;
        for (int i = 0; i < showList.size(); i++)
        {
            Task* t = showList[i];
            // 绘制任务名称行
            sf::Text lineTxt;
            lineTxt.setFont(font);
            lineTxt.setCharacterSize(18);
            lineTxt.setFillColor(sf::Color::White);
            lineTxt.setString(std::to_wstring(i) + L". " + t->getName() + L" ID:" + std::to_wstring(t->getId()));
            lineTxt.setPosition(textX, curY);
            win.draw(lineTxt);

            // 根据视图切换按钮文字，ID 100+i
            std::wstring btnLabel;
            if (m_viewMode == 1) btnLabel = L"接取";
            else if (m_viewMode == 2) btnLabel = L"详情";
            else if (m_viewMode == 3) btnLabel = L"提交";
            addButton(opBtnX, curY, opBtnW, opBtnH, btnLabel, 100 + i, font);

            curY += lineStep;
        }
    }

    // 底部黄色提示文字
    sf::Text tipTxt;
    tipTxt.setFont(font);
    tipTxt.setFillColor(sf::Color::Yellow);
    tipTxt.setCharacterSize(19);
    tipTxt.setString(m_tipMsg);
    tipTxt.setPosition(100, 440);
    win.draw(tipTxt);

    // 绘制所有按钮（底部功能按钮 + 每行操作按钮）
    for (auto& btn : m_buttons)
    {
        win.draw(btn.rect);
        win.draw(btn.text);
    }
}

void TaskUI::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    // 鼠标悬浮更新按钮hover高亮
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mouse);
    }

    // 鼠标左键点击逻辑
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        Player* p = gm->getPlayer();
        TaskManager* tm = p->getTaskManager();

        // 先获取当前视图任务列表（和渲染保持一致，同步截断）
        std::vector<Task*> showList;
        if (m_viewMode == 1)
        {
            std::vector<Task*> allTasks = tm->getAllTaskPtrs();
            for (Task* t : allTasks)
            {
                if (t->getStatus() == TaskStatus::Available && tm->canAccept(t->getId()))
                    showList.push_back(t);
            }
        }
        else if (m_viewMode == 2)
        {
            std::vector<Task*> allTasks = tm->getAllTaskPtrs();
            for (Task* t : allTasks)
            {
                auto st = t->getStatus();
                if (st == TaskStatus::InProgress || st == TaskStatus::Completed)
                    showList.push_back(t);
            }
        }
        else if (m_viewMode == 3)
        {
            std::vector<Task*> allTasks = tm->getAllTaskPtrs();
            for (Task* t : allTasks)
            {
                if (t->getStatus() == TaskStatus::Completed)
                    showList.push_back(t);
            }
        }
        // 同步截断，防止下标越界
        while (showList.size() > MAX_TASK_LINE)
            showList.pop_back();

        // 遍历所有按钮判断点击
        for (auto& btn : m_buttons)
        {
            if (!btn.isMouseOver(mouse))
                continue;

            // 底部按钮：返回
            if (btn.btnId == 996)
            {
                gm->setState(GameState::MAIN_MENU);
                m_viewMode = 0;
                m_tipMsg.clear();
                break;
            }
            // 切换到可接取视图
            else if (btn.btnId == 997)
            {
                m_viewMode = 1;
                m_tipMsg = L"点击右侧【接取】领取任务";
                break;
            }
            // 切换到进行中详情视图
            else if (btn.btnId == 998)
            {
                m_viewMode = 2;
                m_tipMsg = L"点击右侧【详情】查看任务完整信息";
                break;
            }
            // 切换到可提交视图
            else if (btn.btnId == 999)
            {
                m_viewMode = 3;
                m_tipMsg = L"点击右侧【提交】领取任务奖励";
                break;
            }
            // 行内操作按钮：ID >=100
            else if (btn.btnId >= 100)
            {
                int idx = btn.btnId - 100;
                if (idx < 0 || idx >= (int)showList.size())
                {
                    m_tipMsg = L"任务条目无效";
                    break;
                }
                Task* targetTask = showList[idx];
                int tid = targetTask->getId();

                if (m_viewMode == 1)
                {
                    // 接取任务
                    bool ok = tm->accept(tid);
                    m_tipMsg = ok ? L"任务接取成功！" : L"无法接取该任务";
                }
                else if (m_viewMode == 2)
                {
                    // 查看完整详情
                    m_tipMsg = tm->getDetail(tid);
                }
                else if (m_viewMode == 3)
                {
                    // 提交领奖
                    bool ok = tm->claim(tid);
                    m_tipMsg = ok ? L"领奖成功，已完成任务！" : L"暂不可提交";
                }
                break;
            }
        }
    }
}