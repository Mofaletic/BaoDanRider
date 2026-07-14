#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/BDRTypes.h"
#include "BDRDeliveryGameMode.generated.h"

class ABDRDeliveryZone;
class ABDRPickupZone;
class ABDRRiderPawn;

UCLASS()
class BAODANRIDER_API ABDRDeliveryGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABDRDeliveryGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable)
    FBDROrderData GeneratePreviewOrder() const;

    UFUNCTION(BlueprintCallable)
    void AcceptOrder(const FBDROrderData& Order, FName DeliveryMapName = TEXT("DeliveryCity"));

    UFUNCTION(BlueprintCallable)
    void ReturnToHub(FName HubMapName = TEXT("Hub"));

    UFUNCTION(BlueprintCallable)
    void ApplySettlement(const FBDRRunResult& Result);

    UFUNCTION(BlueprintCallable)
    bool BuyUpgrade(EBDRUpgradeType Type);

    UFUNCTION(BlueprintCallable)
    void RestRider();

    UFUNCTION(BlueprintPure)
    float GetRemainingSeconds() const;

    UFUNCTION(BlueprintPure, Category="Prototype Day")
    float GetDayRemainingSeconds() const;

    UFUNCTION(BlueprintPure, Category="Prototype Day")
    int32 GetDayEarnings() const;

    UFUNCTION(BlueprintPure, Category="Prototype Day")
    int32 GetOrdersCompleted() const;

    UFUNCTION(BlueprintPure, Category="Prototype Day")
    FString GetDayRatingLetter() const;

    UFUNCTION(BlueprintPure, Category="Prototype Day")
    bool IsOrderTimerRunning() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    int32 GetBackpackOccupiedCells() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    int32 GetBackpackCapacityCells() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    bool IsRiderOnFoot() const;

    UFUNCTION(BlueprintCallable, Category="Prototype Day")
    void RestartPrototypeOrder();

    UFUNCTION(BlueprintCallable, Category="Prototype Delivery")
    void HandlePickupReached(ABDRPickupZone* Zone, ABDRRiderPawn* Rider);

    UFUNCTION(BlueprintCallable, Category="Prototype Delivery")
    void HandleDeliveryReached(ABDRDeliveryZone* Zone, ABDRRiderPawn* Rider);

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    FString GetObjectiveText() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    FString GetStatusMessage() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    FString GetTargetDirectionText() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    float GetTargetDistanceMeters() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    float GetCargoIntegrity() const;

    UFUNCTION(BlueprintPure, Category="Prototype Delivery")
    FString GetRatingLetter() const;

    UFUNCTION(BlueprintPure, Category="Prototype Day")
    bool IsAwaitingRestart() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Prototype Day")
    bool bAutoStartPrototypeDay = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Prototype Day", meta=(ClampMin="60.0", ForceUnits="s"))
    float DayDurationSeconds = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Prototype Day", meta=(ClampMin="30.0", ForceUnits="s"))
    float FixedOrderTimeLimit = 150.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Prototype Day", meta=(ClampMin="1"))
    int32 FixedOrderBaseReward = 80;

private:
    void FindGameplayActors();
    void BeginPrototypeDay(bool bResetRider);
    void BeginNextPrototypeOrder();
    void EndPrototypeDay();
    FVector GetCurrentTargetLocation() const;
    ABDRRiderPawn* ResolveRider() const;
    void UpdateZoneMarkers() const;
    static FString RatingToLetter(int32 Rating);

    TWeakObjectPtr<ABDRPickupZone> PickupZone;
    TWeakObjectPtr<ABDRDeliveryZone> DeliveryZone;
    TWeakObjectPtr<ABDRRiderPawn> ActiveRider;
    FString StatusMessage;
    float RemainingDaySeconds = 0.0f;
    int32 DayEarnings = 0;
    int32 OrdersCompleted = 0;
    int32 DayRatingTotal = 0;
    bool bDayActive = false;
    bool bDayComplete = false;
    bool bFinishActiveOrderAfterDay = false;
    bool bDayExpiredMessageShown = false;
    bool bOverdueMessageShown = false;
};
