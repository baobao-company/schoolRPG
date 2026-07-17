#include "BattleSystem.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include "Task.h"
#include <sstream>
#include <algorithm>
#include <ctime>
std::mt19937 Enemy::s_rng(static_cast<unsigned>(std::time(nullptr)));
Enemy::Enemy(const std::wstring& name, EnemyType type, int hp, int atk, int def, int spd, int exp, int gold)
    : m_name(name), m_type(type), m_hp(hp), m_maxHP(hp), m_attack(atk), m_defense(def), m_speed(spd), m_expReward(exp), m_goldReward(gold), m_state(EnemyState::ALIVE)
{}
std::wstring Enemy::getName() const { return m_name; }
EnemyType Enemy::getType() const { return m_type; }
int Enemy::getHP() const { return m_hp; }
int Enemy::getMaxHP() const { return m_maxHP; }
int Enemy::getAttack() const { return m_attack; }
int Enemy::getDefense() const { return m_defense; }
int Enemy::getSpeed() const { return m_speed; }
int Enemy::getExpReward() const { return m_expReward; }
int Enemy::getGoldReward() const { return m_goldReward; }
EnemyState Enemy::getState() const { return m_state; }
bool Enemy::isAlive() const { return m_state == EnemyState::ALIVE && m_hp > 0; }
void Enemy::setHP(int hp)
{
    m_hp = hp < 0 ? 0 : (hp > m_maxHP ? m_maxHP : hp);
}
void Enemy::setState(EnemyState st) { m_state = st; }
void Enemy::setExpReward(int e) { m_expReward = e; }
void Enemy::setGoldReward(int g) { m_goldReward = g; }
int Enemy::attack(Player* player)
{
    int def = player->getTotalDef();
    int dmg = m_attack - def;
    if (dmg < 1) dmg = 1;
    std::uniform_int_distribution<int> dist(dmg * 80 / 100, dmg * 120 / 100);
    dmg = dist(s_rng);
    if (dmg < 1) dmg = 1;
    return player->takeDmg(dmg);
}
int Enemy::takeDamage(int dmg)
{
    int real = dmg - m_defense;
    if (real < 1) real = 1;
    m_hp -= real;
    if (m_hp < 0) m_hp = 0;
    return real;
}
std::wstring Enemy::getDescText() const
{
    std::wstringstream ss;
    ss << m_name << L" HP:" << std::to_wstring(m_hp) << L"/" << std::to_wstring(m_maxHP) << L" ATK:" << std::to_wstring(m_attack) << L" DEF:" << std::to_wstring(m_defense);
    return ss.str();
}
Slime::Slime() : Enemy(L"史莱姆", EnemyType::SLIME, 20, 3, 1, 15, 5, 2) {}
std::wstring Slime::getDescText() const { return L"【史莱姆】软绵绵弱小生物 | " + Enemy::getDescText(); }
WildCat::WildCat() : Enemy(L"野猫", EnemyType::WILD_CAT, 15, 6, 1, 20, 8, 5) {}
std::wstring WildCat::getDescText() const { return L"【野猫】敏捷高速 | " + Enemy::getDescText(); }
GiantFish::GiantFish() : Enemy(L"变异鱼", EnemyType::GIANT_FISH, 30, 8, 2, 30, 15, 3) {}
std::wstring GiantFish::getDescText() const { return L"【变异鱼】水中怪物 | " + Enemy::getDescText(); }
AngryGoose::AngryGoose() : Enemy(L"大白鹅", EnemyType::ANGRY_GOOSE, 45, 12, 3, 50, 25, 4) {}
int AngryGoose::attack(Player* player)
{
    int base = Enemy::attack(player);
    std::uniform_int_distribution<int> r(0, 100);
    if (r(s_rng) < 30)
        base += base * 3 / 2;
    return base;
}
std::wstring AngryGoose::getDescText() const { return L"【大白鹅】校园霸主 | " + Enemy::getDescText(); }
ShadowBoss::ShadowBoss() : Enemy(L"暗影校长", EnemyType::SHADOW_BOSS, 80, 18, 5, 150, 80, 6) {}
int ShadowBoss::attack(Player* player)
{
    int dmg = Enemy::attack(player);
    std::uniform_int_distribution<int> r(0, 100);
    if (r(s_rng) < 20) dmg *= 2;
    return dmg;
}
std::wstring ShadowBoss::getDescText() const { return L"【暗影校长】最终BOSS | " + Enemy::getDescText(); }
BattleSystem::BattleSystem(Player* p) : m_player(p), m_active(false) { initPool(); }
void BattleSystem::initPool()
{
    m_pool.push_back(std::make_shared<Slime>());
    m_pool.push_back(std::make_shared<WildCat>());
    m_pool.push_back(std::make_shared<GiantFish>());
    m_pool.push_back(std::make_shared<AngryGoose>());
    m_pool.push_back(std::make_shared<ShadowBoss>());
}
std::shared_ptr<Enemy> BattleSystem::createEnemy(EnemyType t)
{
    switch (t)
    {
        case EnemyType::SLIME: return std::make_shared<Slime>();
        case EnemyType::WILD_CAT: return std::make_shared<WildCat>();
        case EnemyType::GIANT_FISH: return std::make_shared<GiantFish>();
        case EnemyType::ANGRY_GOOSE: return std::make_shared<AngryGoose>();
        case EnemyType::SHADOW_BOSS: return std::make_shared<ShadowBoss>();
        default: return std::make_shared<Slime>();
    }
}
void BattleSystem::showEnemyList(std::wstring& out) const
{
    std::wstringstream ss;
    ss << L"====敌人列表====\n";
    for (size_t i = 0; i < m_pool.size(); i++)
    {
        auto& e = m_pool[i];
        ss << std::to_wstring(i+1) << L"." << e->getName() << L" HP" << std::to_wstring(e->getMaxHP())
        << L" ATK" << std::to_wstring(e->getAttack()) << L" EXP" << std::to_wstring(e->getExpReward()) << L" G" << std::to_wstring(e->getGoldReward()) << L"\n";
    }
    out = ss.str();
}
void BattleSystem::showStatus(Player* p, Enemy* e, std::wstring& out)
{
    std::wstringstream ss;
    ss << L"【战斗状态】\n你的HP:" << std::to_wstring(p->getHP()) << L"/" << std::to_wstring(p->getMaxHP())
    << L"\n" << e->getName() << L" HP:" << std::to_wstring(e->getHP()) << L"/" << std::to_wstring(e->getMaxHP());
    out = ss.str();
}
void BattleSystem::startBattle(int idx)
{
    if (idx < 0 || idx >= (int)m_pool.size()) return;
    auto enemy = createEnemy(m_pool[idx]->getType());
    m_active = true;
    std::wstring log;
    log += L"遭遇" + enemy->getName() + L"！\n";
    int pSpd = m_player->getSpeed();
    int eSpd = enemy->getSpeed();
    bool playerFirst = pSpd >= eSpd;
    log += L"速度对比 玩家" + std::to_wstring(pSpd) + L" vs 敌人" + std::to_wstring(eSpd);
    log += playerFirst ? L" 你先手\n" : L" 敌人先手\n";
    while (m_active)
    {
        std::wstring statText;
        showStatus(m_player, enemy.get(), statText);
        log += statText + L"\n";
        if (playerFirst)
        {
            std::wstring pLog;
            playerTurn(enemy.get(), pLog);
            log += pLog;
            if (!enemy->isAlive())
            {
                log += L"击败" + enemy->getName() + L"！\n";
                grantRewards(enemy.get(), log);
                break;
            }
            std::wstring eLog;
            enemyTurn(enemy.get(), eLog);
            log += eLog;
            if (m_player->getHP() <= 0)
            {
                log += L"你被击败！";
                break;
            }
        }
        else
        {
            std::wstring eLog;
            enemyTurn(enemy.get(), eLog);
            log += eLog;
            if (m_player->getHP() <= 0)
            {
                log += L"你被击败！";
                break;
            }
            std::wstring pLog;
            playerTurn(enemy.get(), pLog);
            log += pLog;
            if (!enemy->isAlive())
            {
                log += L"击败" + enemy->getName() + L"！\n";
                grantRewards(enemy.get(), log);
                break;
            }
        }
    }
    m_active = false;
}
void BattleSystem::playerTurn(Enemy* e, std::wstring& logOut)
{
    std::wstringstream ss;
    int atkDmg = m_player->getTotalAtk();
    int real = e->takeDamage(atkDmg);
    ss << L"你发起攻击，造成" << std::to_wstring(real) << L"伤害\n";
    logOut = ss.str();
}
void BattleSystem::enemyTurn(Enemy* e, std::wstring& logOut)
{
    std::wstringstream ss;
    int dmg = e->attack(m_player);
    ss << e->getName() << L"攻击你，造成" << std::to_wstring(dmg) << L"伤害\n";
    logOut = ss.str();
}
void BattleSystem::grantRewards(Enemy* e, std::wstring& log)
{
    int exp = e->getExpReward();
    int gold = e->getGoldReward();
    m_player->addExp(exp);
    m_player->addGold(gold);
    std::wstringstream ss;
    ss << L"获得EXP:" << std::to_wstring(exp) << L" 金币:" << std::to_wstring(gold) << L"\n";
    std::wstring dropLog;
    dropItems(e, dropLog);
    ss << dropLog;
    log += ss.str();
    TaskManager* tm = m_player->getTaskManager();
    tm->onEvent(ConditionType::KillMonster, (int)e->getType() + 1, 1);
}
void BattleSystem::dropItems(Enemy* e, std::wstring& log)
{
    std::wstringstream ss;
    std::vector<std::shared_ptr<Item>> drops;
    switch (e->getType())
    {
        case EnemyType::SLIME:
        {
            std::uniform_int_distribution<int> r(1, 2);
            drops.emplace_back(std::make_shared<Material>(L"普通材料", r(Enemy::s_rng), Material::COMMON));
            break;
        }
        case EnemyType::WILD_CAT:
            drops.emplace_back(std::make_shared<Material>(L"普通材料", 1, Material::COMMON));
            break;
        case EnemyType::GIANT_FISH:
        {
            std::uniform_int_distribution<int> r(1, 2);
            drops.emplace_back(std::make_shared<Material>(L"普通材料", r(Enemy::s_rng), Material::COMMON));
            std::uniform_int_distribution<int> r2(0, 100);
            if (r2(Enemy::s_rng) < 30)
                drops.emplace_back(std::make_shared<Material>(L"稀有材料", 1, Material::UNCOMMON));
            break;
        }
        case EnemyType::ANGRY_GOOSE:
        {
            std::uniform_int_distribution<int> r(1, 2);
            drops.emplace_back(std::make_shared<Material>(L"稀有材料", r(Enemy::s_rng), Material::UNCOMMON));
            std::uniform_int_distribution<int> r2(0, 100);
            if (r2(Enemy::s_rng) < 20)
                drops.emplace_back(std::make_shared<Material>(L"罕见材料", 1, Material::RARE));
            break;
        }
        case EnemyType::SHADOW_BOSS:
        {
            drops.emplace_back(std::make_shared<Material>(L"史诗材料", 2, Material::EPIC));
            std::uniform_int_distribution<int> r(0, 100);
            if (r(Enemy::s_rng) < 50)
                drops.emplace_back(std::make_shared<Material>(L"传说材料", 1, Material::LEGENDARY));
            break;
        }
        default: break;
    }
    if (!drops.empty())
    {
        ss << L"掉落：\n";
        for (auto& it : drops)
        {
            Item* c = cloneItem(it.get());
            Bag* bag = m_player->getBag();
            bag->addItem(c);
            ss << it->getName() << L" ×" << std::to_wstring(it->getQuantity()) << L"\n";
        }
    }
    log = ss.str();
}
void BattleSystem::showMenu() {}
