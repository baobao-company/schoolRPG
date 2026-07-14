#ifndef TASK_H
#define TASK_H


#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iomanip>
#include "Item.h"     
#include "Player.h"   

// 枚举类型定义
enum class TaskType 
{
    Main,   // 主线任务
    Side    // 支线任务
};

//任务状态枚举
enum class TaskStatus 
{
    Available,   // 可接取
    InProgress,  // 进行中
    Completed,   // 已完成
    Finished     // 已领奖
};

//任务条件类型枚举
enum class ConditionType 
{
    TalkNpc,      // 与NPC对话：targetId=NPC编号
    CollectItem,  // 收集物品：targetId=物品ID
    KillMonster,  // 击杀怪物：targetId=怪物ID（0表示任意）
    ReachLevel,   // 达到等级：targetId=目标等级
    HaveGold      // 拥有金币：targetId=目标金币数
};
// TaskCondition 任务条件结构体
struct TaskCondition 
{
    ConditionType type;        // 条件类型
    int targetId;              // 目标ID（0表示任意）
    int requiredCount;         // 所需数量
    int currentCount;          // 当前进度（运行时更新）
    // ----- 构造函数 -----
    TaskCondition() : type(ConditionType::TalkNpc), targetId(0), requiredCount(1), currentCount(0) {}
    TaskCondition(ConditionType _type, int _targetId, int _requiredCount): type(_type), targetId(_targetId), requiredCount(_requiredCount), currentCount(0) {}
    //判断条件是否达成
    bool isMet() const 
    {
        return currentCount >= requiredCount;
    }
    //获取进度字符串
    std::string getProgressString() const 
    {
        return std::to_string(currentCount) + " / " + std::to_string(requiredCount);
    }

    //获取条件类型的中文名称
    std::string getConditionTypeString() const 
    {
        switch (type)
        {
            case ConditionType::TalkNpc:     return "与NPC对话";
            case ConditionType::CollectItem: return "收集物品";
            case ConditionType::KillMonster: return "击杀怪物";
            case ConditionType::ReachLevel:  return "达到等级";
            case ConditionType::HaveGold:    return "拥有金币";
            default: return "未知";
        }
    }
    //获取完整条件描述
    std::string getDescription() const 
    {
        std::string desc = getConditionTypeString();
        if (targetId != 0) 
        {
            desc += " (ID:" + std::to_string(targetId) + ")";
        }
        return desc + " " + getProgressString();
    }

    //推进进度
    void addProgress(int amount = 1) 
    {
        if (currentCount < requiredCount) 
        {
            currentCount += amount;
            if (currentCount > requiredCount) 
            {
                currentCount = requiredCount;
            }
        }
    }

    //重置进度
    void reset() 
    {
        currentCount = 0;
    }
};
// TaskReward 任务奖励结构体
struct TaskReward 
{
    int baseExp;                          // 基础经验值
    int baseGold;                         // 基础金币数
    float expGrowthRate;                  // 经验增长率（每级额外加的经验）
    float goldGrowthRate;                 // 金币增长率（每级额外加的金币）
    std::vector<std::shared_ptr<Item>> rewardItems;  // 奖励物品列表
    TaskReward() : baseExp(0), baseGold(0), expGrowthRate(0), goldGrowthRate(0) {}
    // 无物品奖励
    TaskReward(int _baseExp, int _baseGold, float _expRate, float _goldRate): baseExp(_baseExp), baseGold(_baseGold),expGrowthRate(_expRate), goldGrowthRate(_goldRate) {}
    // 有物品奖励
    TaskReward(int _baseExp, int _baseGold, float _expRate, float _goldRate,const std::vector<std::shared_ptr<Item>>& _items)
        : baseExp(_baseExp), baseGold(_baseGold),expGrowthRate(_expRate), goldGrowthRate(_goldRate),rewardItems(_items) {}
    
    //计算方法
    int calculateExp(int playerLevel) const 
    {
        return baseExp + static_cast<int>(playerLevel * expGrowthRate);
    }

    //计算实际金币奖励,公式：基础金币 + 玩家等级 × 增长率
    int calculateGold(int playerLevel) const 
    {
        return baseGold + static_cast<int>(playerLevel * goldGrowthRate);
    }

    //获取奖励描述字符串
    std::string getDescription(int playerLevel) const 
    {
        std::string desc = "EXP: " + std::to_string(calculateExp(playerLevel)) +", 金币: " + std::to_string(calculateGold(playerLevel));
        if (!rewardItems.empty()) 
        {
            desc += ", 物品: ";
            for (size_t i = 0; i < rewardItems.size(); ++i) 
            {
                if (rewardItems[i]) 
                {
                    desc += rewardItems[i]->getName();
                }
                if (i < rewardItems.size() - 1) desc += ", ";
            }
        }
        return desc;
    }
};
// Task 任务类
class Task 
{
    private:
    int m_id;                         // 任务唯一ID
    std::string m_name;               // 任务名称
    std::string m_description;        // 任务描述
    TaskType m_type;                  // 任务类型
    TaskStatus m_status;              // 任务状态
    int m_preTaskId;                  // 前置任务ID（-1表示无前置）
    std::vector<TaskCondition> m_conditions;  // 完成条件列表
    TaskReward m_reward;              // 任务奖励
public:
    Task() : m_id(0), m_status(TaskStatus::Available), m_preTaskId(-1) {}
    Task(int id, const std::string& name, const std::string& desc,TaskType type, int preTaskId,const std::vector<TaskCondition>& conditions,const TaskReward& reward);
    //Getter方法
    int getId() const { return m_id; }
    std::string getName() const { return m_name; }
    std::string getDescription() const { return m_description; }
    TaskType getType() const { return m_type; }
    TaskStatus getStatus() const { return m_status; }
    int getPreTaskId() const { return m_preTaskId; }
    const std::vector<TaskCondition>& getConditions() const { return m_conditions; }
    const TaskReward& getReward() const { return m_reward; }
    //Setter 方法
    void setStatus(TaskStatus status) { m_status = status; }

    //核心
    //检查任务是否完成
    bool checkCompletion(const Player* player);
    //更新任务进度
    void updateProgress(ConditionType type, int targetId, int amount = 1);
    //计算经验奖励
    int getExpReward(int playerLevel) const 
    {
        return m_reward.calculateExp(playerLevel);
    }
    //计算金币奖励
    int getGoldReward(int playerLevel) const 
    {
        return m_reward.calculateGold(playerLevel);
    }
    //获取奖励物品列表
    const std::vector<std::shared_ptr<Item>>& getRewardItems() const 
    {
        return m_reward.rewardItems;
    }
    //界面
    std::string getStatusString() const;
    std::string getTypeString() const;
    std::string getFullInfo(int playerLevel) const;
    std::string getProgressSummary() const;
    bool hasAnyTargetCondition() const;
};
// TaskManager 任务管理器
//任务管理器类,负责管理所有任务的创建、状态维护、进度更新、奖励发放
class TaskManager 
{
    private:
    //存储所有任务的容器(使用 unique_ptr 自动管理内存)
    std::vector<std::unique_ptr<Task>> m_allTasks;
    //ID,任务指针的映射表
    std::map<int, Task*> m_taskMap;
    Player* m_player;

    //初始化所有任务数据
    void initTasks();

    public:
    TaskManager(Player* player);
    ~TaskManager() = default;
    //任务查询
    Task* getTask(int taskId) const;
    bool isTaskAvailable(int taskId) const;
    //任务操作
    //接取任务
    bool acceptTask(int taskId);
    //事件触发接口
    void onEvent(ConditionType type, int targetId, int amount = 1);
    //检查所有进行中的任务是否完成
    void checkAllTasks();
    //手动完成任务（通常由 checkAllTasks 自动完成）
    bool completeTask(int taskId);
    //领取任务奖励
    bool claimReward(int taskId);

    //界面显示
    void showTaskList() const;
    void showTaskDetail(int taskId) const;
    void showProgress() const;

    //数据持久化
    void resetAllTasks();
    std::string serializeProgress() const;
    void deserializeProgress(const std::string& data);
};

#endif // TASK_H
