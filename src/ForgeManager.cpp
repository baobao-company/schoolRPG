#include "ForgeManager.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include <sstream>
ForgeManager::ForgeManager(Player* p) : m_player(p) { initConfig(); }
void ForgeManager::initConfig()
{
    m_cfg = {
        {0,1,L"普通材料",3,100,2,2},
        {1,2,L"普通材料",4,200,2,2},
        {2,3,L"普通材料",5,300,3,2},
        {3,4,L"稀有材料",2,500,3,2},
        {4,5,L"稀有材料",3,700,4,2},
        {5,6,L"稀有材料",4,900,4,2},
        {6,7,L"罕见材料",2,1200,5,2},
        {7,8,L"罕见材料",3,1500,5,2},
        {8,9,L"史诗材料",2,2000,6,2},
        {9,10,L"传说材料",1,3000,10,2},
    };
}
std::wstring ForgeManager::getMaterialText() const
{
    Bag* bag = m_player->getBag();
    std::vector<Material*> mats = bag->getMaterials();
    int cnt[5] = {0,0,0,0};
    std::vector<std::wstring> names = {L"普通材料",L"稀有材料",L"罕见材料",L"史诗材料",L"传说材料"};
    for (auto m : mats)
    {
        int idx = (int)m->getRarity();
        if (idx >=0 && idx <5) cnt[idx] += m->getQuantity();
    }
    std::wstringstream ss;
    ss << L"库存材料：\n";
    for (int i=0; i<5; i++) ss << names[i] << L" ×" << std::to_wstring(cnt[i]) << L"\n";
    return ss.str();
}
std::wstring ForgeManager::getRuleText() const
{
    std::wstringstream ss;
    ss << L"====强化规则====\n";
    for (auto& c : m_cfg)
    {
        ss << L"+" << std::to_wstring(c.from) << L"→+" << std::to_wstring(c.to)
        << L" 材料" << c.matName << L"×" << std::to_wstring(c.matCount)
        << L" 金币" << std::to_wstring(c.goldCost)
        << L" 武器+" << std::to_wstring(c.atkBonus) << L" 防具+" << std::to_wstring(c.defBonus) << L"\n";
    }
    ss << L"上限+10，强化100%成功\n";
    return ss.str();
}
std::vector<Item*> ForgeManager::getEnhanceable() const
{
    std::vector<Item*> res;
    Bag* bag = m_player->getBag();
    auto ws = bag->getWeapons();
    auto as = bag->getArmors();
    for (auto w : ws) res.push_back(w);
    for (auto a : as) res.push_back(a);
    return res;
}
bool ForgeManager::checkRes(const EnchantLevelConfig& cfg)
{
    if (m_player->getGold() < cfg.goldCost) return false;
    Bag* bag = m_player->getBag();
    int have = 0;
    for (auto m : bag->getMaterials())
        if (m->getName() == cfg.matName) have += m->getQuantity();
    return have >= cfg.matCount;
}
void ForgeManager::consumeRes(const EnchantLevelConfig& cfg)
{
    m_player->addGold(-cfg.goldCost);
    int need = cfg.matCount;
    Bag* bag = m_player->getBag();
    for (auto m : bag->getMaterials())
    {
        if (m->getName() == cfg.matName && need >0)
        {
            int sub = std::min(need, m->getQuantity());
            bag->removeItem(m->getName(), sub);
            need -= sub;
        }
    }
}
std::wstring ForgeManager::enhancePreview(Item* item)
{
    std::wstringstream ss;
    int lvl = item->getEnhanceLevel();
    if (lvl >=10) return L"该装备已满级";
    auto& cfg = m_cfg[lvl];
    ss << item->getDisplayText() << L"\n";
    ss << L"+" << std::to_wstring(lvl) << L" → +" << std::to_wstring(cfg.to)<< L"\n";
    if (item->isEnhanceable() && item->getType() == Item::WEAPON)
    {
        Weapon* w = static_cast<Weapon*>(item);
        int old = w->getDamage();
        w->addEnhanceLevel(cfg.atkBonus);
        int nw = w->getDamage();
        w->setEnhanceLevel(lvl);
        ss << L"攻击 " << std::to_wstring(old) << L" → " << std::to_wstring(nw) << L"\n";
    }
    else
    {
        Armor* a = static_cast<Armor*>(item);
        int old = a->getDef();
        a->addEnhanceLevel(cfg.defBonus);
        int nw = a->getDef();
        a->setEnhanceLevel(lvl);
        ss << L"防御 " << std::to_wstring(old) << L" → " << std::to_wstring(nw) << L"\n";
    }
    ss << L"消耗：" << cfg.matName << L"×" << std::to_wstring(cfg.matCount) << L" " << std::to_wstring(cfg.goldCost) << L"金币";
    return ss.str();
}
std::wstring ForgeManager::doEnhance(Item* item)
{
    int lvl = item->getEnhanceLevel();
    if (lvl >=10 || !item->isEnhanceable()) return L"无法强化";
    auto& cfg = m_cfg[lvl];
    if (!checkRes(cfg)) return L"材料/金币不足";
    consumeRes(cfg);
    item->setEnhanceLevel(cfg.to);
    return L"强化成功：" + item->getDisplayText();
}
std::wstring ForgeManager::craft(int lvl)
{
    if (lvl >=4) return L"传说材料无法合成";
    std::vector<std::wstring> names = {L"普通材料",L"稀有材料",L"罕见材料",L"史诗材料",L"传说材料"};
    std::wstring src = names[lvl];
    std::wstring dst = names[lvl+1];
    Bag* bag = m_player->getBag();
    int have = 0;
    for (auto m : bag->getMaterials())
        if (m->getName() == src) have += m->getQuantity();
    if (have <3) return src + L"数量不足";
    bag->removeItem(src, 3);
    bool exist = false;
    for (auto m : bag->getMaterials())
    {
        if (m->getName() == dst)
        {
            m->addQuantity(1);
            exist = true;
            break;
        }
    }
    if (!exist)
    {
        Material* newMat = new Material(dst,1,(Material::Rarity)(lvl+1));
        bag->addItem(newMat);
    }
    return L"合成成功，获得" + dst + L"×1";
}
std::vector<Item*> ForgeManager::getEquippableAll() const
{
    std::vector<Item*> res;
    Bag* bag = m_player->getBag();
    // 背包所有武器防具
    auto ws = bag->getWeapons();
    auto as = bag->getArmors();
    for (Weapon* w : ws) res.push_back(w);
    for (Armor* a : as) res.push_back(a);
    // 身上穿戴装备也加入列表，不用脱下就能强化
    Weapon* eqW = m_player->getEquipWep();
    Armor* eqA = m_player->getEquipArm();
    if (eqW) res.push_back(eqW);
    if (eqA) res.push_back(eqA); // 修复这里
    return res;
}
