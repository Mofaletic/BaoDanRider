#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Core/BDRTypes.h"
#include "BDRDataRows.generated.h"

USTRUCT(BlueprintType)
struct FBDRCargoItemRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FIntPoint Size = FIntPoint(1, 1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFragile = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSpillable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseTemperature = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseStability = 1.0f;
};

USTRUCT(BlueprintType)
struct FBDRHazardRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBDRHazardType Type = EBDRHazardType::Roadblock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BalanceDamage = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpeedPenalty = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CargoShake = 0.12f;
};

USTRUCT(BlueprintType)
struct FBDRUpgradeRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBDRUpgradeType Type = EBDRUpgradeType::Speed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BaseCost = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CostStep = 40;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float EffectPerLevel = 0.0f;
};
