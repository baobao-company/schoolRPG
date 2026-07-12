#include "BattleSystem.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include "Task.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <chrono>

// 静态成员初始化
std::mt19937 Enemy::s_rng(static_cast<unsigned>(std::time(nullptr)));

// Enemy 基类实现
Enemy::Enemy(const std::string& name, EnemyType type, int hp, int attack, int defense,int speed, int expReward, int goldReward)
    : m_name(name), m_type(type), m_hp(hp), m_maxHP(hp),m_attack(attack), m_defense(defense),m_speed(speed),m_expReward(expReward), m_goldReward(goldReward),m_state(EnemyState::ALIVE) {}

std::string Enemy::getName() const { return m_name; }
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
    if (hp < 0) m_hp = 0;
    else if (hp > m_maxHP) m_hp = m_maxHP;
    else m_hp = hp;
}

void Enemy::setState(EnemyState state) { m_state = state; }
void Enemy::setExpReward(int exp) { m_expReward = exp; }
void Enemy::setGoldReward(int gold) { m_goldReward = gold; }

//敌人攻击玩家
int Enemy::attack(Player* player) 
{
    if (!player) return 0;
    //获取玩家总防御力（基础防御 + 装备槽防具）
    int playerDefense = player->getTotalDefense();
    // 伤害 = 攻击 - 防御，最低1
    int damage = m_attack - playerDefense;
    if (damage < 1) damage = 1;
    // 随机波动 ±20%
    std::uniform_int_distribution<int> dist(damage * 80 / 100, damage * 120 / 100);
    damage = dist(s_rng);
    if (damage < 1) damage = 1;
    // 应用伤害
    int actualDamage = player->takeDamage(damage);
    return actualDamage;
}

//敌人受到伤害
int Enemy::takeDamage(int damage) 
{
    int actualDamage = damage - m_defense;
    if (actualDamage < 1) actualDamage = 1;
    m_hp -= actualDamage;
    if (m_hp < 0) m_hp = 0;
    return actualDamage;
}

//显示敌人信息
void Enemy::display() const 
{
    std::cout << "  " << m_name << " | HP: " << m_hp << "/" << m_maxHP<< " | 攻击: " << m_attack << " | 防御: " << m_defense;
}
// 具体敌人类型实现

//史莱姆
Slime::Slime(): Enemy("史莱姆", EnemyType::SLIME, 20, 3, 1, 15, 5,2) {}
void Slime::display() const 
{
    std::cout << "【史莱姆】";
    Enemy::display();
    std::cout << " |  软绵绵的，看起来很好欺负\n";
}
//野猫
WildCat::WildCat(): Enemy("野猫", EnemyType::WILD_CAT, 15, 6, 1, 20, 8,5) {}
void WildCat::display() const 
{
    std::cout << "【野猫】";
    Enemy::display();
    std::cout << " |  敏捷的生物，动作很快\n";
}

//变异鱼
GiantFish::GiantFish(): Enemy("变异鱼", EnemyType::GIANT_FISH, 30, 8, 2, 30, 15,3) {}
void GiantFish::display() const 
{
    std::cout << "【变异鱼】";
    Enemy::display();
    std::cout << " |  被污染的鱼，异常凶猛\n";
}
//大白鹅
AngryGoose::AngryGoose(): Enemy("大白鹅", EnemyType::ANGRY_GOOSE, 45, 12, 3, 50, 25,4) {}

//大白鹅攻击重写
int AngryGoose::attack(Player* player) {

    // 基础攻击
    int damage = Enemy::attack(player);
    // 30%概率暴击，额外伤害
    std::uniform_int_distribution<int> critDist(0, 100);
    if (critDist(s_rng) < 30) 
    {
        int extraDamage = damage * 3 / 2;
        damage += extraDamage;
        std::cout << "   大白鹅暴怒！造成额外伤害 " << extraDamage << "！\n";
    }
    return damage;
}
void AngryGoose::display() const 
{
    std::cout << "【大白鹅】";
    Enemy::display();
    std::cout << " |  校园最凶猛的生物，不可小觑！\n";
}
//暗影校长
ShadowBoss::ShadowBoss(): Enemy("暗影校长", EnemyType::SHADOW_BOSS, 80, 18, 5, 150, 80,6) {}
//Boss攻击重写
int ShadowBoss::attack(Player* player) 
{
    // 基础攻击
    int damage = Enemy::attack(player);
    // 20%概率造成双倍伤害
    std::uniform_int_distribution<int> doubleDist(0, 100);
    if (doubleDist(s_rng) < 20) 
    {
        damage *= 2;
        std::cout << "   暗影校长释放暗影魔法！造成双倍伤害！\n";
    }
    return damage;
}

void ShadowBoss::display() const 
{
    std::cout << "【暗影校长】";
    Enemy::display();
    std::cout << "  校园异变的幕后黑手，极度危险！\n";
}
// BattleSystem 实现
BattleSystem::BattleSystem(Player* player) : m_player(player), m_battleActive(false) {initEnemyPool();}
void BattleSystem::initEnemyPool() 
{
    m_enemyPool.push_back(std::make_shared<Slime>());
    m_enemyPool.push_back(std::make_shared<WildCat>());
    m_enemyPool.push_back(std::make_shared<GiantFish>());
    m_enemyPool.push_back(std::make_shared<AngryGoose>());
    m_enemyPool.push_back(std::make_shared<ShadowBoss>());
}

//创建敌人实例
std::shared_ptr<Enemy> BattleSystem::createEnemy(EnemyType type) 
{
    switch (type) 
    {
        case EnemyType::SLIME:       return std::make_shared<Slime>();
        case EnemyType::WILD_CAT:    return std::make_shared<WildCat>();
        case EnemyType::GIANT_FISH:  return std::make_shared<GiantFish>();
        case EnemyType::ANGRY_GOOSE: return std::make_shared<AngryGoose>();
        case EnemyType::SHADOW_BOSS: return std::make_shared<ShadowBoss>();
        default:                     return std::make_shared<Slime>();
    }
}
// 主菜单
//显示战斗主菜单
void BattleSystem::showMenu() 
{
    while (true) 
    {
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║                     战斗系统                        ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
        // 检查玩家状态
        if (m_player->getHP() <= 0) 
        {
            std::cout << " 你的生命值为0，请先恢复生命值！\n";
            std::cout << "   （使用药品或回城休息）\n";
            std::cout << "\n按回车键返回...";
            std::cin.ignore();              ////////////////////////////////////////
            std::cin.get();                 ////////////////////////////////////////
            return;
        }
        showEnemyList();
        std::cout << "─────────────────────────────────────────────────────────\n";
        std::cout << "请选择要挑战的敌人编号（输入0返回）：";
        int choice;
        std::cin >> choice;                 /////////////////////////////////////////////////////////
        if (choice == 0)
        {
            std::cout << " 离开战斗系统\n";
            return;
        }
        if (choice < 1 || choice > static_cast<int>(m_enemyPool.size())) {
            std::cout << " 无效选择！\n";
            continue;
        }
        // 开始战斗（choice - 1 对应敌人池中的索引）
        startBattle(choice - 1);
    }
}
//显示敌人列表
void BattleSystem::showEnemyList() const 
{
    std::cout << "\n 敌人列表\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << std::left;
    std::cout << "  " << std::setw(4) << "编号"<< std::setw(12) << "名称"<< std::setw(10) << "HP"<< std::setw(10) << "攻击"<< std::setw(10) << "防御"<< "奖励\n";
    std::cout << "  ─────────────────────────────────────────────────────\n";
    for (size_t i = 0; i < m_enemyPool.size(); ++i) 
    {
        const auto& enemy = m_enemyPool[i];
        std::cout << "  " << std::setw(4) << (i + 1)
                  << std::setw(12) << enemy->getName()
                  << std::setw(10) << (std::to_string(enemy->getMaxHP()))
                  << std::setw(10) << enemy->getAttack()
                  << std::setw(10) << enemy->getDefense()
                  << "EXP:" << enemy->getExpReward() 
                  << " 金币:" << enemy->getGoldReward() << "\n";
    }
    std::cout << "═══════════════════════════════════════════════════════\n";
}

// 战斗流程
//开始战斗
void BattleSystem::startBattle(int enemyTypeIndex) 
{
    if (enemyTypeIndex < 0 || enemyTypeIndex >= static_cast<int>(m_enemyPool.size())) 
    {
        std::cout << " 无效的敌人索引！\n";
        return;
    }
    // 创建敌人实例
    EnemyType type = m_enemyPool[enemyTypeIndex]->getType();
    std::shared_ptr<Enemy> enemy = createEnemy(type);
    m_battleActive = true;
    std::cout << "\n战斗开始！\n";
    std::cout << "你遭遇了 ";
    enemy->display();
    std::cout << "\n";

    // 在创建敌人之后，回合循环之前，加上先手判断
    int playerSpeed = m_player->getSpeed();
    int enemySpeed = enemy->getSpeed();
    bool playerFirst = (playerSpeed >= enemySpeed);

    // 显示速度对比
    std::cout << " 速度对比：玩家 " << playerSpeed << " vs " << enemy->getName() << " " << enemySpeed;
    if (playerFirst) 
    {
    std::cout << " → 你先行动！" << std::endl;
    } 
    else 
    {
    std::cout << " → 敌人先行动！" << std::endl;
    }

    // 回合循环按先手顺序执行
    while (m_battleActive) 
    {
        displayBattleStatus(enemy.get());
    
        if (playerFirst) 
        {
            // 玩家回合
            playerTurn(enemy.get());

            // 检查玩家是否胜利
            if (!enemy->isAlive()) 
            {
                std::cout << "\n 你击败了 " << enemy->getName() << "！\n";
                grantRewards(enemy.get());
                break;
            }

            // 敌人回合
            enemyTurn(enemy.get());

            // 检查玩家是否死亡
            if (m_player->getHP() <= 0) 
            {
                std::cout << "\n 你被 " << enemy->getName() << " 击败了！\n";
                break;
            }
            displayBattleStatus(enemy.get());
        } 
        else 
        {
            // 敌人回合
            enemyTurn(enemy.get());

            // 检查玩家是否死亡
            if (m_player->getHP() <= 0) 
            {
                std::cout << "\n 你被 " << enemy->getName() << " 击败了！\n";
                break;
            }

            // 玩家回合
            playerTurn(enemy.get());

            // 检查玩家是否胜利
            if (!enemy->isAlive()) 
            {
                std::cout << "\n 你击败了 " << enemy->getName() << "！\n";
                grantRewards(enemy.get());
                break;
            }
            displayBattleStatus(enemy.get());
        }
    }
    m_battleActive = false;
}
//玩家回合
void BattleSystem::playerTurn(Enemy* enemy) 
{
    while (true) 
    {
        std::cout << "\n【你的回合】\n";
        std::cout << "  1. 攻击\n";
        std::cout << "  2. 使用物品\n";
        std::cout << "请选择：";
        int choice;
        std::cin >> choice;                     //////////////////////////////////////

        if (choice == 1) 
        {
            int playerAttack = m_player->getTotalAttack(); 
            int damage = enemy->takeDamage(playerAttack);
            break;
        } 
        else if (choice == 2) 
        {
            Bag* bag = m_player->getBag();
            if (!bag || bag->getItemCount() == 0) 
            {
                std::cout << " 背包中没有可用物品！" << std::endl;
                continue;
            }
            // 只显示药品（战斗中只能用药）
            std::cout << "\n【战斗中可用的药品】" << std::endl;
            std::vector<Item*> medicines = bag->getItemsByType(Item::MEDICINE);
            if (medicines.empty()) 
            {
                std::cout << " 没有药品可用！" << std::endl;
                continue;
            }
            for (auto* item : medicines)  
            {
                item->display();
            }
            std::cout << "请输入要使用的药品名称（输入0取消）：";
            std::string itemName;
            std::cin >> itemName;                   ///////////////////////////////////
            if (itemName == "0") continue;
            bag->useItem(itemName, m_player);
            break;
        }
        else 
        {
            std::cout << " 无效选择！\n";
        }
    }
}
//敌人回合
void BattleSystem::enemyTurn(Enemy* enemy) 
{
    std::cout << "\n【敌人回合】\n";
    int damage = enemy->attack(m_player);
}

// 奖励发放
void BattleSystem::grantRewards(Enemy* enemy) 
{
    if (!enemy) return;
    // 获取奖励值
    int exp = enemy->getExpReward();
    int gold = enemy->getGoldReward();
    // 发放经验
    m_player->addExp(exp);
    // 发放金币
    m_player->addGold(gold);
    // 掉落物品
    dropItems(enemy);
    // 通知任务系统：击杀怪物事件
    TaskManager* taskMgr = m_player->getTaskManager();
    if (taskMgr) 
    {
        int monsterId = static_cast<int>(enemy->getType()) + 1;
        taskMgr->onEvent(ConditionType::KillMonster, monsterId, 1);
    }
}

//掉落物品
void BattleSystem::dropItems(Enemy* enemy) 
{
    Bag* bag = m_player->getBag();
    if (!bag) return;

    std::vector<std::shared_ptr<Item>> droppedItems;

    switch (enemy->getType()) 
    {
        case EnemyType::SLIME: 
        {
            std::uniform_int_distribution<int> dist(1, 2);
            int count = dist(Enemy::s_rng);
            droppedItems.push_back(std::make_shared<Material>("普通材料", count, Material::COMMON));
            break;
        }
        case EnemyType::WILD_CAT: 
        {
            droppedItems.push_back(std::make_shared<Material>("普通材料", 1, Material::COMMON));
            break;
        }
        case EnemyType::GIANT_FISH: 
        {
            droppedItems.push_back(std::make_shared<Material>("普通材料", 2, Material::COMMON));
            std::uniform_int_distribution<int> dist(0, 100);
            if (dist(Enemy::s_rng) < 30) 
            {
                droppedItems.push_back(std::make_shared<Material>("稀有材料", 1, Material::UNCOMMON));
            }
            break;
        }
        case EnemyType::ANGRY_GOOSE: 
        {
            std::uniform_int_distribution<int> dist(1, 2);
            int count = dist(Enemy::s_rng);
            droppedItems.push_back(std::make_shared<Material>("稀有材料", count, Material::UNCOMMON));
            
            std::uniform_int_distribution<int> rareDist(0, 100);
            if (rareDist(Enemy::s_rng) < 20) 
            {
                droppedItems.push_back(std::make_shared<Material>("罕见材料", 1, Material::RARE));
            }
            break;
        }
        case EnemyType::SHADOW_BOSS: 
        {
            droppedItems.push_back(std::make_shared<Material>("史诗材料", 2, Material::EPIC));
            
            std::uniform_int_distribution<int> legendaryDist(0, 100);
            if (legendaryDist(Enemy::s_rng) < 50) 
            {
                droppedItems.push_back(std::make_shared<Material>("传说材料", 1, Material::LEGENDARY));
            }
            break;
        }
        default:
            break;
    }
    if (!droppedItems.empty()) 
    {
        std::cout << "   掉落物品：\n";
        for (const auto& item : droppedItems) 
        {
            Item* itemCopy = cloneItem(item.get());
            if (itemCopy) 
            {
                bag->addItem(itemCopy);
                std::cout << "    - " << item->getName() << " ×" << item->getQuantity() << "\n";
            }
        }
    }
}
// 辅助方法
//显示战斗状态
void BattleSystem::displayBattleStatus(Enemy* enemy) 
{
    std::cout << "\n【战斗状态】\n";
    std::cout << "   你的HP: " << m_player->getHP() << "/" << m_player->getMaxHP() << "\n";
    std::cout << enemy->getName() << " HP: " << enemy->getHP() << "/" << enemy->getMaxHP() << "\n";
}
