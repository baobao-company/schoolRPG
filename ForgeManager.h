#ifndef FORGE_MANAGER_H
#define FORGE_MANAGER_H

#include <vector>
#include <string>

class Player;
class Item;
class Weapon;
class Armor;

//强化等级配置
struct EnchantLevelConfig 
{
    int fromLevel;          // 当前等级
    int toLevel;            // 目标等级
    std::string materialName; // 消耗材料名称
    int materialCount;      // 消耗材料数量
    int goldCost;           // 消耗金币
    int attackBonus;        // 武器攻击增量（参考值，由武器自身计算）
    int defenseBonus;       // 防具防御增量（固定+2）
};

// 锻造工坊类 ForgeManager
class ForgeManager 
{
    private:
    Player* m_player;                // 玩家指针
    std::vector<EnchantLevelConfig> m_config;  // 强化配置表
    //私有方法
    void initConfig();               // 初始化强化配置
    bool checkEnchantResources(const EnchantLevelConfig& config);  // 检查资源
    void consumeEnchantResources(const EnchantLevelConfig& config); // 消耗资源
    public:
    ForgeManager(Player* player);
    ~ForgeManager() = default;
    //主菜单
    void showMenu();
    //强化相关
    void showEnhanceMenu();          // 显示可强化装备列表
    void enhanceItem(Item* item);    // 强化指定装备
    //合成相关
    void showCraftMenu();            // 显示合成界面
    void craftMaterial(int level);   // 执行3合1合成
    //辅助功能
    void showMaterials();            // 显示当前材料库存
    void showRules();                // 显示强化规则表
};

#endif // FORGE_MANAGER_H