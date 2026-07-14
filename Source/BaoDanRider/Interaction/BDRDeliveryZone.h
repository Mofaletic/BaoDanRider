#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BDRTypes.h"
#include "BDRDeliveryZone.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class UStaticMeshComponent;
class ABDRRiderPawn;

UCLASS()
class BAODANRIDER_API ABDRDeliveryZone : public AActor
{
    GENERATED_BODY()

public:
    ABDRDeliveryZone();

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    FBDRRunResult CompleteDelivery(ABDRRiderPawn* Rider, const FBDROrderData& Order, EBDRDeliveryMethod Method, float RemainingSeconds);

    UFUNCTION(BlueprintCallable)
    void SetObjectiveActive(bool bActive);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> Trigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> MarkerPad;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Beacon;

private:
    UFUNCTION()
    void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void UpdateMarkerMaterial();
    bool bObjectiveActive = false;
};
