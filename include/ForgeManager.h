#ifndef FORGE_MANAGER_H
#define FORGE_MANAGER_H
#include <vector>
#include <string>
class Player;
class Item;
struct EnchantLevelConfig
{
    int from, to;
    std::wstring matName;
    int matCount, goldCost, atkBonus, defBonus;
};
class ForgeManager
{
private:
    Player* m_player;
    std::vector<EnchantLevelConfig> m_cfg;
    void initConfig();
    bool checkRes(const EnchantLevelConfig& cfg);
    void consumeRes(const EnchantLevelConfig& cfg);
public:
    ForgeManager(Player* p);
    ~ForgeManager() = default;
    std::wstring getMaterialText() const;
    std::wstring getRuleText() const;
    std::vector<Item*> getEnhanceable() const;
    std::wstring enhancePreview(Item* item);
    std::wstring doEnhance(Item* item);
    std::wstring craft(int lvl);
    std::vector<Item*> getEquippableAll() const;
};
#endif
