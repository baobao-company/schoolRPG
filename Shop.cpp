#include "Shop.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include "Task.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Shop::Shop(Player* player) : m_player(player), m_nextItemId(1) {initShopItems();}

// 初始化商品列表
void Shop::initShopItems() 
{
    // 食物（战斗外使用，永久加属性）
    m_shopItems[m_nextItemId++] = ShopItem(1, std::make_shared<Food>("面包", 1, 10, 0, 0), 20, -1, -1);
    m_shopItems[m_nextItemId++] = ShopItem(2, std::make_shared<Food>("烤肉", 1, 20, 0, 0), 40, -1, -1);
    m_shopItems[m_nextItemId++] = ShopItem(3, std::make_shared<Food>("力量果实", 1, 0, 2, 0), 100, 3, -1);
    // 药品（战斗中回血）
    m_shopItems[m_nextItemId++] = ShopItem(4, std::make_shared<Medicine>("回复药水", 1, 30), 30, -1, -1);
    m_shopItems[m_nextItemId++] = ShopItem(5, std::make_shared<Medicine>("高级回复药水", 1, 80), 80, 5, -1);
    // 材料
    m_shopItems[m_nextItemId++] = ShopItem(6, std::make_shared<Material>("普通材料", 1, Material::COMMON), 10, -1, -1);
    m_shopItems[m_nextItemId++] = ShopItem(7, std::make_shared<Material>("稀有材料", 1, Material::UNCOMMON), 50, -1, -1);
    // 装备
    m_shopItems[m_nextItemId++] = ShopItem(8, std::make_shared<Weapon>("铁剑", 1, 8), 100, 5, -1);
    m_shopItems[m_nextItemId++] = ShopItem(9, std::make_shared<Armor>("学院制服", 1, 3), 80, 3, 1001);
}
// 主菜单
void Shop::showMenu() 
{
    // 进入商店时检查解锁状态
    checkUnlockStatus();
    while (true) 
    {
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║                     商店                          ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
        std::cout << "  当前金币: " << m_player->getGold() << " 金\n";
        std::cout << "\n─────────────────────────────────────────────────────────\n";
        std::cout << "  1. 查看商品列表\n";
        std::cout << "  2. 购买商品\n";
        std::cout << "  3. 出售物品\n";
        std::cout << "  4. 返回主菜单\n";
        std::cout << "─────────────────────────────────────────────────────────\n";
        std::cout << "请选择：";
        int choice;
        std::cin >> choice;                             //////////////////////////////////////////////////
        switch (choice) 
        {
            case 1:
                showShopItems();
                break;
            case 2: 
            {
                // 显示商品列表后再让玩家选择购买
                showShopItems();
                std::cout << "\n请输入要购买的商品ID（输入0返回）：";
                int itemId;
                std::cin >> itemId;                     /////////////////////////////////////////////////////
                if (itemId != 0) 
                {
                    buyItem(itemId);
                }
                break;
            }
            case 3:
                sellItem();
                break;
            case 4:
                std::cout << " 离开商店\n";
                return;
            default:
                std::cout << " 无效输入，请重新选择！\n";
                break;
        }
    }
}
// 显示商品列表
void Shop::showShopItems() 
{
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "   商品列表\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << std::left;
    std::cout << "  " << std::setw(4) << "ID" << std::setw(12) << "名称" << std::setw(10) << "类型" << std::setw(10) << "价格" << std::setw(10) << "库存" << "状态\n";
    std::cout << "  ─────────────────────────────────────────────────────\n";
    for (const auto& pair : m_shopItems) 
    {
        const ShopItem& shopItem = pair.second;
        const auto& item = shopItem.item;
        if (!item) continue;
        // 获取物品类型名称
        std::string typeName;
        switch (item->getType()) 
        {
            case Item::FOOD:     typeName = "食物"; break;
            case Item::WEAPON:   typeName = "武器"; break;
            case Item::ARMOR:    typeName = "防具"; break;
            case Item::MEDICINE: typeName = "药品"; break;
            case Item::MATERIAL: typeName = "材料"; break;
            default:             typeName = "未知"; break;
        }
        // 获取库存显示
        std::string stockStr = (shopItem.stock == -1) ? "∞" : std::to_string(shopItem.stock);
        // 获取状态
        std::string statusStr;
        if (!shopItem.isUnlocked) 
        {
            statusStr = "未解锁 (需完成任务" + std::to_string(shopItem.unlockTaskId) + ")";
        } 
        else if (shopItem.stock == 0) 
        {
            statusStr = " 已售罄";
        }
        else 
        {
            statusStr = "可购买";
        }
        std::cout << "  " << std::setw(4) << shopItem.id
                  << std::setw(12) << item->getName()
                  << std::setw(10) << typeName
                  << std::setw(10) << shopItem.price
                  << std::setw(10) << stockStr
                  << statusStr << "\n";
    }
    std::cout << "═══════════════════════════════════════════════════════\n";
}
// 购买商品
void Shop::buyItem(int itemId) 
{
    // 1. 检查商品是否存在
    auto it = m_shopItems.find(itemId);
    if (it == m_shopItems.end()) 
    {
        std::cout << " 商品不存在！\n";
        return;
    }
    ShopItem& shopItem = it->second;
    const auto& item = shopItem.item;
    if (!item) 
    {
        std::cout << " 商品数据错误！\n";
        return;
    }
    // 2. 检查是否已解锁
    if (!shopItem.isUnlocked) 
    {
        std::cout << " 该商品尚未解锁！需要完成任务 ID:" << shopItem.unlockTaskId << " 解锁\n";
        return;
    }
    // 3. 检查库存
    if (shopItem.stock == 0) 
    {
        std::cout << " 该商品已售罄！\n";
        return;
    }
    // 4. 检查金币
    int gold = m_player->getGold();
    if (gold < shopItem.price)
    {
        std::cout << " 金币不足！需要 " << shopItem.price << " 金，当前 " << gold << " 金\n";
        return;
    }
    // 5. 执行购买
    // 扣除金币
    m_player->addGold(-shopItem.price);
    // 减少库存（如果是无限库存则不变）
    if (shopItem.stock > 0) 
    {
        shopItem.stock--;
    }
    // 复制物品添加到背包
    Bag* bag = m_player->getBag();
    Item* itemCopy = cloneItem(item.get());
    if (itemCopy) 
    {
        bag->addItem(itemCopy);
        std::cout << " 购买成功！获得 " << item->getName() << "\n";
        std::cout << "  剩余金币: " << m_player->getGold() << " 金\n";
        if (shopItem.stock > 0) 
        {
            std::cout << "  剩余库存: " << shopItem.stock << "\n";
        } 
        else if (shopItem.stock == -1) 
        {
            std::cout << "  库存: 无限\n";
        }
    } 
    else 
    {
        // 购买失败，退还金币
        m_player->addGold(shopItem.price);
        std::cout << " 物品购买失败，已退还金币！\n";
    }
}
// 出售物品
void Shop::sellItem() 
{
    Bag* bag = m_player->getBag();
    if (!bag) return;
    // 检查背包是否为空
    if (bag->getItemCount() == 0) 
    {
        std::cout << " 背包是空的，没有物品可出售！\n";
        return;
    }
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "   出售物品\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  出售价格 = 商店价格的 50%\n";
    std::cout << "  当前金币: " << m_player->getGold() << " 金\n";
    std::cout << "─────────────────────────────────────────────────────────\n";
    // 显示所有物品（使用 Bag 的 displayAll）
    bag->displayAll();
    std::cout << "─────────────────────────────────────────────────────────\n";
    std::cout << "请输入要出售的物品名称（输入0返回）：";
    std::string itemName;
    std::cin >> itemName;
    if (itemName == "0") return;
    // 查找物品
    Item* item = bag->findItem(itemName);
    if (!item) 
    {
        std::cout << " 背包中没有 " << itemName << "\n";
        return;
    }
    // 显示物品信息
    std::cout << "\n  物品: " << item->getName();
    if (item->getQuantity() > 1) 
    {
        std::cout << " (数量: " << item->getQuantity() << ")";
    }
    std::cout << "\n";
    // 询问出售数量
    int sellQty = 1;
    if (item->getQuantity() > 1) 
    {
        std::cout << "请输入出售数量（1-" << item->getQuantity() << "）：";
        std::cin >> sellQty;
        if (sellQty < 1 || sellQty > item->getQuantity()) 
        {
            std::cout << " 无效数量！\n";
            return;
        }
    }
    // 计算价格
    int pricePerUnit = 0;
    // 在商店商品中查找匹配的价格
    bool foundInShop = false;
    for (const auto& pair : m_shopItems) 
    {
        const ShopItem& shopItem = pair.second;
        if (shopItem.item && shopItem.item->getName() == item->getName()) 
        {
            pricePerUnit = shopItem.price / 2;  // 半价出售
            foundInShop = true;
            break;
        }
    }
    // 如果商店中没有该商品，使用默认价格（物品类型基础价格）
    if (!foundInShop) 
    {
        // 根据类型设定基础价格
        switch (item->getType()) 
        {
            case Item::FOOD:     pricePerUnit = 10; break;
            case Item::WEAPON:   pricePerUnit = 40; break;
            case Item::ARMOR:    pricePerUnit = 30; break;
            case Item::MEDICINE: pricePerUnit = 15; break;
            case Item::MATERIAL: pricePerUnit = 5; break;
            default:             pricePerUnit = 10; break;
        }
        std::cout << "  (该物品在商店中无定价，使用默认价格)\n";
    }
    int totalPrice = pricePerUnit * sellQty;
    // 确认出售
    std::cout << "\n出售 " << item->getName() << " ×" << sellQty << "，获得 " << totalPrice << " 金，是否确认？(y/n)：";
    char confirm;
    std::cin >> confirm;                //////////////////////////////////////////////////////////
    if (confirm != 'y' && confirm != 'Y') 
    {
        std::cout << "已取消出售。\n";
        return;
    }
    // 执行出售
    bag->removeItem(item->getName(), sellQty);
    m_player->addGold(totalPrice);
    std::cout << " 出售成功！获得 " << totalPrice << " 金\n";
}

// 解锁系统
void Shop::unlockItem(int itemId) 
{
    auto it = m_shopItems.find(itemId);
    if (it == m_shopItems.end()) 
    {
        return;
    }
    
    if (!it->second.isUnlocked) 
    {
        it->second.isUnlocked = true;
        std::cout << " 商店已解锁新商品: " << it->second.item->getName() << "！\n";
    }
}

//查看所有商品的解锁状态
void Shop::checkUnlockStatus() 
{
    TaskManager* taskMgr = m_player->getTaskManager();
    if (!taskMgr) return;
    for (auto& pair : m_shopItems) 
    {
        ShopItem& shopItem = pair.second;
        if (shopItem.unlockTaskId == -1) 
        {
            // 初始解锁
            shopItem.isUnlocked = true;
        } 
        else 
        {
            // 检查任务是否已完成
            Task* task = taskMgr->getTask(shopItem.unlockTaskId);
            if (task && task->getStatus() == TaskStatus::Finished) 
            {
                shopItem.isUnlocked = true;
            }
        }
    }
}

// 数据持久化（存档/读档）
std::string Shop::serialize() const 
{
    std::ostringstream oss;
    bool first = true;

    for (const auto& pair : m_shopItems) 
    {
        if (!first) oss << "|";
        oss << pair.first << ":" << pair.second.stock;
        first = false;
    }
    return oss.str();
}

//反序列化商店数据

void Shop::deserialize(const std::string& data) 
{
    if (data.empty()) return;
    std::stringstream ss(data);
    std::string token;
    while (std::getline(ss, token, '|')) 
    {
        if (token.empty()) continue;
        size_t colonPos = token.find(':');
        if (colonPos == std::string::npos) continue;
        try
        {
            int id = std::stoi(token.substr(0, colonPos));
            int stock = std::stoi(token.substr(colonPos + 1));
            
            auto it = m_shopItems.find(id);
            if (it != m_shopItems.end()) 
            {
                it->second.stock = stock;
            }
        } 
        catch (const std::exception& e) 
        {
            // 忽略解析错误
        }
    }
    // 恢复后重新检查解锁状态
    checkUnlockStatus();
}