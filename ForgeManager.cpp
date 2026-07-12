#include "ForgeManager.h"
#include "Player.h"
#include "Bag.h"
#include "Item.h"
#include <iostream>
#include <iomanip>

ForgeManager::ForgeManager(Player* player) : m_player(player) {initConfig();}
/**
 * 初始化强化配置表
 * 
 * 10级强化规则：
 *   等级0→1:  普通材料×3 + 100金 → 武器+2, 防具+2
 *   等级1→2:  普通材料×4 + 200金 → 武器+2, 防具+2
 *   等级2→3:  普通材料×5 + 300金 → 武器+3, 防具+2
 *   等级3→4:  稀有材料×2 + 500金 → 武器+3, 防具+2
 *   等级4→5:  稀有材料×3 + 700金 → 武器+4, 防具+2
 *   等级5→6:  稀有材料×4 + 900金 → 武器+4, 防具+2
 *   等级6→7:  罕见材料×2 + 1200金 → 武器+5, 防具+2
 *   等级7→8:  罕见材料×3 + 1500金 → 武器+5, 防具+2
 *   等级8→9:  史诗材料×2 + 2000金 → 武器+6, 防具+2
 *   等级9→10: 传说材料×1 + 3000金 → 武器+10, 防具+2
 * 
 * 设计说明：
 *   - 材料需求逐级提升品质
 *   - 金币需求逐级增加
 *   - 武器攻击增量逐级提升（鼓励追求高强化）
 *   - 防具防御增量固定+2（简化设计）
 */
void ForgeManager::initConfig() 
{
    m_config = {
        // fromLevel, toLevel, materialName, count, gold, attackBonus, defenseBonus
        {0, 1, "普通材料", 3, 100, 2, 2},
        {1, 2, "普通材料", 4, 200, 2, 2},
        {2, 3, "普通材料", 5, 300, 3, 2},
        {3, 4, "稀有材料", 2, 500, 3, 2},
        {4, 5, "稀有材料", 3, 700, 4, 2},
        {5, 6, "稀有材料", 4, 900, 4, 2},
        {6, 7, "罕见材料", 2, 1200, 5, 2},
        {7, 8, "罕见材料", 3, 1500, 5, 2},
        {8, 9, "史诗材料", 2, 2000, 6, 2},
        {9, 10, "传说材料", 1, 3000, 10, 2}
    };
}
// 主菜单
void ForgeManager::showMenu() 
{
    while (true) 
    {
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║                       锻造工坊                         ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
        // 显示材料库存
        showMaterials();
        std::cout << "\n─────────────────────────────────────────────────────────\n";
        std::cout << "  1. 强化装备\n";
        std::cout << "  2. 材料合成（3合1升级）\n";
        std::cout << "  3. 查看强化规则\n";
        std::cout << "  4. 返回主菜单\n";
        std::cout << "─────────────────────────────────────────────────────────\n";
        std::cout << "请选择：";

        int choice;
        std::cin >> choice;          ///////////////////////////////////////////////////////////////////////
        switch (choice) 
        {
            case 1:
                showEnhanceMenu();
                break;
            case 2:
                showCraftMenu();
                break;
            case 3:
                showRules();
                break;
            case 4:
                std::cout << " 离开锻造工坊\n";
                return;
            default:
                std::cout << " 无效输入，请重新选择！\n";
                break;
        }
    }
}
// 显示材料库存
void ForgeManager::showMaterials() 
{
    Bag* bag = m_player->getBag();
    if (!bag) return;
    std::cout << "\n 当前材料库存：\n";
    // 从背包获取所有材料
    std::vector<Material*> materials = bag->getMaterials();
    // 5种材料的名称
    std::vector<std::string> materialNames = {"普通材料", "稀有材料", "罕见材料", "史诗材料", "传说材料"};
    // 统计各等级材料数量（索引0-4对应5个稀有度）
    int counts[5] = {0, 0, 0, 0, 0};
    for (Material* mat : materials) 
    {
        int level = static_cast<int>(mat->getRarity());
        if (level >= 0 && level < 5) 
        {
            counts[level] += mat->getQuantity();
        }
    }
    // 显示统计结果
    for (int i = 0; i < 5; ++i) 
    {
        std::cout << "    " << materialNames[i] << " × " << counts[i] << "\n";
    }
}
// 强化装备
void ForgeManager::showEnhanceMenu() 
{
    Bag* bag = m_player->getBag();
    if (!bag) return;
    // 收集所有可强化装备（武器和防具）
    std::vector<Weapon*> weapons = bag->getWeapons();
    std::vector<Armor*> armors = bag->getArmors();
    std::vector<Item*> enchantableItems;
    for (Weapon* w : weapons) enchantableItems.push_back(w);
    for (Armor* a : armors) enchantableItems.push_back(a);
    // 检查是否有可强化装备
    if (enchantableItems.empty()) 
    {
        std::cout << "\n 背包中没有可强化的装备！\n";
        return;
    }
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "  可强化装备列表\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    // 显示所有可强化装备
    for (size_t i = 0; i < enchantableItems.size(); ++i) 
    {
        Item* item = enchantableItems[i];
        std::cout << "  " << (i + 1) << ". ";
        item->display(); 
        // 显示下一级强化信息
        int currentLevel = item->getEnhanceLevel();
        if (currentLevel < 10) 
        {
            const auto& config = m_config[currentLevel];
            std::cout << "      → 下一级: +" << config.toLevel 
                      << " (需要 " << config.materialName << " ×" << config.materialCount 
                      << " + " << config.goldCost << "金)\n";
        } 
        else 
        {
            std::cout << "      → ★ 已达最大强化等级（+10）！\n";
        }
    }
    std::cout << "─────────────────────────────────────────────────────────\n";
    std::cout << "请选择要强化的装备编号（输入0返回）：";

    int choice;
    std::cin >> choice;                           ////////////////////////////////////////////////////////////

    if (choice == 0) return;
    if (choice < 1 || choice > static_cast<int>(enchantableItems.size()))
    {
        std::cout << " 无效选择！\n";
        return;
    }
    // 执行强化
    enhanceItem(enchantableItems[choice - 1]);
}

//执行装备强化
void ForgeManager::enhanceItem(Item* item) 
{
    int currentLevel = item->getEnhanceLevel();
    if (!item || !item->isEnhanceable()||currentLevel >= 10) 
    {
        std::cout << " 该物品不可强化！\n";
        return;
    }
    const auto& config = m_config[currentLevel];
    // ----- 显示强化预览 -----
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "  强化预览\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    
    // 显示装备当前信息
    std::cout << "  装备: ";
    item->display();
    std::cout << "\n  当前等级: +" << currentLevel << "\n";
    std::cout << "  目标等级: +" << config.toLevel << "\n";

    // 计算并显示属性变化
    if (item->getType() == Item::WEAPON) 
    {
        Weapon* weapon = static_cast<Weapon*>(item);
        int currentDamage = weapon->getDamage();
        // 临时设置目标等级计算新攻击力
        int oldLevel = weapon->getEnhanceLevel();
        weapon->setEnhanceLevel(config.toLevel);
        int newDamage = weapon->getDamage();
        weapon->setEnhanceLevel(oldLevel);
        std::cout << "  攻击力: " << currentDamage << " → " << newDamage << " (+" << (newDamage - currentDamage) << ")\n";
    } 
    else if (item->getType() == Item::ARMOR) 
    {
        Armor* armor = static_cast<Armor*>(item);
        int currentDefense = armor->getDefense();
        int oldLevel = armor->getEnhanceLevel();
        armor->setEnhanceLevel(config.toLevel);
        int newDefense = armor->getDefense();
        armor->setEnhanceLevel(oldLevel);
        std::cout << "  防御力: " << currentDefense << " → " << newDefense << " (+" << (newDefense - currentDefense) << ")\n";
    }

    // 显示消耗
    std::cout << "\n  需要材料: " << config.materialName << " ×" << config.materialCount << "\n";
    std::cout << "  需要金币: " << config.goldCost << "\n";
    // ----- 检查资源 -----
    if (!checkEnchantResources(config)) 
    {
        std::cout << "\n资源不足，无法强化！\n";
        return;
    }
    // ----- 确认强化 -----
    std::cout << "\n是否强化？(y/n)：";
    char confirm;
    std::cin >> confirm;                      /////////////////////////////////////////////////////////////

    if (confirm != 'y' && confirm != 'Y') 
    {
        std::cout << "已取消强化。\n";
        return;
    }
    // ----- 执行强化 -----
    consumeEnchantResources(config);
    item->setEnhanceLevel(config.toLevel);

    // ----- 显示成功信息 -----
    std::cout << "\n 强化成功！\n";
    item->display();
    std::cout << "\n";
}

//资源检查与消耗
bool ForgeManager::checkEnchantResources(const EnchantLevelConfig& config) 
{
    Bag* bag = m_player->getBag();
    int gold = m_player->getGold();
    // 检查金币
    if (gold < config.goldCost) 
    {
        std::cout << "  金币不足！需要 " << config.goldCost << "，当前 " << gold << "\n";
        return false;
    }
    // 检查材料
    int materialCount = 0;
    std::vector<Material*> materials = bag->getMaterials();
    for (Material* mat : materials) 
    {
        if (mat->getName() == config.materialName) 
        {
            materialCount += mat->getQuantity();
        }
    }

    if (materialCount < config.materialCount) 
    {
        std::cout << "  材料不足！需要 " << config.materialName << " ×" << config.materialCount << "，当前 " << materialCount << "\n";
        return false;
    }
    return true;
}

//消耗强化资源
void ForgeManager::consumeEnchantResources(const EnchantLevelConfig& config) 
{
    Bag* bag = m_player->getBag();
    // 消耗金币
    m_player->addGold(-config.goldCost);
    // 消耗材料
    int remaining = config.materialCount;
    std::vector<Material*> materials = bag->getMaterials();
    for (Material* mat : materials) 
    {
        if (mat->getName() == config.materialName) 
        {
            int currentQty = mat->getQuantity();
            int toRemove = std::min(remaining, currentQty);
            if (toRemove > 0) 
            {
                bag->removeItem(mat->getName(), toRemove);
                remaining -= toRemove;
                if (remaining <= 0) break;
            }
        }
    }
}

// 材料合成
void ForgeManager::showCraftMenu() 
{
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "   材料合成（3合1升级）\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    // 显示当前材料库存
    showMaterials();
    std::cout << "\n请选择要升级的材料等级：\n";
    std::cout << "  1. 普通材料 × 3 → 稀有材料 × 1\n";
    std::cout << "  2. 稀有材料 × 3 → 罕见材料 × 1\n";
    std::cout << "  3. 罕见材料 × 3 → 史诗材料 × 1\n";
    std::cout << "  4. 史诗材料 × 3 → 传说材料 × 1\n";
    std::cout << "  0. 返回\n";
    std::cout << "请选择：";

    int choice;
    std::cin >> choice;                     //////////////////////////////////////////////////////
    if (choice == 0) return;
    if (choice < 1 || choice > 4) 
    {
        std::cout << " 无效选择！\n";
        return;
    }
    // 调用合成方法，参数0-3对应4种合成
    craftMaterial(choice - 1);
}
//执行材料合成
void ForgeManager::craftMaterial(int level) 
{
    // 5种材料名称
    std::vector<std::string> materialNames = {"普通材料", "稀有材料", "罕见材料", "史诗材料", "传说材料"};
    // 检查是否已达最高等级
    if (level >= 4) 
    {
        std::cout << " 传说材料已是最高等级，无法继续合成！\n";
        return;
    }
    Bag* bag = m_player->getBag();
    std::string sourceName = materialNames[level];
    std::string targetName = materialNames[level + 1];
    // 检查源材料数量
    int sourceCount = 0;
    std::vector<Material*> materials = bag->getMaterials();
    for (Material* mat : materials) 
    {
        if (mat->getName() == sourceName) 
        {
            sourceCount += mat->getQuantity();
        }
    }
    if (sourceCount < 3) 
    {
        std::cout << " " << sourceName << " 数量不足！需要 3 个，当前 " << sourceCount << "\n";
        return;
    }
    // 确认合成
    std::cout << "合成 " << sourceName << " × 3 → " << targetName << " × 1，是否继续？(y/n)：";
    char confirm;
    std::cin >> confirm;                                     /////////////////////////////////////////
    if (confirm != 'y' && confirm != 'Y') 
    {
        std::cout << "已取消合成。\n";
        return;
    }
    // 执行合成：消耗3个源材料
    bag->removeItem(sourceName, 3);
    // 查找目标材料是否已存在
    bool found = false;
    std::vector<Material*> targetMaterials = bag->getMaterials();
    for (Material* mat : targetMaterials) 
    {
        if (mat->getName() == targetName) 
        {
            mat->addQuantity(1);
            found = true;
            break;
        }
    }
    // 如果目标材料不存在，创建新的
    if (!found) 
    {
        // level+1 对应目标材料的稀有度
        Material::MaterialRarity rarity = static_cast<Material::MaterialRarity>(level + 1);
        Material* newMaterial = new Material(targetName, 1, rarity);
        bag->addItem(newMaterial);
    }
    std::cout << " 合成成功！获得 " << targetName << " × 1\n";
}

// 查看强化规则
void ForgeManager::showRules() 
{
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "   强化规则表\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    
    std::cout << std::left;
    std::cout << "  " << std::setw(12) << "当前→目标" << std::setw(14) << "消耗材料" << std::setw(10) << "数量" << std::setw(12) << "消耗金币" << std::setw(12) << "武器加成" << "防具加成\n";
    std::cout << "  ───────────────────────────────────────────────────────\n";
    for (const auto& config : m_config) 
    {
        std::cout << "  +" << config.fromLevel << "→+" << std::setw(7) << config.toLevel
                  << std::setw(14) << config.materialName
                  << "×" << std::setw(8) << config.materialCount
                  << std::setw(12) << config.goldCost
                  << "+" << std::setw(10) << config.attackBonus
                  << "+" << config.defenseBonus << "\n";
    }
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << " 强化说明：\n";
    std::cout << "   · 武器每级强化增加攻击力（不同等级增量不同）\n";
    std::cout << "   · 防具每级强化固定 +2 防御\n";
    std::cout << "   · 强化上限为 +10\n";
    std::cout << "   · 强化100%成功，无需担心失败\n";
    std::cout << "\n按回车键返回...";
    std::cin.ignore();                                        //////////////////////////
    std::cin.get();                                             ////////////////////////////////////////
}