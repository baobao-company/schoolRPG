#ifndef TASK_H
#define TASK_H
#include <string>
#include <vector>
#include <map>
#include <memory>
class Player;
class Item;
enum class TaskType { Main, Side };
enum class TaskStatus { Available, InProgress, Completed, Finished };
enum class ConditionType { TalkNpc, CollectItem, KillMonster, ReachLevel, HaveGold };
struct TaskCondition
{
    ConditionType type;
    int targetId, req, curr;
    TaskCondition();
    TaskCondition(ConditionType t, int tid, int r);
    bool isFinish() const;
    std::wstring getText() const;
    void add(int amt=1);
    void reset();
};
struct TaskReward
{
    int baseExp, baseGold;
    float expRate, goldRate;
    std::vector<std::shared_ptr<Item>> items;
    TaskReward();
    TaskReward(int e, int g, float er, float gr);
    TaskReward(int e, int g, float er, float gr, std::vector<std::shared_ptr<Item>> its);
    int calcExp(int lvl) const;
    int calcGold(int lvl) const;
    std::wstring getText(int lvl) const;
};
class Task
{
private:
    int m_id;
    std::wstring m_name, m_desc;
    TaskType m_type;
    TaskStatus m_st;
    int m_pre;
    std::vector<TaskCondition> m_cons;
    TaskReward m_rwd;
public:
    Task(int id, std::wstring name, std::wstring desc, TaskType t, int pre, std::vector<TaskCondition> cons, TaskReward rwd);
    int getId() const;
    std::wstring getName() const;
    std::wstring getDesc() const;
    TaskType getType() const;
    TaskStatus getStatus() const;
    int getPreTask() const;
    const std::vector<TaskCondition>& getConds() const;
    const TaskReward& getReward() const;
    void setStatus(TaskStatus s);
    bool checkComplete(const Player* p);
    void update(ConditionType ty, int tid, int amt);
    std::wstring getStatusStr() const;
    std::wstring getTypeStr() const;
    std::wstring fullInfo(int lvl) const;
    std::wstring progressStr() const;
};
class TaskManager
{
private:
    Player* m_p;
    std::vector<std::unique_ptr<Task>> m_all;
    std::map<int, Task*> m_map;
    void initAllTask();
public:
    TaskManager(Player* p);
    Task* getById(int id) const;
    bool canAccept(int id) const;
    bool accept(int id);
    void onEvent(ConditionType ty, int tid, int amt);
    void checkAll();
    bool claim(int id);
    std::wstring getTaskList() const;
    std::wstring getDetail(int id) const;
    std::wstring getProgressText() const;
    std::string serialize() const;
    void deserialize(const std::string& data);

    std::vector<Task*> getAllTaskPtrs() const;
};
#endif
