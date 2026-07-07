// BattleSystem.cpp
#include "BattleSystem.h"
#include "character.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

//判断敌人是否存活
bool Enemy::isAlive() const
{
    return Hp>0;
}
//受到伤害
void Enemy::takeDamage(int damage)
{
    Hp-=damage;
    if(Hp<0) Hp=0;
}
//显示敌人信息
void Enemy::display() const
{
    std::cout<<"【" << name << "】";                                     /**/
    std::cout<<" 类型:" << GetType();                                    /**/
    std::cout<<" 等级:" << level;                                        /**/
    std::cout<<" HP:" << Hp << "/" << MaxHp;                             /**/
    std::cout<<" 攻击力:"<<attack;                                          /**/
    std::cout<<" 防御力:"<<Defence;                                     /**/
    std::cout<<" 速度:"<<speed;                                          /**/
    std::cout<<" 状态:"<<status;                                            /**/
    std::cout << std::endl;                                                /**/
}
//普通怪构造
Normal::Normal(int playerLevel)
    : Enemy("普通怪物", playerLevel,
            30+playerLevel*10,      // MaxHp
            30+playerLevel*10,      // Hp
            5+playerLevel*3,        // attack
            2+playerLevel*2,        // Defence
            5+playerLevel,          // speed
            "正常") {}
//普通怪获得类型
std::string Normal::GetType() const
{
    return "普通怪";
}
//普通伤害
int Normal::Attack(Character& target)
{
    int damage = GetAttack() - target.GetDefence() / 2;
    damage = std::max(1, damage);
    return damage;
}
//Elite实现 
Elite::Elite(int playerLevel)
    : Enemy("精英怪物", playerLevel + 1,
            50 + playerLevel * 15,
            50 + playerLevel * 15,
            10 + playerLevel * 5,
            4 + playerLevel * 3,
            8 + playerLevel * 2,
            "正常") {}

std::string Elite::GetType() const
{
    return "精英";
}

int Elite::Attack(Character& target)
{
    int damage = GetAttack() - target.GetDefence() / 2 + 3;
    damage = std::max(1, damage);
    return damage;
}

// ============ Boss 实现 ============
Boss::Boss(int playerLevel)
    : Enemy("BOSS", playerLevel + 3,
            100 + playerLevel * 25,
            100 + playerLevel * 25,
            15 + playerLevel * 8,
            6 + playerLevel * 4,
            6 + playerLevel * 2,
            "正常") {}

std::string Boss::GetType() const
{
    return "BOSS";
}

int Boss::Attack(Character& target)
{
    int damage=GetAttack()-target.GetDefence()/2+5;
    damage=std::max(1, damage);
    return damage;
}

int Boss::SpecialAttack(Character& target)
{
    int damage=GetAttack()- target.GetPDefence() / 3;
    damage = std::max(1, damage);
    return damage;
}
//BattleSystem实现
BattleSystem::BattleSystem(Character* p)
    : player(p), currentEnemy(nullptr), isbattleActive(false)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}
BattleSystem::~BattleSystem()
{
    delete currentEnemy;
    currentEnemy = nullptr;
}
//三个公开接口
void BattleSystem::startNormalBattle()
{
    startBattle(new Normal(player->GetLevel()));
}

void BattleSystem::startEliteBattle()
{
    startBattle(new Elite(player->GetLevel()));
}

void BattleSystem::startBossBattle()
{
    startBattle(new Boss(player->GetLevel()));
}

void BattleSystem::playerTurn()
{
    std::cout << "\n--- 你的回合 ---\n";                            /**/
    std::cout << "1. 攻击  2. 逃跑  3.使用道具\n";                   /**/
    std::cout << "请选择: ";                                        /**/

    int choice;
    std::cin >> choice;                                            /**/

    if (choice == 1)
    {
        int damage = player->GetAttack()-currentEnemy->GetDefence()/2;
        damage = std::max(1, damage);
        currentEnemy->takeDamage(damage);
    }
    else if (choice == 2)
    {
        int success = rand() % 100;
        if (success < 60)
        {
            std::cout << "成功逃跑！\n";
            isbattleActive = false;
            delete currentEnemy;
            currentEnemy = nullptr;
        }
        else
        {
            std::cout << "逃跑失败！\n";
        }
    }
    /*else if(choice==3)
    {
        std::cout<<"选择使用的道具"<<std::endl;
    }*/
    else
    {
        std::cout << "无效输入，默认攻击！\n";
        int damage = player->GetAttack() - currentEnemy->GetDefence()/2;
        damage = std::max(1, damage);
        currentEnemy->takeDamage(damage);
        std::cout << "你攻击了 " << currentEnemy->GetName()
                  << "，造成 " << damage << " 点伤害！\n";
    }
}

void BattleSystem::enemyTurn()
{
    if (!currentEnemy || !currentEnemy->isAlive()) return;
    std::cout << "\n--- 敌人回合 ---\n";
    int damage = currentEnemy->Attack(*player);
    player->takeDamage(damage);
    std::cout << currentEnemy->GetName() << " 反击，造成 " << damage << " 点伤害！\n";
}

void BattleSystem::processReward()
{
    if (!currentEnemy) return;

    int exp = currentEnemy->Getlevel() * 15;
    int gold = currentEnemy->Getlevel() * 10;

    player->AddExp(exp);
    player->AddGold(gold);

    std::cout << "\n战斗胜利！\n";
    std::cout << " 获得 " << exp << " 经验值\n";
    std::cout << " 获得 " << gold << " 金币\n";
    player->GetExp(exp);
    isbattleActive = false;
    delete currentEnemy;
    currentEnemy = nullptr;
}

void BattleSystem::displayStatus() const
{
    if (!currentEnemy) return;

    std::cout << "\n===== 战斗状态 =====\n";
    std::cout << " 玩家: " << player->GetName()
              << "  HP: " << player->GetHp() << "/" << player->GetMaxHp() << "\n";
    std::cout << " 敌人: " << currentEnemy->GetName()
              << "  HP: " << currentEnemy->GetHp() << "/" << currentEnemy->GetMaxHp() << "\n";
}
//核心战斗逻辑
void BattleSystem::startBattle(Enemy* enemy)
{
    // 清理之前的敌人
    delete currentEnemy;
    currentEnemy = nullptr;

    currentEnemy = enemy;
    isbattleActive = true;
    std::cout << "\n 战斗开始！\n";
    std::cout << "你遇到了 ";
    currentEnemy->display();
    displayStatus();

    // 战斗循环
    while (isbattleActive && player->isAlive() && currentEnemy->isAlive())
    {
        playerTurn();
        if (!currentEnemy->isAlive())
        {
            processReward();
            break;
        }
        enemyTurn();
        if (!player->isAlive())
        {
            std::cout << " 你被击败了！\n";
            isbattleActive = false;
            break;
        }
        displayStatus();
    }
}