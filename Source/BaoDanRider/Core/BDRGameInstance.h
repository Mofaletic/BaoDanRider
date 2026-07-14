#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/BDRTypes.h"
#include "BDRGameInstance.generated.h"

UCLASS()
class BAODANRIDER_API UBDRGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintCallable)
    void LoadProfile();

    UFUNCTION(BlueprintCallable)
    void SaveProfile() const;

    UFUNCTION(BlueprintCallable)
    void ResetProfile();

    UFUNCTION(BlueprintCallable)
    void SetActiveOrder(const FBDROrderData& Order);

    UFUNCTION(BlueprintPure)
    bool CanPickupActiveOrder() const;

    UFUNCTION(BlueprintCallable)
    bool TryMarkActiveOrderPickedUp();

    UFUNCTION(BlueprintPure)
    bool CanCompleteActiveOrder() const;

    UFUNCTION(BlueprintCallable)
    void ApplyRunResult(const FBDRRunResult& Result);

    UFUNCTION(BlueprintCallable)
    void EndDeliveryDay();

    UFUNCTION(BlueprintPure)
    bool HasActiveOrder() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBDRGamePhase Phase = EBDRGamePhase::Hub;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBDRPlayerProfile Profile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBDROrderData ActiveOrder;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBDRRunResult LastRunResult;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasLastRunResult = false;

#if WITH_DEV_AUTOMATION_TESTS
    void SetProfilePersistenceEnabledForTests(bool bEnabled);
#endif

private:
    static const FString SaveSlotName;
    bool bHasActiveOrder = false;
    bool bProfilePersistenceEnabled = true;
};
