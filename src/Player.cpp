#include "Player.h"
#include "Bag.h"
#include "Shop.h"
#include "Task.h"
#include "ForgeManager.h"
#include "Item.h"
#include <sstream>
#include <algorithm>
#include <algorithm>
#include <codecvt>
#include <locale>
#include <cwchar>

// UTF8 宽字符互转，彻底解决中文方框乱码
static std::string WstrToUtf8(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}
static std::wstring Utf8ToWstr(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.from_bytes(str);
}
// 逗号转义，防止分割错乱
static std::string escapeComma(const std::wstring& wstr)
{
    std::wstring temp = wstr;
    for (wchar_t& c : temp)
        if (c == L',') c = L'#';
    return WstrToUtf8(temp);
}
static std::wstring unescapeComma(const std::string& s)
{
    std::wstring temp = Utf8ToWstr(s);
    for (wchar_t& c : temp)
        if (c == L'#') c = L',';
    return temp;
}

Player::Player(const std::wstring& name)
    : m_name(name), m_level(1), m_hp(50), m_maxHP(50),
    m_baseAtk(5), m_baseDef(2), m_speed(3), m_exp(0), m_gold(100),
    m_equipWep(nullptr), m_equipArm(nullptr)
{
    m_bag = new Bag(name + L"背包", 100);
    m_shop = new Shop(this);
    m_taskMgr = new TaskManager(this);
    m_forge = new ForgeManager(this);
     // 赠送运动护腕
    std::shared_ptr<Weapon> giftWeapon = std::make_shared<Weapon>(L"手套", 1, 5);
    m_bag->addItem(giftWeapon);

    // 新增一件新手防具示例（护甲+3）
    std::shared_ptr<Armor> giftArmor = std::make_shared<Armor>(L"校服外套", 1, 3);
    m_bag->addItem(giftArmor);
}
Player::~Player()
{
    delete m_bag;
    delete m_shop;
    delete m_taskMgr;
    delete m_forge;
}
bool Player::equipWeapon(Weapon* w)
{
    if(!w) return false;
    if(m_equipWep) unequipWeapon();
    if(m_bag->removeItem(w, false))
    {
        m_equipWep = w;
        return true;
    }
    return false;
}
bool Player::equipArmor(Armor* a)
{
    if(!a) return false;
    if(m_equipArm) unequipArmor();
    if(m_bag->removeItem(a, false))
    {
        m_equipArm = a;
        return true;
    }
    return false;
}
void Player::unequipWeapon()
{
    if(!m_equipWep) return;
    if(m_bag->addItem(m_equipWep)) m_equipWep = nullptr;
}
void Player::unequipArmor()
{
    if(!m_equipArm) return;
    if(m_bag->addItem(m_equipArm)) m_equipArm = nullptr;
}
std::wstring Player::getName() const { return m_name; }
int Player::getLevel() const { return m_level; }
int Player::getHP() const { return m_hp; }
int Player::getMaxHP() const { return m_maxHP; }
int Player::getBaseAttack() const { return m_baseAtk; }
int Player::getBaseDefense() const { return m_baseDef; }
int Player::getSpeed() const { return m_speed; }
int Player::getExp() const { return m_exp; }
int Player::getGold() const { return m_gold; }
Weapon* Player::getEquipWep() const { return m_equipWep; }
Armor* Player::getEquipArm() const { return m_equipArm; }
int Player::getTotalAtk() const
{
    int w = m_equipWep ? m_equipWep->getDamage() : 0;
    return m_baseAtk + w;
}
int Player::getTotalDef() const
{
    int a = m_equipArm ? m_equipArm->getDef() : 0;
    return m_baseDef + a;
}
int Player::getExpNext() const { return expNeed(m_level); }
void Player::setName(const std::wstring& n) { m_name = n; }
void Player::setLevel(int l) { if(l<1) l=1; if(l>100) l=100; m_level=l; }
void Player::setHP(int hp) { if(hp<0)hp=0; if(hp>m_maxHP)hp=m_maxHP; m_hp=hp; }
void Player::setMaxHP(int hp) { if(hp<1)hp=1; m_maxHP=hp; if(m_hp>m_maxHP)m_hp=m_maxHP; }
void Player::setBaseAttack(int v) { if(v<0)v=0; m_baseAtk=v; }
void Player::setBaseDefense(int v) { if(v<0)v=0; m_baseDef=v; }
void Player::setSpeed(int v) { if(v<0)v=0; m_speed=v; }
void Player::setExp(int e) { if(e<0)e=0; m_exp=e; }
void Player::setGold(int g) { if(g<0)g=0; m_gold=g; }
void Player::addBaseAttack(int v)
{
    if (v < 0) v = 0;
    m_baseAtk += v;
}

void Player::addBaseDefense(int v)
{
    if (v < 0) v = 0;
    m_baseDef += v;
}
void Player::addExp(int amt)
{
    if(amt <=0) return;
    m_exp += amt;
    while(canLevelUp()) levelUp();
    m_taskMgr->onEvent(ConditionType::ReachLevel, m_level, 1);
}
bool Player::addGold(int amt)
{
    if(amt >=0)
    {
        m_gold += amt;
        m_taskMgr->onEvent(ConditionType::HaveGold, m_gold, 1);
        return true;
    }
    int cost = -amt;
    if(m_gold < cost) return false;
    m_gold -= cost;
    m_taskMgr->onEvent(ConditionType::HaveGold, m_gold, 1);
    return true;
}
int Player::healHP(int amt)
{
    if(amt <=0) return 0;
    int old = m_hp;
    m_hp += amt;
    if(m_hp > m_maxHP) m_hp = m_maxHP;
    return m_hp - old;
}
int Player::takeDmg(int dmg)
{
    if(dmg <=0) return 0;
    int old = m_hp;
    m_hp -= dmg;
    if(m_hp <0) m_hp=0;
    return old - m_hp;
}
void Player::fullRestore() { m_hp = m_maxHP; }
int Player::expNeed(int lvl) { return lvl * 50 + 20; }
bool Player::canLevelUp() const { return m_exp >= expNeed(m_level); }
void Player::levelUp()
{
    int req = expNeed(m_level);
    m_exp -= req;
    m_level++;
    m_maxHP +=10;
    m_hp = m_maxHP;
    m_baseAtk +=2;
    m_baseDef +=1;
    m_speed +=1;
}
Bag* Player::getBag() const { return m_bag; }
Shop* Player::getShop() const { return m_shop; }
TaskManager* Player::getTaskManager() const { return m_taskMgr; }
ForgeManager* Player::getForge() const { return m_forge; }
std::wstring Player::getInfoText() const
{
    std::wstringstream ss;
    ss << L"角色：" << m_name << L"\n"
    << L"等级：" << std::to_wstring(m_level) << L"\n"
    << L"生命：" << std::to_wstring(m_hp) << L"/" << std::to_wstring(m_maxHP) << L"\n"
    << L"经验：" << std::to_wstring(m_exp) << L"/" << std::to_wstring(getExpNext()) << L"\n"
    << L"金币：" << std::to_wstring(m_gold) << L"\n"
    << L"总攻击：" << std::to_wstring(getTotalAtk()) << L"(基础" << std::to_wstring(m_baseAtk) << L")\n"
    << L"总防御：" << std::to_wstring(getTotalDef()) << L"(基础" << std::to_wstring(m_baseDef) << L")\n"
    << L"速度：" << std::to_wstring(m_speed);
    if(m_equipWep) ss << L"\n装备武器：" << m_equipWep->getName();
    if(m_equipArm) ss << L"\n装备防具：" << m_equipArm->getName();
    return ss.str();
}

std::string Player::serialize() const
{
    std::string nameStr = escapeComma(m_name);
    std::wstring equipWName = m_equipWep ? m_equipWep->getName() : L"";
    std::wstring equipAName = m_equipArm ? m_equipArm->getName() : L"";
    std::string wStr = escapeComma(equipWName);
    std::string aStr = escapeComma(equipAName);

    std::stringstream ss;
    ss << nameStr << ","
       << m_level << ","
       << m_hp << ","
       << m_maxHP << ","
       << m_exp << ","
       << m_gold << ","
       << m_baseAtk << ","
       << m_baseDef << ","
       << m_speed << ","
       << wStr << ","
       << aStr;
    return ss.str();
}
bool Player::deserialize(const std::string& data)
{
    if (data.empty()) return false;
    std::vector<std::string> parts;
    std::stringstream ss(data);
    std::string buf;
    while(std::getline(ss, buf, ',')) parts.push_back(buf);
    // 现在至少11段：名字,lv,hp,maxhp,exp,gold,atk,def,speed,weapon,armor
    if(parts.size() < 11) return false;
    try
    {
        m_name = unescapeComma(parts[0]);
        m_level = std::stoi(parts[1]);
        m_hp = std::stoi(parts[2]);
        m_maxHP = std::stoi(parts[3]);
        m_exp = std::stoi(parts[4]);
        m_gold = std::stoi(parts[5]);
        m_baseAtk = std::stoi(parts[6]);
        m_baseDef = std::stoi(parts[7]);
        m_speed = std::stoi(parts[8]);
        std::wstring equipWName = unescapeComma(parts[9]);
        std::wstring equipAName = unescapeComma(parts[10]);

        setLevel(m_level);
        setHP(m_hp);
        setMaxHP(m_maxHP);
        setExp(m_exp);
        setGold(m_gold);
        setBaseAttack(m_baseAtk);
        setBaseDefense(m_baseDef);
        setSpeed(m_speed);

        // 自动重新穿戴装备
        m_equipWep = nullptr;
        m_equipArm = nullptr;
        Bag* bag = getBag();
        if (!equipWName.empty())
        {
            for (auto w : bag->getWeapons())
            {
                if (w->getName() == equipWName)
                {
                    equipWeapon(w);
                    break;
                }
            }
        }
        if (!equipAName.empty())
        {
            for (auto a : bag->getArmors())
            {
                if (a->getName() == equipAName)
                {
                    equipArmor(a);
                    break;
                }
            }
        }
        return true;
    }
    catch(...) { return false; }
}
