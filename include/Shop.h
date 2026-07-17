#ifndef SHOP_H
#define SHOP_H
#include <string>
#include <vector>
#include <map>
#include <memory>
class Player;
class Item;
struct ShopItem
{
    int id;
    std::shared_ptr<Item> item;
    int price;
    int stock;
    bool unlocked;
    int unlockTask;
    ShopItem();
    ShopItem(int id, std::shared_ptr<Item> it, int p, int st=-1, int tk=-1);
};
class Shop
{
private:
    Player* m_player;
    std::map<int, ShopItem> m_goods;
    int m_nextId;
    void initGoods();
public:
    Shop(Player* p);
    std::wstring getShopList() const;
    std::wstring buy(int id);
    std::wstring sellItem(const std::wstring& name, int qty);
    void refreshUnlock();
    std::string serialize() const;
    void deserialize(const std::string& data);
    std::tuple<std::wstring, int, int, bool> getShopGoodInfo(int goodsId);
};
#endif
