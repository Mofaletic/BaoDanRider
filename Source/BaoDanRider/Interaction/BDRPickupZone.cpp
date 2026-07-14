#include "Interaction/BDRPickupZone.h"

#include "Cargo/BDRCargoGridComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/BDRDeliveryGameMode.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/BDRRiderPawn.h"
#include "UObject/ConstructorHelpers.h"

ABDRPickupZone::ABDRPickupZone()
{
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    RootComponent = Trigger;
    Trigger->SetBoxExtent(FVector(110.0f, 140.0f, 120.0f));
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Trigger->SetGenerateOverlapEvents(true);

    MarkerPad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerPad"));
    MarkerPad->SetupAttachment(Trigger);
    MarkerPad->SetRelativeLocation(FVector(0.0f, 0.0f, -80.0f));
    MarkerPad->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.08f));
    MarkerPad->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Beacon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beacon"));
    Beacon->SetupAttachment(Trigger);
    Beacon->SetRelativeLocation(FVector(0.0f, 0.0f, 30.0f));
    Beacon->SetRelativeScale3D(FVector(0.10f, 0.10f, 3.0f));
    Beacon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MarkerMaterial(TEXT("/Game/Materials/M_BDR_Greybox.M_BDR_Greybox"));
    if (CylinderMesh.Succeeded())
    {
        MarkerPad->SetStaticMesh(CylinderMesh.Object);
    }
    if (CubeMesh.Succeeded())
    {
        Beacon->SetStaticMesh(CubeMesh.Object);
    }
    if (MarkerMaterial.Succeeded())
    {
        MarkerPad->SetMaterial(0, MarkerMaterial.Object);
        Beacon->SetMaterial(0, MarkerMaterial.Object);
    }
}

void ABDRPickupZone::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    UpdateMarkerMaterial();
}

void ABDRPickupZone::BeginPlay()
{
    Super::BeginPlay();
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABDRPickupZone::HandleBeginOverlap);
    UpdateMarkerMaterial();
}

void ABDRPickupZone::ResolvePickup(ABDRRiderPawn* Rider, const FBDROrderData& Order, float& RemainingSeconds, FString& OutMessage)
{
    if (!Rider || !Rider->CargoGrid)
    {
        OutMessage = TEXT("Pickup failed: rider or cargo box is missing.");
        return;
    }

    switch (Order.PickupEvent)
    {
    case EBDRPickupEvent::CookingNow:
        RemainingSeconds = FMath::Max(0.0f, RemainingSeconds - 18.0f);
        OutMessage = TEXT("The restaurant is still cooking. 18 seconds lost.");
        break;
    case EBDRPickupEvent::DeliverOtherFirst:
        RemainingSeconds += 12.0f;
        OutMessage = TEXT("The restaurant grants 12 extra seconds.");
        break;
    default:
        OutMessage = TEXT("Food picked up on time.");
        break;
    }

    Rider->CargoGrid->AutoFillStarterMeal();
}

void ABDRPickupZone::SetObjectiveActive(bool bActive)
{
    bObjectiveActive = bActive;
    UpdateMarkerMaterial();
}

void ABDRPickupZone::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ABDRRiderPawn* Rider = Cast<ABDRRiderPawn>(OtherActor))
    {
        if (ABDRDeliveryGameMode* DeliveryMode = Cast<ABDRDeliveryGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            DeliveryMode->HandlePickupReached(this, Rider);
        }
    }
}

void ABDRPickupZone::UpdateMarkerMaterial()
{
    const FLinearColor Color = bObjectiveActive ? FLinearColor(0.03f, 0.65f, 1.0f) : FLinearColor(0.12f, 0.16f, 0.20f);
    const TArray<UStaticMeshComponent*> MarkerComponents = {MarkerPad.Get(), Beacon.Get()};
    for (UStaticMeshComponent* Component : MarkerComponents)
    {
        if (Component)
        {
            if (UMaterialInstanceDynamic* Material = Component->CreateAndSetMaterialInstanceDynamic(0))
            {
                Material->SetVectorParameterValue(TEXT("Color"), Color);
                Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
            }
        }
    }
}
