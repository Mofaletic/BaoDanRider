#include "Hazards/BDRHazardBase.h"
#include "Components/BoxComponent.h"
#include "Player/BDRRiderPawn.h"

ABDRHazardBase::ABDRHazardBase()
{
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    RootComponent = Trigger;
    Trigger->SetBoxExtent(FVector(110.0f, 110.0f, 80.0f));
    Trigger->SetCollisionProfileName(TEXT("Trigger"));
}

void ABDRHazardBase::BeginPlay()
{
    Super::BeginPlay();
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABDRHazardBase::HandleOverlap);
}

void ABDRHazardBase::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ABDRRiderPawn* Rider = Cast<ABDRRiderPawn>(OtherActor))
    {
        Rider->ApplyHazardHit(BalanceDamage, SpeedPenalty, CargoShake);
    }
}
