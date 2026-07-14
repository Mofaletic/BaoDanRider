#include "Core/BDRUpgradeService.h"

int32 UBDRUpgradeService::GetUpgradeCost(const FBDRPlayerProfile& Profile, EBDRUpgradeType Type)
{
    const int32 Level = Profile.Upgrades.GetLevel(Type);
    switch (Type)
    {
    case EBDRUpgradeType::Speed:
        return 70 + Level * 55;
    case EBDRUpgradeType::Stability:
        return 65 + Level * 50;
    case EBDRUpgradeType::CargoCapacity:
        return 80 + Level * 60;
    case EBDRUpgradeType::Insulation:
        return 60 + Level * 45;
    case EBDRUpgradeType::AntiSpill:
        return 55 + Level * 40;
    default:
        return 9999;
    }
}

bool UBDRUpgradeService::TryBuyUpgrade(FBDRPlayerProfile& Profile, EBDRUpgradeType Type)
{
    const int32 Cost = GetUpgradeCost(Profile, Type);
    if (Profile.Money < Cost)
    {
        return false;
    }

    Profile.Money -= Cost;
    Profile.Upgrades.GetMutableLevel(Type) += 1;
    return true;
}

float UBDRUpgradeService::GetSpeedMultiplier(const FBDRPlayerProfile& Profile)
{
    return 1.0f + Profile.Upgrades.Speed * 0.08f;
}

float UBDRUpgradeService::GetStabilityMultiplier(const FBDRPlayerProfile& Profile)
{
    return 1.0f + Profile.Upgrades.Stability * 0.12f;
}
