#pragma once

#include "CoreMinimal.h"
#include "BDRTypes.generated.h"

UENUM(BlueprintType)
enum class EBDRGamePhase : uint8
{
    Hub,
    Pickup,
    Packing,
    Riding,
    Delivery,
    Settlement
};

UENUM(BlueprintType)
enum class EBDRPickupEvent : uint8
{
    Normal,
    CookingNow,
    DeliverOtherFirst
};

UENUM(BlueprintType)
enum class EBDRDeliveryMethod : uint8
{
    FaceToFace,
    DoorPhoto
};

UENUM(BlueprintType)
enum class EBDRHazardType : uint8
{
    RedLightTraffic,
    Roadblock,
    FoodSpill,
    CleanerChase,
    FoodSnatcher
};

UENUM(BlueprintType)
enum class EBDRUpgradeType : uint8
{
    Speed,
    Stability,
    CargoCapacity,
    Insulation,
    AntiSpill
};

USTRUCT(BlueprintType)
struct FBDRUpgradeLevels
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Speed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Stability = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CargoCapacity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Insulation = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AntiSpill = 0;

    int32 GetLevel(EBDRUpgradeType Type) const;
    int32& GetMutableLevel(EBDRUpgradeType Type);
};

USTRUCT(BlueprintType)
struct FBDRPlayerProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Nickname = TEXT("爆单骑手");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Experience = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Money = 80;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Stamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBDRUpgradeLevels Upgrades;

    int32 GetNextLevelExperience() const;
};

USTRUCT(BlueprintType)
struct FBDROrderData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString OrderId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RestaurantName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CustomerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CustomerAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeLimitSeconds = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BaseReward = 45;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DistanceMeters = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString RiskTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBDRPickupEvent PickupEvent = EBDRPickupEvent::Normal;
};

USTRUCT(BlueprintType)
struct FBDRCargoItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name = TEXT("餐品");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FIntPoint Size = FIntPoint(1, 1);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Temperature = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Stability = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFragile = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSpillable = false;
};

USTRUCT(BlueprintType)
struct FBDRRunResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSuccess = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Reward = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Experience = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Rating = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RemainingSeconds = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FoodIntegrity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Note;
};
