#include "Core/BDRTypes.h"

int32 FBDRUpgradeLevels::GetLevel(EBDRUpgradeType Type) const
{
    switch (Type)
    {
    case EBDRUpgradeType::Speed:
        return Speed;
    case EBDRUpgradeType::Stability:
        return Stability;
    case EBDRUpgradeType::CargoCapacity:
        return CargoCapacity;
    case EBDRUpgradeType::Insulation:
        return Insulation;
    case EBDRUpgradeType::AntiSpill:
        return AntiSpill;
    default:
        return 0;
    }
}

int32& FBDRUpgradeLevels::GetMutableLevel(EBDRUpgradeType Type)
{
    switch (Type)
    {
    case EBDRUpgradeType::Speed:
        return Speed;
    case EBDRUpgradeType::Stability:
        return Stability;
    case EBDRUpgradeType::CargoCapacity:
        return CargoCapacity;
    case EBDRUpgradeType::Insulation:
        return Insulation;
    case EBDRUpgradeType::AntiSpill:
        return AntiSpill;
    default:
        return Speed;
    }
}

int32 FBDRPlayerProfile::GetNextLevelExperience() const
{
    return 100 + FMath::Max(0, Level - 1) * 65;
}
