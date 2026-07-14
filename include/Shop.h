#ifndef SHOP_H
#define SHOP_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Item.h"

class Player;

// 商品结构体
struct ShopItem 
{
    int id;                          // 商品ID
    std::shared_ptr<Item> item;      // 物品指针（具体物品对象）
    int price;                       // 购买价格
    int stock;                       // 库存数量（-1表示无限）
    bool isUnlocked;                 // 是否已解锁
    int unlockTaskId;                // 解锁所需任务ID（-1表示初始解锁）
    // 构造函数
    ShopItem() : id(0), price(0), stock(-1), isUnlocked(true), unlockTaskId(-1) {}

    ShopItem(int _id, std::shared_ptr<Item> _item, int _price, int _stock = -1, int _unlockTaskId = -1)
        : id(_id), item(_item), price(_price), stock(_stock),isUnlocked(_unlockTaskId == -1), unlockTaskId(_unlockTaskId) {}
};
// 商店类 Shop
class Shop 
{
    public:
    //构造函数
    Shop(Player* player);
    ~Shop() = default;
    //主菜单
    void showMenu();
    //商品管理
    void showShopItems();              // 显示商品列表
    void buyItem(int itemId);          // 购买商品
    void sellItem();                   // 出售物品（让玩家选择）
    //解锁系统
    void unlockItem(int itemId);       // 解锁商品
    void checkUnlockStatus();          // 检查所有商品的解锁状态
    //数据持久化
    std::string serialize() const;     // 序列化商店数据
    void deserialize(const std::string& data);  // 反序列化
    private:
    Player* m_player;                  // 玩家指针
    std::map<int, ShopItem> m_shopItems;  // 商品容器（ID → 商品）
    int m_nextItemId;                  // 下一个商品ID
    //私有方法
    void initShopItems();              // 初始化商品列表
    void updateStock(int itemId, int amount);  // 更新库存
    bool isItemAvailable(int itemId) const;    // 检查商品是否可购买
    void showSellableItems() const;    // 显示可出售的物品列表
};

#endif // SHOP_H
