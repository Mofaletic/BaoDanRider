#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BDRTypes.h"
#include "BDRCargoGridComponent.generated.h"

UCLASS(ClassGroup=(BaoDanRider), meta=(BlueprintSpawnableComponent))
class BAODANRIDER_API UBDRCargoGridComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBDRCargoGridComponent();

    UFUNCTION(BlueprintCallable)
    bool TryPlaceItem(const FBDRCargoItem& Item, int32 X, int32 Y);

    UFUNCTION(BlueprintCallable)
    void ClearCargo();

    UFUNCTION(BlueprintCallable)
    void AutoFillStarterMeal();

    UFUNCTION(BlueprintCallable)
    void ApplyShake(float Strength, int32 AntiSpillLevel);

    UFUNCTION(BlueprintCallable)
    void TickTemperature(float DeltaSeconds, int32 InsulationLevel);

    UFUNCTION(BlueprintPure)
    bool CanPlaceItem(const FBDRCargoItem& Item, int32 X, int32 Y) const;

    UFUNCTION(BlueprintPure)
    float GetIntegrity() const;

    UFUNCTION(BlueprintPure)
    float GetTemperatureAverage() const;

    UFUNCTION(BlueprintPure)
    int32 GetOccupiedCellCount() const;

    UFUNCTION(BlueprintPure)
    int32 GetCapacityCellCount() const;

    UFUNCTION(BlueprintPure)
    FString GetCellLabel(int32 X, int32 Y) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Width = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Height = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FBDRCargoItem> Items;

private:
    TArray<int32> Cells;

    int32 CellIndex(int32 X, int32 Y) const;
    void EnsureGrid();
};
