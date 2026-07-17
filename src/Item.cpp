#include "Item.h"
#include "Player.h"
Item::Item(const std::wstring& name, ItemType type, int qty)
    : m_name(name), m_type(type), m_quantity(qty), m_enhanceLevel(0)
{}
std::wstring Item::getName() const { return m_name; }
Item::ItemType Item::getType() const { return m_type; }
int Item::getQuantity() const { return m_quantity; }
std::wstring Item::getDescription() const { return m_description; }
int Item::getEnhanceLevel() const { return m_enhanceLevel; }
void Item::setQuantity(int q) { m_quantity = q; }
void Item::addQuantity(int q) { m_quantity += q; if(m_quantity < 0) m_quantity = 0; }
void Item::setDescription(const std::wstring& d) { m_description = d; }
void Item::setEnhanceLevel(int lvl)
{
    if(lvl < 0) m_enhanceLevel = 0;
    else if(lvl > 10) m_enhanceLevel = 10;
    else m_enhanceLevel = lvl;
}
void Item::addEnhanceLevel(int delta) { setEnhanceLevel(m_enhanceLevel + delta); }
bool Item::isEnhanceable() const { return false; }
std::wstring Item::getDisplayText() const
{
    std::wstringstream ss;
    ss << m_name << L" ×" << std::to_wstring(m_quantity);
    if(m_enhanceLevel > 0) ss << L" +" << std::to_wstring(m_enhanceLevel);
    if(!m_description.empty()) ss << L" | " << m_description;
    return ss.str();
}
// Food
Food::Food(const std::wstring& name, int qty, int hp, int atk, int def)
    : Item(name, FOOD, qty), m_hpBonus(hp), m_atkBonus(atk), m_defBonus(def)
{}
void Food::use(Player* p)
{
    if(m_quantity <= 0) return;
    if(m_hpBonus > 0) p->setMaxHP(p->getMaxHP() + m_hpBonus), p->healHP(m_hpBonus);
    if(m_atkBonus > 0) p->addBaseAttack(m_atkBonus);
    if(m_defBonus > 0) p->addBaseDefense(m_defBonus);
    m_quantity--;
}
std::wstring Food::getDisplayText() const
{
    std::wstringstream ss;
    ss << L"[食物] " << Item::getDisplayText();
    ss << L" HP+" << std::to_wstring(m_hpBonus) << L" ATK+" << std::to_wstring(m_atkBonus) << L" DEF+" << std::to_wstring(m_defBonus);
    return ss.str();
}
int Food::getHpBonus() const { return m_hpBonus; }
int Food::getAtkBonus() const { return m_atkBonus; }
int Food::getDefBonus() const { return m_defBonus; }
// Weapon
Weapon::Weapon(const std::wstring& name, int qty, int dmg)
    : Item(name, WEAPON, qty), m_baseDamage(dmg)
{}
void Weapon::use(Player* p) { p->equipWeapon(this); }
std::wstring Weapon::getDisplayText() const
{
    std::wstringstream ss;
    ss << L"[武器] " << Item::getDisplayText() << L" 攻击:" << std::to_wstring(getDamage());
    return ss.str();
}
int Weapon::getBaseDamage() const { return m_baseDamage; }
int Weapon::getEnhanceDmg() const
{
    int lvl = m_enhanceLevel;
    if(lvl == 0) return 0;
    if(lvl <=2) return lvl*2;
    if(lvl <=5) return 4 + (lvl-2)*3;
    if(lvl <=8) return 13 + (lvl-5)*4;
    return 25 + (lvl-8)*5;
}
int Weapon::getDamage() const { return m_baseDamage + getEnhanceDmg(); }
// Armor
Armor::Armor(const std::wstring& name, int qty, int def)
    : Item(name, ARMOR, qty), m_baseDef(def)
{}
void Armor::use(Player* p) { p->equipArmor(this); }
std::wstring Armor::getDisplayText() const
{
    std::wstringstream ss;
    ss << L"[防具] " << Item::getDisplayText() << L" 防御:" << std::to_wstring(getDef());
    return ss.str();
}
int Armor::getBaseDef() const { return m_baseDef; }
int Armor::getEnhanceDef() const { return m_enhanceLevel * 2; }
int Armor::getDef() const { return m_baseDef + getEnhanceDef(); }
// Medicine
Medicine::Medicine(const std::wstring& name, int qty, int heal)
    : Item(name, MEDICINE, qty), m_healAmount(heal)
{}
void Medicine::use(Player* p)
{
    if(m_quantity <= 0) return;
    int restore = p->healHP(m_healAmount);
    if(restore > 0) m_quantity--;
}
std::wstring Medicine::getDisplayText() const
{
    std::wstringstream ss;
    ss << L"[药品] " << Item::getDisplayText() << L" 治疗:" << std::to_wstring(m_healAmount);
    return ss.str();
}
int Medicine::getHealAmt() const { return m_healAmount; }
// Material
Material::Material(const std::wstring& name, int qty, Rarity r)
    : Item(name, MATERIAL, qty), m_rarity(r)
{}
void Material::use(Player*) {}
std::wstring Material::getDisplayText() const
{
    std::wstringstream ss;
    ss << L"[材料] " << Item::getDisplayText() << L" 稀有度:" << getRarityStr();
    return ss.str();
}
Material::Rarity Material::getRarity() const { return m_rarity; }
std::wstring Material::getRarityStr() const
{
    switch(m_rarity)
    {
        case COMMON: return L"普通";
        case UNCOMMON: return L"稀有";
        case RARE: return L"罕见";
        case EPIC: return L"史诗";
        case LEGENDARY: return L"传说";
        default: return L"未知";
    }
}
bool Material::canMerge(const Material& other) const { return m_rarity == other.m_rarity; }
// 物品克隆
Item* cloneItem(const Item* src)
{
    if(!src) return nullptr;
    switch(src->getType())
    {
        case Item::FOOD:
        {
            auto f = dynamic_cast<const Food*>(src);
            return new Food(f->getName(), f->getQuantity(), f->getHpBonus(), f->getAtkBonus(), f->getDefBonus());
        }
        case Item::WEAPON:
        {
            auto w = dynamic_cast<const Weapon*>(src);
            auto nw = new Weapon(w->getName(), w->getQuantity(), w->getBaseDamage());
            nw->setEnhanceLevel(w->getEnhanceLevel());
            return nw;
        }
        case Item::ARMOR:
        {
            auto a = dynamic_cast<const Armor*>(src);
            auto na = new Armor(a->getName(), a->getQuantity(), a->getBaseDef());
            na->setEnhanceLevel(a->getEnhanceLevel());
            return na;
        }
        case Item::MEDICINE:
        {
            auto m = dynamic_cast<const Medicine*>(src);
            return new Medicine(m->getName(), m->getQuantity(), m->getHealAmt());
        }
        case Item::MATERIAL:
        {
            auto mat = dynamic_cast<const Material*>(src);
            return new Material(mat->getName(), mat->getQuantity(), mat->getRarity());
        }
        default: return nullptr;
    }
}
