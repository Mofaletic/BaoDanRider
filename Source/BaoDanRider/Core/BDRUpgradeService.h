#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Core/BDRTypes.h"
#include "BDRUpgradeService.generated.h"

UCLASS(BlueprintType)
class BAODANRIDER_API UBDRUpgradeService : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    static int32 GetUpgradeCost(const FBDRPlayerProfile& Profile, EBDRUpgradeType Type);

    UFUNCTION(BlueprintCallable)
    static bool TryBuyUpgrade(UPARAM(ref) FBDRPlayerProfile& Profile, EBDRUpgradeType Type);

    UFUNCTION(BlueprintPure)
    static float GetSpeedMultiplier(const FBDRPlayerProfile& Profile);

    UFUNCTION(BlueprintPure)
    static float GetStabilityMultiplier(const FBDRPlayerProfile& Profile);
};
