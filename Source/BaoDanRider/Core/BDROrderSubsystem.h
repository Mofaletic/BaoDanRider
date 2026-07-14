#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/BDRTypes.h"
#include "BDROrderSubsystem.generated.h"

UCLASS()
class BAODANRIDER_API UBDROrderSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    FBDROrderData GenerateOrder(int32 PlayerLevel) const;

    UFUNCTION(BlueprintCallable)
    void StartOrderTimer(const FBDROrderData& Order);

    UFUNCTION(BlueprintCallable)
    void PrepareOrderTimer(const FBDROrderData& Order);

    UFUNCTION(BlueprintCallable)
    void TickOrder(float DeltaSeconds);

    UFUNCTION(BlueprintCallable)
    void SetRemainingSeconds(float NewRemainingSeconds);

    UFUNCTION(BlueprintCallable)
    void StopOrderTimer();

    UFUNCTION(BlueprintPure)
    float GetRemainingSeconds() const;

    UFUNCTION(BlueprintPure)
    bool IsOrderExpired() const;

    UFUNCTION(BlueprintPure)
    bool IsOrderTimerRunning() const;

private:
    float RemainingSeconds = 0.0f;
    bool bTimerRunning = false;
};
