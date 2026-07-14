#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BDRPrototypeBlockout.generated.h"

class ABDRDeliveryZone;
class ABDRHazardBase;
class ABDRPickupZone;
class ABDRVisualBenchmark;
class UChildActorComponent;
class UStaticMeshComponent;

UCLASS()
class BAODANRIDER_API ABDRPrototypeBlockout : public AActor
{
    GENERATED_BODY()

public:
    ABDRPrototypeBlockout();

    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox")
    bool bBuildPreview = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox")
    bool bSpawnGameplayZones = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox")
    bool bSpawnHazardTriggers = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual Benchmark")
    bool bSpawnVisualBenchmark = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ClampMin="2", ClampMax="12"))
    int32 BuildingPairs = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ClampMin="2400.0", UIMin="2400.0", ForceUnits="cm"))
    float BlockLength = 4200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ClampMin="2600.0", UIMin="2600.0", ForceUnits="cm"))
    float BlockWidth = 3400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ForceUnits="cm"))
    float StartRoadCenterY = -800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ClampMin="500.0", UIMin="500.0", ForceUnits="cm"))
    float RoadWidth = 720.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ClampMin="160.0", UIMin="160.0", ForceUnits="cm"))
    float SidewalkWidth = 240.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ClampMin="320.0", ClampMax="560.0", UIMin="320.0", UIMax="560.0", ForceUnits="cm"))
    float ShortcutWidth = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Greybox", meta=(ClampMin="240.0", UIMin="240.0", ForceUnits="cm"))
    float FloorHeight = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
    TSubclassOf<ABDRPickupZone> PickupZoneClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
    TSubclassOf<ABDRDeliveryZone> DeliveryZoneClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
    TSubclassOf<ABDRHazardBase> HazardClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual Benchmark")
    TSubclassOf<ABDRVisualBenchmark> VisualBenchmarkClass;

private:
    UStaticMeshComponent* AddBox(const FString& Name, const FVector& Center, const FVector& Dimensions, const FLinearColor& Color, bool bEnableCollision = true);
    void AddRoadMarking(const FString& Name, const FVector& Center, const FVector& Dimensions, const FLinearColor& Color);
    void AddGreyboxBuilding(const FString& Name, const FVector& GroundCenter, float Width, float Depth, int32 FloorCount, bool bFacesPositiveX, const FLinearColor& Color, const FLinearColor& AccentColor);
    UChildActorComponent* AddChildActor(const FString& Name, TSubclassOf<AActor> ActorClass, const FVector& Location);
    void ClearPreview();
};
