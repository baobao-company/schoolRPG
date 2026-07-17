#include "Task.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include <sstream>
TaskCondition::TaskCondition() : type(ConditionType::TalkNpc), targetId(0), req(1), curr(0) {}
TaskCondition::TaskCondition(ConditionType t, int tid, int r) : type(t), targetId(tid), req(r), curr(0) {}
bool TaskCondition::isFinish() const { return curr >= req; }
std::wstring TaskCondition::getText() const
{
    std::wstring ty;
    switch(type)
    {
       case ConditionType::TalkNpc: ty=L"对话NPC";break;
        case ConditionType::CollectItem: ty=L"收集物品";break;
        case ConditionType::KillMonster: ty=L"击杀怪物";break;
        case ConditionType::ReachLevel: ty=L"达到等级";break;
        case ConditionType::HaveGold: ty=L"拥有金币";
        default: ty=L"未知";
    }
    std::wstringstream ss;
    ss << ty << L" " << std::to_wstring(curr) << L"/" << std::to_wstring(req);
    return ss.str();
}
void TaskCondition::add(int amt)
{
    if (curr < req) curr += amt;
    if (curr > req) curr = req;
}
void TaskCondition::reset() { curr =0; }
TaskReward::TaskReward() : baseExp(0),baseGold(0),expRate(0),goldRate(0) {}
TaskReward::TaskReward(int e, int g, float er, float gr) : baseExp(e),baseGold(g),expRate(er),goldRate(gr) {}
TaskReward::TaskReward(int e, int g, float er, float gr, std::vector<std::shared_ptr<Item>> its)
    : baseExp(e),baseGold(g),expRate(er),goldRate(gr),items(its) {}
int TaskReward::calcExp(int lvl)const { return baseExp + (int)(lvl * expRate); }
int TaskReward::calcGold(int lvl)const { return baseGold + (int)(lvl * goldRate); }
std::wstring TaskReward::getText(int lvl) const
{
    std::wstringstream ss;
    ss << L"EXP:" << std::to_wstring(calcExp(lvl)) << L" 金币:" << std::to_wstring(calcGold(lvl));
    if (!items.empty())
    {
        ss << L" 物品:";
        for (auto& it : items) ss << it->getName() << L" ";
    }
    return ss.str();
}
Task::Task(int id, std::wstring name, std::wstring desc, TaskType t, int pre, std::vector<TaskCondition> cons, TaskReward rwd)
    : m_id(id), m_name(name), m_desc(desc), m_type(t), m_st(TaskStatus::Available), m_pre(pre), m_cons(cons), m_rwd(rwd) {}
int Task::getId() const { return m_id; }
std::wstring Task::getName() const { return m_name; }
std::wstring Task::getDesc() const { return m_desc; }
TaskType Task::getType() const { return m_type; }
TaskStatus Task::getStatus() const { return m_st; }
int Task::getPreTask() const { return m_pre; }
const std::vector<TaskCondition>& Task::getConds() const 
{ 
    return m_cons; 
}

const TaskReward& Task::getReward() const 
{ 
    return m_rwd; 
}
void Task::setStatus(TaskStatus s) { m_st = s; }
bool Task::checkComplete(const Player*)
{
    if (m_st != TaskStatus::InProgress) return false;
    for (auto& c : m_cons) if (!c.isFinish()) return false;
    return true;
}
void Task::update(ConditionType ty, int tid, int amt)
{
    if (m_st != TaskStatus::InProgress) return;
    for (auto& c : m_cons)
        if (c.type == ty && (c.targetId ==0 || c.targetId == tid))
            c.add(amt);
}
std::wstring Task::getStatusStr() const
{
    switch(m_st)
    {
        case TaskStatus::Available: return L"可接";
        case TaskStatus::InProgress: return L"进行中";
        case TaskStatus::Completed: return L"待领奖";
        case TaskStatus::Finished: return L"已完成";
        default: return L"未知";
    }
}
std::wstring Task::getTypeStr() const { return m_type == TaskType::Main ? L"主线" : L"支线"; }
std::wstring Task::fullInfo(int lvl) const
{
    std::wstringstream ss;
    ss << L"【" << getTypeStr() << L"】" << m_name << L" ID:" << std::to_wstring(m_id) << L"\n";
    ss << L"状态:" << getStatusStr() << L"\n描述:" << m_desc << L"\n条件:\n";
    for (auto& c : m_cons) ss << L"-" << c.getText() << L"\n";
    ss << L"奖励:" << m_rwd.getText(lvl);
    if (m_pre != -1) ss << L"\n前置任务ID:" << std::to_wstring(m_pre);
    return ss.str();
}
std::wstring Task::progressStr() const
{
    if (m_st != TaskStatus::InProgress) return L"";
    std::wstringstream ss;
    for (size_t i=0; i<m_cons.size(); i++)
    {
        if(i>0) ss << L",";
        ss << m_cons[i].getText();
    }
    return ss.str();
}
TaskManager::TaskManager(Player* p) : m_p(p) { initAllTask(); }
void TaskManager::initAllTask()
{
    //主线
    m_all.push_back(std::unique_ptr<Task>(new Task(1001,L"消灭骚扰者",L"击杀3只野猫",TaskType::Main,-1,
        std::vector<TaskCondition>{{ConditionType::KillMonster,1,3}},TaskReward(50,20,1,0.5,{std::make_shared<Medicine>(L"体力药水",1,30)}))));
    m_all.push_back(std::unique_ptr<Task>(new Task(1002,L"清理后山",L"5只变异鱼",TaskType::Main,1001,
        std::vector<TaskCondition>{{ConditionType::KillMonster,2,5}},TaskReward(80,50,1.5,1))));
    m_all.push_back(std::unique_ptr<Task>(new Task(1003,L"升级考核",L"等级10",TaskType::Main,1002,
        std::vector<TaskCondition>{{ConditionType::ReachLevel,10,1}},TaskReward(100,80,2,1.5,{std::make_shared<Weapon>(L"新手短剑",1,8)}))));
    m_all.push_back(std::unique_ptr<Task>(new Task(1004,L"湖边巡逻",L"3只大白鹅",TaskType::Main,1003,
        std::vector<TaskCondition>{{ConditionType::KillMonster,3,3}},TaskReward(120,100,2,1.5))));
    m_all.push_back(std::unique_ptr<Task>(new Task(1005,L"保卫教学楼",L"击杀10任意怪",TaskType::Main,1004,
        std::vector<TaskCondition>{{ConditionType::KillMonster,0,10}},TaskReward(200,150,3,2,{std::make_shared<Medicine>(L"高级药水",2,80),std::make_shared<Food>(L"能量面包",3,20,10)}))));
    m_all.push_back(std::unique_ptr<Task>(new Task(1006,L"幕后黑手",L"击杀暗影校长",TaskType::Main,1005,
        std::vector<TaskCondition>{{ConditionType::KillMonster,4,1}},TaskReward(500,300,5,3,{std::make_shared<Weapon>(L"传说之剑",1,35),std::make_shared<Material>(L"暗影水晶",1,Material::LEGENDARY)}))));
    //支线
    m_all.push_back(std::unique_ptr<Task>(new Task(2001,L"食堂打工",L"拥有300金币",TaskType::Side,-1,
        std::vector<TaskCondition>{{ConditionType::HaveGold,300,1}},TaskReward(40,30,0.5,3))));
    m_all.push_back(std::unique_ptr<Task>(new Task(2002,L"悬赏猎手",L"击杀15怪物",TaskType::Side,-1,
        std::vector<TaskCondition>{{ConditionType::KillMonster,0,15}},TaskReward(100,80,2,2))));
    m_all.push_back(std::unique_ptr<Task>(new Task(2003,L"校园富豪",L"1000金币",TaskType::Side,-1,
        std::vector<TaskCondition>{{ConditionType::HaveGold,1000,1}},TaskReward(60,100,0.5,4))));
    for (auto& t : m_all) m_map[t->getId()] = t.get();
}
Task* TaskManager::getById(int id) const
{
    auto it = m_map.find(id);
    return it == m_map.end() ? nullptr : it->second;
}
bool TaskManager::canAccept(int id) const
{
    Task* t = getById(id);
    if (!t || t->getStatus() != TaskStatus::Available) return false;
    int pre = t->getPreTask();
    if (pre == -1) return true;
    Task* pt = getById(pre);
    return pt && pt->getStatus() == TaskStatus::Finished;
}
bool TaskManager::accept(int id)
{
    if (!canAccept(id)) return false;
    Task* t = getById(id);
    t->setStatus(TaskStatus::InProgress);
    return true;
}
void TaskManager::onEvent(ConditionType ty, int tid, int amt)
{
    for (auto& t : m_all)
        if (t->getStatus() == TaskStatus::InProgress)
            t->update(ty, tid, amt);
    checkAll();
}
void TaskManager::checkAll()
{
    for (auto& t : m_all)
    {
        if (t->getStatus() == TaskStatus::InProgress && t->checkComplete(m_p))
            t->setStatus(TaskStatus::Completed);
    }
}
bool TaskManager::claim(int id)
{
    Task* t = getById(id);
    if (!t || t->getStatus() != TaskStatus::Completed) return false;
    int lvl = m_p->getLevel();
    m_p->addExp(t->getReward().calcExp(lvl));
    m_p->addGold(t->getReward().calcGold(lvl));
    Bag* bag = m_p->getBag();
    for (auto& it : t->getReward().items)
        bag->addItem(cloneItem(it.get()));
    t->setStatus(TaskStatus::Finished);
    return true;
}
std::wstring TaskManager::getTaskList() const
{
    std::wstringstream ss;
    ss << L"====主线任务====\n";
    for (auto& t : m_all)
    {
        if (t->getType() == TaskType::Main)
        {
            ss << L"[" << t->getStatusStr() << L"] " << t->getName() << L" ID:" << std::to_wstring(t->getId());
            if (t->getStatus() == TaskStatus::InProgress)
                ss << L" 进度:" << t->progressStr();
            ss << L"\n";
        }
    }
    ss << L"\n====支线任务====\n";
    for (auto& t : m_all)
    {
        if (t->getType() == TaskType::Side)
        {
            ss << L"[" << t->getStatusStr() << L"] " << t->getName() << L" ID:" << std::to_wstring(t->getId());
            if (t->getStatus() == TaskStatus::InProgress)
                ss << L" 进度:" << t->progressStr();
            ss << L"\n";
        }
    }
    return ss.str();
}
std::wstring TaskManager::getDetail(int id) const
{
    Task* t = getById(id);
    if (!t) return L"任务不存在";
    return t->fullInfo(m_p->getLevel());
}
std::wstring TaskManager::getProgressText() const
{
    std::wstringstream ss;
    bool has = false;
    for (auto& t : m_all)
    {
        if (t->getStatus() == TaskStatus::InProgress)
        {
            has = true;
            ss << t->getName() << L":" << t->progressStr() << L"\n";
        }
    }
    if (!has) ss << L"无进行中任务";
    return ss.str();
}
std::string TaskManager::serialize() const
{
    std::stringstream ss;
    bool firstTask = true;
    for (auto& t : m_all)
    {
        if (!firstTask) ss << "|";
        firstTask = false;
        // 任务ID,状态
        ss << t->getId() << "," << (int)t->getStatus();
        // 条件数值，不加末尾逗号
        const auto& conds = t->getConds();
        for (size_t i = 0; i < conds.size(); i++)
        {
            ss << "," << conds[i].curr;
        }
    }
    return ss.str();
}
void TaskManager::deserialize(const std::string& data)
{
    if (data.empty()) return;
    std::stringstream ss(data);
    std::string block;
    while (std::getline(ss, block, '|'))
    {
        if (block.empty()) continue; // 过滤空段
        std::vector<std::string> parts;
        std::stringstream blkSs(block);
        std::string tok;
        while (std::getline(blkSs, tok, ','))
        {
            if (!tok.empty()) parts.push_back(tok); // 过滤空数字串
        }
        if (parts.size() <2) continue;
        int tid = std::stoi(parts[0]);
        Task* t = getById(tid);
        if (!t) continue;
        TaskStatus st = (TaskStatus)std::stoi(parts[1]);
        t->setStatus(st);
        auto& cons = const_cast<std::vector<TaskCondition>&>(t->getConds());
        for (size_t i=0; i<cons.size() && i+2 < parts.size(); i++)
        {
            cons[i].curr = std::stoi(parts[i+2]);
        }
    }
}
std::vector<Task*> TaskManager::getAllTaskPtrs() const
{
    std::vector<Task*> res;
    for (auto& p : m_all)
    {
        res.push_back(p.get());
    }
    return res;
}
