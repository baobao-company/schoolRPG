#include "Task.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include <iostream>
#include <sstream>

Task::Task(int id, const std::string& name, const std::string& desc,TaskType type, int preTaskId,const std::vector<TaskCondition>& conditions,const TaskReward& reward)
    : m_id(id), m_name(name), m_description(desc), m_type(type),m_status(TaskStatus::Available), m_preTaskId(preTaskId),m_conditions(conditions), m_reward(reward) {}

//检查任务是否完成
bool Task::checkCompletion(const Player* player) 
{

    // 只有进行中的任务才需要检查
    if (m_status != TaskStatus::InProgress) return false;
    // 遍历所有条件
    for (const auto& cond : m_conditions) 
    {
        if (!cond.isMet()) return false;
    }
    return true;
}

//更新任务进度
void Task::updateProgress(ConditionType type, int targetId, int amount) 
{
    // 只有进行中的任务才能更新进度
    if (m_status != TaskStatus::InProgress) return;
    for (auto& cond : m_conditions) 
    {
        // 条件类型匹配 && (目标匹配 或 条件要求任意目标)
        if (cond.type == type && (cond.targetId == 0 || cond.targetId == targetId)) 
        {
            cond.addProgress(amount);
        }
    }
}
//获取状态的中文名称
std::string Task::getStatusString() const
{
    switch (m_status) {
        case TaskStatus::Available:   return "可接取";
        case TaskStatus::InProgress:  return "进行中";
        case TaskStatus::Completed:   return "待领奖 ";
        case TaskStatus::Finished:    return "已完成 ";
        default: return "未知";
    }
}
//获取类型的中文名称
std::string Task::getTypeString() const 
{
    return m_type == TaskType::Main ? "【主线】" : "【支线】";
}

//获取完整任务信息
std::string Task::getFullInfo(int playerLevel) const 
{
    std::string result;
    result += "═══════════════════════════════════════\n";
    result += getTypeString() + " " + m_name + "\n";
    result += "状态: " + getStatusString() + "\n";
    result += "描述: " + m_description + "\n";
    result += "─────────────────────────────────────\n";
    result += "【完成条件】\n";
    for (const auto& cond : m_conditions) {
        result += "  " + cond.getDescription() + "\n";
    }
    result += "─────────────────────────────────────\n";
    result += "【奖励】\n";
    result += "  " + m_reward.getDescription(playerLevel) + "\n";
    if (m_preTaskId != -1) {
        result += "【前置任务】ID: " + std::to_string(m_preTaskId) + "\n";
    }
    result += "═══════════════════════════════════════\n";
    return result;
}

//获取进度摘要
std::string Task::getProgressSummary() const 
{
    if (m_status != TaskStatus::InProgress) return "";
    std::string result;
    for (const auto& cond : m_conditions) 
    {
        if (!result.empty()) result += ", ";
        result += cond.getDescription();
    }
    return result;
}

//判断是否有"任意目标"的条件
bool Task::hasAnyTargetCondition() const 
{
    for (const auto& cond : m_conditions) 
    {
        if (cond.targetId == 0) return true;
    }
    return false;
}

// TaskManager 类的实现
TaskManager::TaskManager(Player* player) : m_player(player) {initTasks();}

//初始化所有任务数据
void TaskManager::initTasks() 
{
    // 主线任务（8个，构成完整故事线）
    // M001: 初入校园 - 新手引导任务
    auto m001 = std::make_unique<Task>(
        1001, "初入校园", "去教务处找辅导员报到，开启你的校园冒险之旅",
        TaskType::Main, -1,
        std::vector<TaskCondition>{ {ConditionType::TalkNpc, 1, 1} },
        TaskReward(20, 10, 0.5f, 0.3f)
    );
    m_taskMap[1001] = m001.get();
    m_allTasks.push_back(std::move(m001));
    // M002: 领取教材 - 熟悉背包系统
    auto m002 = std::make_unique<Task>(
        1002, "领取教材", "去图书馆领取新学期教材",
        TaskType::Main, 1001,
        std::vector<TaskCondition>{ {ConditionType::CollectItem, 101, 1} },
        TaskReward(30, 5, 0.5f, 0.3f)
    );
    m_taskMap[1002] = m002.get();
    m_allTasks.push_back(std::move(m002));
    // M003: 消灭骚扰者 - 第一次战斗教学
    // 奖励：体力药水 × 1
    auto m003 = std::make_unique<Task>(
        1003, "消灭骚扰者", "校门口有野猫袭击同学，赶走它们",
        TaskType::Main, 1002,
        std::vector<TaskCondition>{ {ConditionType::KillMonster, 1, 3} },
        TaskReward(50, 20, 1.0f, 0.5f,
            std::vector<std::shared_ptr<Item>>{
                std::make_shared<Medicine>("体力药水", 1, 30)
            })
    );
    m_taskMap[1003] = m003.get();
    m_allTasks.push_back(std::move(m003));

    // M004: 清理后山
    auto m004 = std::make_unique<Task>(
        1004, "清理后山", "后山池塘的鱼发生变异，清理掉它们",
        TaskType::Main, 1003,
        std::vector<TaskCondition>{ {ConditionType::KillMonster, 2, 5} },
        TaskReward(80, 50, 1.5f, 1.0f)
    );
    m_taskMap[1004] = m004.get();
    m_allTasks.push_back(std::move(m004));

    // M005: 升级考核 - 引导玩家成长
    // 奖励：新手短剑 × 1（武器，可强化）
    auto m005 = std::make_unique<Task>(
        1005, "升级考核", "等级达到10级，通过辅导员的实力考核",
        TaskType::Main, 1004,
        std::vector<TaskCondition>{ {ConditionType::ReachLevel, 10, 1} },
        TaskReward(100, 80, 2.0f, 1.5f,
            std::vector<std::shared_ptr<Item>>{
                std::make_shared<Weapon>("新手短剑", 1, 8)
            })
    );
    m_taskMap[1005] = m005.get();
    m_allTasks.push_back(std::move(m005));

    // M006: 湖边巡逻
    auto m006 = std::make_unique<Task>(
        1006, "湖边巡逻", "夜晚校园湖边出现巨型白鹅袭击路人，去处理",
        TaskType::Main, 1005,
        std::vector<TaskCondition>{ {ConditionType::KillMonster, 3, 3} },
        TaskReward(120, 100, 2.0f, 1.5f)
    );
    m_taskMap[1006] = m006.get();
    m_allTasks.push_back(std::move(m006));

    // M007: 保卫教学楼
    // 奖励：高级回复药水 × 2 + 能量面包 × 3
    auto m007 = std::make_unique<Task>(
        1007, "保卫教学楼", "大批怪物涌入教学楼，组织防御战",
        TaskType::Main, 1006,
        std::vector<TaskCondition>{ {ConditionType::KillMonster, 0, 10} },
        TaskReward(200, 150, 3.0f, 2.0f,
            std::vector<std::shared_ptr<Item>>{
                std::make_shared<Medicine>("高级回复药水", 2, 80),
                std::make_shared<Food>("能量面包", 3, 20, 10)
            })
    );
    m_taskMap[1007] = m007.get();
    m_allTasks.push_back(std::move(m007));

    // M008: 幕后黑手 - 最终Boss战
    // 奖励：传说之剑 × 1 + 暗影水晶（传说材料）× 1
    auto m008 = std::make_unique<Task>(
        1008, "幕后黑手", "击败异变事件的罪魁祸首——暗影校长",
        TaskType::Main, 1007,
        std::vector<TaskCondition>{ {ConditionType::KillMonster, 4, 1} },
        TaskReward(500, 300, 5.0f, 3.0f,
            std::vector<std::shared_ptr<Item>>{
                std::make_shared<Weapon>("传说之剑", 1, 35),
                std::make_shared<Material>("暗影水晶", 1, Material::LEGENDARY)
            })
    );
    m_taskMap[1008] = m008.get();
    m_allTasks.push_back(std::move(m008));

    // 支线任务（5个，相互独立）

    // S001: 食堂打工 - 金币获取型支线
    auto s001 = std::make_unique<Task>(
        2001, "食堂打工", "帮食堂阿姨送餐，赚点零花钱",
        TaskType::Side, -1,
        std::vector<TaskCondition>{ {ConditionType::HaveGold, 300, 1} },
        TaskReward(40, 30, 0.5f, 3.0f)
    );
    m_taskMap[2001] = s001.get();
    m_allTasks.push_back(std::move(s001));

    // S002: 收集魔法树叶
    // 奖励：稀有材料 × 2
    auto s002 = std::make_unique<Task>(
        2002, "收集魔法树叶", "收集发光的魔法树叶交给老师",
        TaskType::Side, -1,
        std::vector<TaskCondition>{ {ConditionType::CollectItem, 102, 5} },
        TaskReward(60, 40, 1.0f, 1.0f,
            std::vector<std::shared_ptr<Item>>{
                std::make_shared<Material>("魔法树叶（稀有）", 2, Material::UNCOMMON)
            })
    );
    m_taskMap[2002] = s002.get();
    m_allTasks.push_back(std::move(s002));

    // S003: 装备收集者
    // 奖励：运动护腕 × 1（武器，可强化）
    auto s003 = std::make_unique<Task>(
        2003, "装备收集者", "收集足够多的装备武装自己",
        TaskType::Side, -1,
        std::vector<TaskCondition>{ {ConditionType::CollectItem, 0, 3} },
        TaskReward(80, 0, 1.5f, 0.5f,
            std::vector<std::shared_ptr<Item>>{
                std::make_shared<Weapon>("运动护腕", 1, 5)
            })
    );
    m_taskMap[2003] = s003.get();
    m_allTasks.push_back(std::move(s003));

    // S004: 悬赏猎手
    auto s004 = std::make_unique<Task>(
        2004, "悬赏猎手", "成为校园悬赏榜的顶级猎手",
        TaskType::Side, -1,
        std::vector<TaskCondition>{ {ConditionType::KillMonster, 0, 15} },
        TaskReward(100, 80, 2.0f, 2.0f)
    );
    m_taskMap[2004] = s004.get();
    m_allTasks.push_back(std::move(s004));

    // S005: 校园富豪
    auto s005 = std::make_unique<Task>(
        2005, "校园富豪", "攒够一笔巨款，体验当富豪的感觉",
        TaskType::Side, -1,
        std::vector<TaskCondition>{ {ConditionType::HaveGold, 1000, 1} },
        TaskReward(60, 100, 0.5f, 4.0f)
    );
    m_taskMap[2005] = s005.get();
    m_allTasks.push_back(std::move(s005));
}

//通过ID获取任务指针
Task* TaskManager::getTask(int taskId) const 
{
    auto it = m_taskMap.find(taskId);
    return it != m_taskMap.end() ? it->second : nullptr;
}

//判断任务是否可接取
bool TaskManager::isTaskAvailable(int taskId) const 
{
    Task* task = getTask(taskId);
    if (!task || task->getStatus() != TaskStatus::Available) return false;
    int preId = task->getPreTaskId();
    if (preId != -1) 
    {
        Task* preTask = getTask(preId);
        if (!preTask || preTask->getStatus() != TaskStatus::Finished) return false;
    }
    return true;
}

//接取任务
bool TaskManager::acceptTask(int taskId) 
{
    Task* task = getTask(taskId);
    if (!task) 
    {
        std::cout << "任务不存在！" << std::endl;
        return false;
    }
    if (!isTaskAvailable(taskId)) 
    {
        std::cout << " 该任务暂不可接取！（可能前置任务未完成）" << std::endl;
        return false;
    }
    task->setStatus(TaskStatus::InProgress);
    std::cout << " 已接取任务: " << task->getName() << std::endl;
    return true;
}

//事件触发接口（核心方法）
void TaskManager::onEvent(ConditionType type, int targetId, int amount) 
{
    for (auto& taskPtr : m_allTasks) 
    {
        if (taskPtr->getStatus() == TaskStatus::InProgress) 
        {
            taskPtr->updateProgress(type, targetId, amount);
        }
    }
    // 事件更新后自动检查是否有任务完成
    checkAllTasks();
}
//检查所有进行中的任务是否完成
void TaskManager::checkAllTasks() 
{
    for (auto& taskPtr : m_allTasks) 
    {
        if (taskPtr->getStatus() == TaskStatus::InProgress) 
        {
            if (taskPtr->checkCompletion(m_player)) 
            {
                taskPtr->setStatus(TaskStatus::Completed);
                std::cout << "任务完成: " << taskPtr->getName() << "！请领取奖励！" << std::endl;
            }
        }
    }
}

//手动完成任务（保留接口）,通常由 checkAllTasks 自动完成
bool TaskManager::completeTask(int taskId) 
{
    Task* task = getTask(taskId);
    if (!task || task->getStatus() != TaskStatus::InProgress) return false;
    if (task->checkCompletion(m_player)) 
    {
        task->setStatus(TaskStatus::Completed);
        return true;
    }
    return false;
}

//领取任务奖励
bool TaskManager::claimReward(int taskId) 
{
    Task* task = getTask(taskId);
    if (!task) 
    {
        std::cout << "任务不存在！" << std::endl;
        return false;
    }
    if (task->getStatus() != TaskStatus::Completed) 
    {
        std::cout << "该任务尚未完成或已领取奖励！" << std::endl;
        return false;
    }
    int level = m_player->getLevel();
    int expReward = task->getExpReward(level);
    int goldReward = task->getGoldReward(level);
    // 1. 发放经验奖励
    m_player->addExp(expReward);
    // 2. 发放金币奖励
    m_player->addGold(goldReward);
    std::cout << " 领取奖励成功！" << std::endl;
    // 3. 发放物品奖励
    const auto& items = task->getRewardItems();
    Bag* bag = m_player->getBag();
    for (const auto& itemPtr : items) 
    {
        if (itemPtr) 
        {
            // 使用 cloneItem 深拷贝物品
            // 因为任务持有的是 shared_ptr，背包需要独立管理内存
            Item* itemCopy = cloneItem(itemPtr.get());
            if (itemCopy) 
            {
                bag->addItem(itemCopy);
            } 
            else 
            {
                std::cout << " 物品克隆失败！" << std::endl;
            }
        }
    }
    task->setStatus(TaskStatus::Finished);
    return true;
}

//显示所有任务列表
void TaskManager::showTaskList() const {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║                     任务面板                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    //主线任务
    std::cout << "\n【主线任务】\n";
    std::cout << "─────────────────────────────────────────────────────────\n";
    int mainCount = 0;
    for (const auto& taskPtr : m_allTasks) 
    {
        if (taskPtr->getType() == TaskType::Main) 
        {
            mainCount++;
            std::cout << "  [" << taskPtr->getStatusString() << "] "<< taskPtr->getName() << " (ID:" << taskPtr->getId() << ")\n";
            if (taskPtr->getStatus() == TaskStatus::InProgress) 
            {
                std::cout << "    进度: " << taskPtr->getProgressSummary() << "\n";
            }
        }
    }
    if (mainCount == 0) std::cout << "  (暂无主线任务)\n";
    //支线任务
    std::cout << "\n 【支线任务】\n";
    std::cout << "─────────────────────────────────────────────────────────\n";
    int sideCount = 0;
    for (const auto& taskPtr : m_allTasks) 
    {
        if (taskPtr->getType() == TaskType::Side) 
        {
            sideCount++;
            std::cout << "  [" << taskPtr->getStatusString() << "] "<< taskPtr->getName() << " (ID:" << taskPtr->getId() << ")\n";
            if (taskPtr->getStatus() == TaskStatus::InProgress) 
            {
                std::cout << "    进度: " << taskPtr->getProgressSummary() << "\n";
            }
        }
    }
    if (sideCount == 0) std::cout << "  (暂无支线任务)\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
}

//显示特定任务的详细信息，包括描述、条件、奖励等
void TaskManager::showTaskDetail(int taskId) const 
{
    Task* task = getTask(taskId);
    if (!task) 
    {
        std::cout << "任务不存在！" << std::endl;
        return;
    }
    std::cout << task->getFullInfo(m_player->getLevel());
    if (task->getStatus() == TaskStatus::Available) 
    {
        std::cout << " 输入 task accept " << taskId << " 接取此任务\n";
    } 
    else if (task->getStatus() == TaskStatus::Completed) {
        std::cout << " 输入 task claim " << taskId << " 领取奖励\n";
    }
}

//显示当前进行中任务的进度
void TaskManager::showProgress() const 
{
    std::cout << "\n 【当前任务进度】\n";
    bool hasProgress = false;
    for (const auto& taskPtr : m_allTasks) 
    {
        if (taskPtr->getStatus() == TaskStatus::InProgress) 
        {
            hasProgress = true;
            std::cout << "  " << taskPtr->getName() << ": " << taskPtr->getProgressSummary() << "\n";
        }
    }
    if (!hasProgress) 
    {
        std::cout << "  当前没有进行中的任务。\n";
    }
}

//重置所有任务（新游戏时调用）
void TaskManager::resetAllTasks() 
{
    for (auto& taskPtr : m_allTasks) 
    {
        taskPtr->setStatus(TaskStatus::Available);
        for (auto& cond : taskPtr->getConditions()) 
        {
            const_cast<TaskCondition&>(cond).reset();
        }
    }
}

//序列化任务进度（用于存档）
std::string TaskManager::serializeProgress() const 
{
    std::ostringstream oss;
    for (const auto& taskPtr : m_allTasks) 
    {
        oss << taskPtr->getId() << ","
            << static_cast<int>(taskPtr->getStatus()) << ",";
        for (const auto& cond : taskPtr->getConditions()) 
        {
            oss << cond.currentCount << ",";
        }
        oss << "|";
    }
    return oss.str();
}

//反序列化任务进度（用于读档）
void TaskManager::deserializeProgress(const std::string& data) 
{
    if (data.empty()) return;
    std::stringstream ss(data);
    std::string taskData;
    while (std::getline(ss, taskData, '|')) 
    {
        if (taskData.empty()) continue;    
        std::stringstream taskSS(taskData);
        std::string token;
        std::vector<std::string> parts;
        while (std::getline(taskSS, token, ',')) 
        {
            if (!token.empty()) parts.push_back(token);
        }
        
        if (parts.size() >= 2) 
        {
            int taskId = std::stoi(parts[0]);
            TaskStatus status = static_cast<TaskStatus>(std::stoi(parts[1]));
            Task* task = getTask(taskId);
            if (task) 
            {
                task->setStatus(status);   
                // 恢复条件进度
                const auto& conditions = task->getConditions();
                for (size_t i = 0; i < conditions.size() && i + 2 < parts.size(); ++i) 
                {
                    int progress = std::stoi(parts[i + 2]);
                    const_cast<TaskCondition&>(conditions[i]).currentCount = progress;
                }
            }
        }
    }
}