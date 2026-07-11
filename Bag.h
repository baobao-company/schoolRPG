#ifndef BAG_H
#define BAG_H

#include "Item.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>



class Bag 
{
    private:
    std::string m_ownerName;     // 背包持有者名称
    int m_capacity;              // 背包容量上限
    std::vector<Item*> m_items;  // 物品存储容器
    public:
    Bag(const std::string& ownerName = "Player", int capacity = 20);
    ~Bag();
    //禁用拷贝
    Bag(const Bag&) = delete;
    Bag& operator=(const Bag&) = delete;

    //物品管理
    bool addItem(Item* item);
    
    //添加物品（智能指针版本)
    bool addItem(std::shared_ptr<Item> item);
    //移除物品
    bool removeItem(const std::string& name, int quantity = 1);

    //使用物品
    bool useItem(const std::string& name, Player* player);

    //查找物品
    Item* findItem(const std::string& name);

    //按类型查询
    
    //按类型获取所有物品
    std::vector<Item*> getItemsByType(Item::ItemType type) const;
    std::vector<Weapon*> getWeapons() const;
    std::vector<Armor*> getArmors() const;
    std::vector<Material*> getMaterials() const;

    //显示方法
    //按类型显示物品
    void displayByType(Item::ItemType type) const;
    //显示所有物品,按武器→防具→药品→食物→材料的顺序分类显示
    void displayAll() const;
    //统计信息
    //获取物品总数量（所有物品数量之和）
    int getTotalItems() const;
    //获取指定类型的物品数量
    int getTypeCount(Item::ItemType type) const;
    // 获取物品种类数（背包中有多少种不同的物品）
    int getItemCount() const;
    //获取背包容量上限
    int getCapacity() const;
    //管理操作
    //清空背包
    void clear();
    //按类型排序
    void sortByType();
    
    //按名称排序
    void sortByName();
};

Item* cloneItem(const Item* item);

#endif // BAG_H
