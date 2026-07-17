#ifndef BATTLE_H
#define BATTLE_H
#include <string>
#include <vector>
#include <memory>
#include <random>
class Player;
class Item;
class Weapon;
class Armor;
class Material;
enum class EnemyType
{
    SLIME, WILD_CAT, GIANT_FISH, ANGRY_GOOSE, SHADOW_BOSS
};
enum class EnemyState
{
    ALIVE, DEAD, FLEEING
};
class Enemy
{
public:
    static std::mt19937 s_rng;
    Enemy(const std::wstring& name, EnemyType type, int hp, int atk, int def, int spd, int exp, int gold);
    virtual ~Enemy() = default;
    std::wstring getName() const;
    EnemyType getType() const;
    int getHP() const;
    int getMaxHP() const;
    int getAttack() const;
    int getDefense() const;
    int getSpeed() const;
    int getExpReward() const;
    int getGoldReward() const;
    EnemyState getState() const;
    bool isAlive() const;
    void setHP(int hp);
    void setState(EnemyState st);
    void setExpReward(int e);
    void setGoldReward(int g);
    virtual int attack(Player* player);
    int takeDamage(int dmg);
    virtual std::wstring getDescText() const;
protected:
    std::wstring m_name;
    EnemyType m_type;
    int m_hp, m_maxHP;
    int m_attack, m_defense, m_speed;
    int m_expReward, m_goldReward;
    EnemyState m_state;
};
class Slime : public Enemy
{
public:
    Slime();
    std::wstring getDescText() const override;
};
class WildCat : public Enemy
{
public:
    WildCat();
    std::wstring getDescText() const override;
};
class GiantFish : public Enemy
{
public:
    GiantFish();
    std::wstring getDescText() const override;
};
class AngryGoose : public Enemy
{
public:
    AngryGoose();
    int attack(Player* player) override;
    std::wstring getDescText() const override;
};
class ShadowBoss : public Enemy
{
public:
    ShadowBoss();
    int attack(Player* player) override;
    std::wstring getDescText() const override;
};
class BattleSystem
{
public:
    BattleSystem(Player* p);
    ~BattleSystem() = default;
    void showMenu();
    void startBattle(int idx);
    void playerTurn(Enemy* e, std::wstring& logOut);
    void enemyTurn(Enemy* e, std::wstring& logOut);
    void showEnemyList(std::wstring& out) const;
    std::shared_ptr<Enemy> createEnemy(EnemyType t);
    void grantRewards(Enemy* e, std::wstring& log);
private:
    Player* m_player;
    std::vector<std::shared_ptr<Enemy>> m_pool;
    bool m_active;
    void initPool();
    void showStatus(Player* p, Enemy* e, std::wstring& out);
    void dropItems(Enemy* e, std::wstring& log);
};
#endif
