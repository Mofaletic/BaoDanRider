#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BDRTypes.h"
#include "BDRHazardBase.generated.h"

class UBoxComponent;

UCLASS()
class BAODANRIDER_API ABDRHazardBase : public AActor
{
    GENERATED_BODY()

public:
    ABDRHazardBase();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> Trigger;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBDRHazardType Type = EBDRHazardType::Roadblock;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BalanceDamage = 18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpeedPenalty = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CargoShake = 0.12f;
};
