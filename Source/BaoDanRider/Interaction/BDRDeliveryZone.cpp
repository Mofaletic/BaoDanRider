#include "Interaction/BDRDeliveryZone.h"

#include "Cargo/BDRCargoGridComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/BDRDeliveryGameMode.h"
#include "Core/BDRSettlementService.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/BDRRiderPawn.h"
#include "UObject/ConstructorHelpers.h"

ABDRDeliveryZone::ABDRDeliveryZone()
{
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    RootComponent = Trigger;
    Trigger->SetBoxExtent(FVector(180.0f, 180.0f, 120.0f));
    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Trigger->SetGenerateOverlapEvents(true);

    MarkerPad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerPad"));
    MarkerPad->SetupAttachment(Trigger);
    MarkerPad->SetRelativeLocation(FVector(0.0f, 0.0f, -116.0f));
    MarkerPad->SetRelativeScale3D(FVector(3.2f, 3.2f, 0.08f));
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

void ABDRDeliveryZone::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    UpdateMarkerMaterial();
}

void ABDRDeliveryZone::BeginPlay()
{
    Super::BeginPlay();
    Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABDRDeliveryZone::HandleBeginOverlap);
    UpdateMarkerMaterial();
}

FBDRRunResult ABDRDeliveryZone::CompleteDelivery(ABDRRiderPawn* Rider, const FBDROrderData& Order, EBDRDeliveryMethod Method, float RemainingSeconds)
{
    const float Integrity = Rider && Rider->CargoGrid ? Rider->CargoGrid->GetIntegrity() : 0.0f;
    const float Temperature = Rider && Rider->CargoGrid ? Rider->CargoGrid->GetTemperatureAverage() : 0.0f;
    const FString Note = Method == EBDRDeliveryMethod::FaceToFace ? TEXT("Handed to the customer.") : TEXT("Left at the door with a photo.");
    return UBDRSettlementService::CalculateSettlement(Order, true, Method, RemainingSeconds, Integrity, Temperature, Note);
}

void ABDRDeliveryZone::SetObjectiveActive(bool bActive)
{
    bObjectiveActive = bActive;
    UpdateMarkerMaterial();
}

void ABDRDeliveryZone::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ABDRRiderPawn* Rider = Cast<ABDRRiderPawn>(OtherActor))
    {
        if (ABDRDeliveryGameMode* DeliveryMode = Cast<ABDRDeliveryGameMode>(UGameplayStatics::GetGameMode(this)))
        {
            DeliveryMode->HandleDeliveryReached(this, Rider);
        }
    }
}

void ABDRDeliveryZone::UpdateMarkerMaterial()
{
    const FLinearColor Color = bObjectiveActive ? FLinearColor(1.0f, 0.28f, 0.03f) : FLinearColor(0.20f, 0.12f, 0.08f);
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
