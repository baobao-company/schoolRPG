//战斗模块
#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include <memory>
#include "character.h"
#include "backpack.h"
#include <iostream>
#include <string>

//**敌人基类
class Enemy
{
    private:
    std::string name;                       //名字
    int level;                              //等级
    int MaxHp;                              //最大血量
    int Hp;                                 //当前血量
    int attack;                             //攻击力
    int Defence;                           //防御力
    int speed;                              //速度
    std::string status;                     //状态
    public:
//敌人基类构造
    Enemy(std::string n,int l,int mh,int h,int ak,int df,int s,std::string st)
    {
        name=n;
        level=l;
        MaxHp=mh;
        Hp=h;
        attack=ak;
        Defence=df;
        speed=s;
        status=st;
    }
//基类析构
    virtual ~Enemy() {}
//get函数
    std::string GetName() const {return name;}
    int Getlevel() const {return level;}
    int GetMaxHp() const {return MaxHp;}
    int GetHp() const {return Hp;}
    int GetAttack() const {return attack;}
    int GetDefence() const {return Defence;}
    int GetSpeed() const {return speed;}
    std::string GetStatus() const {return status;}

    void SetHp(int hp) { Hp = hp; }
    void SetStatus(const std::string& st) { status = st; }

    virtual std::string GetType()const=0;                 //显示敌人类型
    bool isAlive() const;                                 //判断敌人是否存活
    virtual int Attack(Character& target)=0;              //攻击人物
    void takeDamage(int damage);                          //收到伤害
    void display() const;                                 //显示敌人信息
};
//三种敌人
//普通
class Normal:public Enemy
{
    Normal(int playerLevel);
    std::string GetType() const override;
    int Attack(Character& target) override;
};
//精英
class Elite : public Enemy
{
public:
    Elite(int playerLevel);
    std::string GetType() const override;
    int Attack(Character& target) override;
};
//BOSS
class Boss : public Enemy
{
public:
    Boss(int playerLevel);
    std::string GetType() const override;
    int Attack(Character& target) override;
    int SpecialAttack(Character& target);                  // Boss专属技能
};
//**战斗系统
class BattleSystem
{
    private:
    Character* player;
    Enemy* currentEnemy;
    bool isbattleActive;

    // 私有辅助函数
    void startBattle(Enemy* enemy);   // 核心战斗逻辑（三个公开函数共用）
    void playerTurn();                // 玩家回合
    void enemyTurn();                 // 敌人回合
    void processReward();             // 结算奖励
    void displayStatus() const;       // 显示战斗状态

    public:
    // 构造函数 & 析构函数
    BattleSystem(Character* p);
    ~BattleSystem();

    // 三个公开接口
    void startNormalBattle();
    void startEliteBattle();
    void startBossBattle();

    // 辅助,判断战斗是否在进行
    bool isBattleActive() const { return isbattleActive; }
};

#endif