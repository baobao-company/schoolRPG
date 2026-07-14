#include "Player.h"
#include "Task.h"        // TaskManager 的实现需要
#include "Bag.h"            
#include "Shop.h"        
#include "ForgeManager.h"   
#include <iostream>
#include <sstream>
#include <iomanip>

Player::Player(const std::string& name)
    : m_name(name),
      m_level(1),           // 初始等级为1
      m_hp(50),             // 初始生命值50
      m_maxHP(50),          // 初始最大生命值50
      m_baseAttack(5),      // 初始攻击
      m_baseDefense(2),     // 初始防御
      m_speed(3),           // 初始速度
      m_exp(0),             // 初始经验值0
      m_gold(100),          // 初始金币100
      m_bag(nullptr),
      m_shop(nullptr),
      m_taskManager(nullptr),
      m_forgeManager(nullptr),
      m_equippedWeapon(nullptr),
      m_equippedArmor(nullptr)

    {
    // 创建子系统
    m_bag = new Bag(name + "的背包", 20);
    m_shop = new Shop(this);  // 创建商店
    // 任务管理器需要 Player 指针，所以传 this
    m_taskManager = new TaskManager(this);
    // 锻造工坊需要 Player 指针，所以传 this
    m_forgeManager = new ForgeManager(this);
    std::cout << " 欢迎 " << m_name << " 来到校园RPG世界！" << std::endl;
    }
Player::~Player() 
{
    delete m_bag;
    delete m_shop;
    delete m_taskManager;
    delete m_forgeManager;
    std::cout << m_name << " 已离开游戏。" << std::endl;
}
//装备武器
bool Player::equipWeapon(Weapon* weapon) 
{
    if (weapon == nullptr) return false;
    // 如果已有武器，先卸下（放回背包）
    if (m_equippedWeapon != nullptr) 
    {
        unequipWeapon();
    }
    // 从背包中移除武器（注意：需要根据指针移除，不是根据名称）
    if (m_bag->removeItem(weapon)) 
    {
        m_equippedWeapon = weapon;
        std::cout << "装备了 " << weapon->getName() << std::endl;
        return true;
    }

    std::cout << " 武器不在背包中，无法装备" << std::endl;
    return false;
}
//装备防具
bool Player::equipArmor(Armor* armor) 
{
    if (armor == nullptr) 
    {
        std::cout << " 防具指针为空！" << std::endl;
        return false;
    }
    // 如果已有防具，先卸下（放回背包）
    if (m_equippedArmor != nullptr) {

        unequipArmor();
    }
    // 从背包中移除防具（但不删除对象）
    if (m_bag->removeItem(armor, false)) 
    {
        m_equippedArmor = armor;
        std::cout << "装备了 " << armor->getName() << std::endl;
        return true;
    }

    std::cout << "防具不在背包中，无法装备" << std::endl;
    return false;
}
//卸下武器
void Player::unequipWeapon() 
{
    if (m_equippedWeapon == nullptr) 
    {
        std::cout << "当前没有装备武器" << std::endl;
        return;
    }
    // 放回背包
    if (m_bag->addItem(m_equippedWeapon)) 
    {
        std::cout << "卸下了 " << m_equippedWeapon->getName() << std::endl;
        m_equippedWeapon = nullptr;
    } 
    else 
    {
        std::cout << " 背包已满，无法卸下！" << std::endl;
    }
}
//卸下防具
void Player::unequipArmor() 
{
    if (m_equippedArmor == nullptr) 
    {
        std::cout << " 当前没有装备防具" << std::endl;
        return;
    }
    // 放回背包
    if (m_bag->addItem(m_equippedArmor)) 
    {
        std::cout << " 卸下了 " << m_equippedArmor->getName() << std::endl;
        m_equippedArmor = nullptr;
    } 
    else 
    {
        std::cout << " 背包已满，无法卸下防具！" << std::endl;
    }
}
// Getter 实现
std::string Player::getName() const 
{
    return m_name;
}
int Player::getLevel() const 
{
    return m_level;
}
int Player::getHP() const 
{
    return m_hp;
}
int Player::getMaxHP() const 
{
    return m_maxHP;
}
int Player::getBaseAttack() const 
{
    return m_baseAttack;
}
int Player::getBaseDefense() const 
{
    return m_baseDefense;
}
int Player::getSpeed() const 
{
    return m_speed;
}
int Player::getTotalAttack() const 
{
    int weaponDamage = 0;
    if (m_equippedWeapon != nullptr) 
    {
        weaponDamage = m_equippedWeapon->getDamage();
    }
    return m_baseAttack + weaponDamage;
}
int Player::getTotalDefense() const 
{
    int armorDefense = 0;
    if (m_equippedArmor != nullptr)
    {
        armorDefense = m_equippedArmor->getDefense();
    }
    return m_baseDefense + armorDefense;
}
int Player::getExp() const 
{
    return m_exp;
}

//获取升级所需经验值
int Player::getExpToNextLevel() const 
{
    return getExpRequired(m_level);
}
//获取金币数
int Player::getGold() const 
{
    return m_gold;
}

// Setter 实现
void Player::setName(const std::string& name) 
{
    m_name = name;
}

void Player::setLevel(int level) 
{
    if (level < 1) level = 1;
    if (level > 100) level = 100;
    m_level = level;
}

//设置生命值
void Player::setHP(int hp) 
{
    if (hp < 0) hp = 0;
    if (hp > m_maxHP) hp = m_maxHP;
    m_hp = hp;
}

void Player::setMaxHP(int maxHP) 
{
    if (maxHP < 1) maxHP = 1;
    m_maxHP = maxHP;
    // 如果当前生命值超过新的最大值，自动调整
    if (m_hp > m_maxHP) 
    {
        m_hp = m_maxHP;
    }
}
void Player::setBaseAttack(int val) 
{
    if (val < 0) val = 0;
    m_baseAttack = val;
}

void Player::setBaseDefense(int val) 
{
    if (val < 0) val = 0;
    m_baseDefense = val;
}

void Player::setSpeed(int val) 
{
    if (val < 0) val = 0;
    m_speed = val;
}

void Player::addBaseAttack(int val) 
{
    m_baseAttack += val;
    if (m_baseAttack < 0) m_baseAttack = 0;
}

void Player::addBaseDefense(int val) 
{
    m_baseDefense += val;
    if (m_baseDefense < 0) m_baseDefense = 0;
}

void Player::addSpeed(int val) 
{
    m_speed += val;
    if (m_speed < 0) m_speed = 0;
}
void Player::setExp(int exp) 
{
    if (exp < 0) exp = 0;
    m_exp = exp;
}

void Player::setGold(int gold) 
{
    if (gold < 0) gold = 0;
    m_gold = gold;
}

// 属性操作
//增加经验值
void Player::addExp(int amount) 
{
    if (amount <= 0) return;
    m_exp += amount;
    std::cout << " 获得 " << amount << " 经验值！" << std::endl;
    // 循环检查升级（可能连续升级）
    while (canLevelUp()) 
    {
        levelUp();
    }
    // 通知任务系统：等级达到事件
    if (m_taskManager) 
    {
        m_taskManager->onEvent(ConditionType::ReachLevel, m_level, 1);
    }
}

//增加金币
bool Player::addGold(int amount) 
{
    if (amount >= 0) {

        // 增加金币
        m_gold += amount;
        if (amount > 0)
        {
            std::cout << " 获得 " << amount << " 金币！" << std::endl;
        }
        return true;
    } 
    else 
    {
        // 扣除金币
        int need = -amount;
        if (m_gold < need) 
        {
            std::cout << " 金币不足！需要 " << need << "，当前 " << m_gold << std::endl;
            return false;
        }
        m_gold -= need;
        std::cout << " 消耗 " << need << " 金币" << std::endl;
        return true;
    }
}

//恢复生命值
int Player::healHP(int amount) 
{
    if (amount <= 0) return 0;
    
    int oldHP = m_hp;
    m_hp += amount;
    if (m_hp > m_maxHP) 
    {
        m_hp = m_maxHP;
    }
    int actualHeal = m_hp - oldHP;
    
    if (actualHeal > 0)
    {
        std::cout << " 恢复生命值 +" << actualHeal << " (当前 " << m_hp << "/" << m_maxHP << ")" << std::endl;
    }
    return actualHeal;
}
//受到伤害
int Player::takeDamage(int damage) 
{
    if (damage <= 0) return 0;
    int oldHP = m_hp;
    m_hp -= damage;
    if (m_hp < 0)
    {
        m_hp = 0;
    }
    int actualDamage = oldHP - m_hp;
    if (actualDamage > 0)
    {
        std::cout << " 受到伤害 -" << actualDamage << " (剩余 " << m_hp << "/" << m_maxHP << ")" << std::endl;
    }
    return actualDamage;
}

//完全恢复
void Player::fullRestore() 
{
    m_hp = m_maxHP;
    std::cout << " 生命值已完全恢复！ (" << m_hp << "/" << m_maxHP << ")" << std::endl;
}
// 升级相关
int Player::getExpRequired(int level) 
{
    return level * 50 + 20;
}
//检查是否可以升级
bool Player::canLevelUp() const 
{
    return m_exp >= getExpRequired(m_level);
}
//升级
void Player::levelUp() 
{
    // 1. 扣除经验值
    int required = getExpRequired(m_level);
    m_exp -= required;
    // 2. 等级提升
    m_level++;
    // 3. 最大生命值提升（每级+10）
    m_maxHP += 10;
    // 4. 生命值完全恢复
    m_hp = m_maxHP;m_baseAttack += 2;
    m_baseDefense += 1;
    m_speed += 1;
}
// 子系统访问
Bag* Player::getBag() const 
{
    return m_bag;
}
TaskManager* Player::getTaskManager() const 
{
    return m_taskManager;
}

ForgeManager* Player::getForgeManager() const 
{
    return m_forgeManager;
}
// 显示方法
void Player::displayInfo() const 
{
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║                    👤 角色信息                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    std::cout << "  角色名称: " << m_name << "\n";
    std::cout << "  等级: " << m_level << "\n";
    std::cout << "  生命值: " << m_hp << " / " << m_maxHP << "\n";
    std::cout << "  经验值: " << m_exp << " / " << getExpRequired(m_level) << "\n";
    std::cout << "  金币: " << m_gold << "\n";
    std::cout << "  攻击力: " << getTotalAttack() << " (基础 " << m_baseAttack << " + 武器 " << (getTotalAttack() - m_baseAttack) << ")\n";
    std::cout << "  防御力: " << getTotalDefense() << " (基础 " << m_baseDefense << " + 防具 " << (getTotalDefense() - m_baseDefense) << ")\n";
    std::cout << "  速度: " << m_speed << "\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
}

// 序列化（存档/读档）
std::string Player::serialize() const 
{
    std::ostringstream oss;
    oss << m_name << ","
        << m_level << ","
        << m_hp << ","
        << m_maxHP << ","
        << m_exp << ","
        << m_gold;
    return oss.str();
}
//反序列化玩家数据
bool Player::deserialize(const std::string& data)
{
    if (data.empty()) return false;
    
    std::stringstream ss(data);
    std::string token;
    std::vector<std::string> parts;
    // 按逗号分割
    while (std::getline(ss, token, ','))
    {
        parts.push_back(token);
    }
    // 检查字段数量
    if (parts.size() < 9) 
    {
        std::cout << " 玩家数据格式错误！" << std::endl;
        return false;
    }
    try {
        m_name = parts[0];
        m_level = std::stoi(parts[1]);
        m_hp = std::stoi(parts[2]);
        m_maxHP = std::stoi(parts[3]);
        m_exp = std::stoi(parts[4]);
        m_gold = std::stoi(parts[5]);
        
        // 数据校验
        if (m_level < 1) m_level = 1;
        if (m_maxHP < 1) m_maxHP = 50;
        if (m_hp < 0) m_hp = 0;
        if (m_hp > m_maxHP) m_hp = m_maxHP;
        if (m_exp < 0) m_exp = 0;
        if (m_gold < 0) m_gold = 0;
        if (m_baseAttack < 0) m_baseAttack = 0;
        if (m_baseDefense < 0) m_baseDefense = 0;
        if (m_speed < 0) m_speed = 0;
        
        return true;
    } 
    catch (const std::exception& e)
    {
        std::cout << "玩家数据解析失败：" << e.what() << std::endl;
        return false;
    }
}
