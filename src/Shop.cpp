#include "Shop.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include "Task.h"
#include <sstream>
ShopItem::ShopItem() : id(0), item(nullptr), price(0), stock(-1), unlocked(true), unlockTask(-1) {}
ShopItem::ShopItem(int id, std::shared_ptr<Item> it, int p, int st, int tk)
    : id(id), item(it), price(p), stock(st), unlocked(tk==-1), unlockTask(tk) {}
Shop::Shop(Player* p) : m_player(p), m_nextId(1) { initGoods(); }
void Shop::initGoods()
{
    m_goods[m_nextId++] = ShopItem(1, std::make_shared<Food>(L"面包",1,10,0,0),20);
    m_goods[m_nextId++] = ShopItem(2, std::make_shared<Food>(L"烤肉",1,20,0,0),40);
    m_goods[m_nextId++] = ShopItem(3, std::make_shared<Food>(L"力量果实",1,0,2,0),100, -1,3);
    m_goods[m_nextId++] = ShopItem(4, std::make_shared<Medicine>(L"回复药水",1,30),30);
    m_goods[m_nextId++] = ShopItem(5, std::make_shared<Medicine>(L"高级回复药水",1,80),80,-1,5);
    m_goods[m_nextId++] = ShopItem(6, std::make_shared<Material>(L"普通材料",1,Material::COMMON),10);
    m_goods[m_nextId++] = ShopItem(7, std::make_shared<Material>(L"稀有材料",1,Material::UNCOMMON),50);
    m_goods[m_nextId++] = ShopItem(8, std::make_shared<Weapon>(L"铁剑",1,8),100,-1,5);
    m_goods[m_nextId++] = ShopItem(9, std::make_shared<Armor>(L"学院制服",1,3),80,-1,1001);
}
void Shop::refreshUnlock()
{
    TaskManager* tm = m_player->getTaskManager();
    for (auto& pair : m_goods)
    {
        auto& si = pair.second;
        if (si.unlockTask == -1) continue;
        Task* t = tm->getById(si.unlockTask);
        if (t && t->getStatus() == TaskStatus::Finished)
            si.unlocked = true;
    }
}
std::wstring Shop::getShopList() const
{
    std::wstringstream ss;
    ss << L"====商店商品 金币:" << std::to_wstring(m_player->getGold()) << L"\n";
    for (auto& p : m_goods)
    {
        auto& g = p.second;
        std::wstring st = g.stock == -1 ? L"无限" : std::to_wstring(g.stock);
        std::wstring lock = g.unlocked ? L"可购买" : L"未解锁(任务"+std::to_wstring(g.unlockTask)+L")";
        ss << std::to_wstring(g.id) << L"." << g.item->getName() << L" 价格" << std::to_wstring(g.price) << L" 库存" << st << L" " << lock << L"\n";
    }
    return ss.str();
}
std::wstring Shop::buy(int id)
{
    refreshUnlock();
    auto it = m_goods.find(id);
    if (it == m_goods.end()) return L"商品不存在";
    ShopItem& gi = it->second;
    if (!gi.unlocked) return L"未解锁";
    if (gi.stock == 0) return L"售罄";
    if (m_player->getGold() < gi.price) return L"金币不足";
    m_player->addGold(-gi.price);
    if (gi.stock >0) gi.stock--;
    Item* copy = cloneItem(gi.item.get());
    Bag* bag = m_player->getBag();
    bool ok = bag->addItem(copy);
    if (!ok)
    {
        m_player->addGold(gi.price);
        delete copy;
        return L"背包已满";
    }
    return L"购买成功：" + gi.item->getName();
}
std::wstring Shop::sellItem(const std::wstring& name, int qty)
{
    Bag* bag = m_player->getBag();
    Item* it = bag->findItem(name);
    if (!it || it->getQuantity() < qty) return L"物品不足";
    int unit = 0;
    bool foundShop = false;
    for (auto& p : m_goods)
    {
        auto& g = p.second;
        if (g.item->getName() == name)
        {
            unit = g.price / 2;
            foundShop = true;
            break;
        }
    }
    if (!foundShop)
    {
        switch (it->getType())
        {
            case Item::FOOD: unit=10; break;
            case Item::WEAPON: unit=40; break;
            case Item::ARMOR: unit=30; break;
            case Item::MEDICINE: unit=15; break;
            case Item::MATERIAL: unit=5; break;
            default: unit=10;
        }
    }
    int total = unit * qty;
    bag->removeItem(name, qty);
    m_player->addGold(total);
    return L"出售成功，获得" + std::to_wstring(total) + L"金币";
}
std::string Shop::serialize() const
{
    std::stringstream ss;
    bool first = true;
    for (auto& p : m_goods)
    {
        if (!first)
            ss << "|";
        first = false;
        ss << p.first << ":" << p.second.stock << ":" << (p.second.unlocked ? 1 : 0);
    }
    return ss.str();
}
void Shop::deserialize(const std::string& data)
{
    if (data.empty())
        return;

    std::stringstream ss(data);
    std::string seg;
    while (std::getline(ss, seg, '|'))
    {
        if (seg.empty())
            continue;

        size_t pos1 = seg.find(':');
        if (pos1 == std::string::npos)
            continue;
        size_t pos2 = seg.find(':', pos1 + 1);
        if (pos2 == std::string::npos)
            continue;

        try
        {
            int goodsId = std::stoi(seg.substr(0, pos1));
            int stockNum = std::stoi(seg.substr(pos1 + 1, pos2 - pos1 - 1));
            int unlockFlag = std::stoi(seg.substr(pos2 + 1));

            auto it = m_goods.find(goodsId);
            if (it != m_goods.end())
            {
                it->second.stock = stockNum;
                it->second.unlocked = (unlockFlag == 1);
            }
        }
        catch (...)
        {
            continue;
        }
    }
    refreshUnlock();
}
std::tuple<std::wstring, int, int, bool> Shop::getShopGoodInfo(int goodsId)
{
    auto it = m_goods.find(goodsId);
    if (it == m_goods.end())
        return {L"无效商品", 0, 0, false};
    ShopItem& item = it->second;
    return {item.item->getName(), item.price, item.stock, item.unlocked};
}
    checkUnlockStatus();
}
