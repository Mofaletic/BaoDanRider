#include "World/BDRVisualBenchmark.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

namespace BDRVisualAssets
{
    constexpr TCHAR Shop[] = TEXT("/Game/AnimeCitySuburbs/Models/Shop01/SM_Shop01.SM_Shop01");
    constexpr TCHAR Residence[] = TEXT("/Game/AnimeCitySuburbs/Models/SM_House02.SM_House02");
    constexpr TCHAR NoodleSign[] = TEXT("/Game/AnimeCitySuburbs/Models/Props/SM_signNoodles.SM_signNoodles");
    constexpr TCHAR FoodStand[] = TEXT("/Game/AnimeCitySuburbs/Models/Props/SM_fastfoodStand01.SM_fastfoodStand01");
    constexpr TCHAR Sidewalk[] = TEXT("/Game/AnimeCitySuburbs/Models/Road/SM_Sidewalk01.SM_Sidewalk01");
    constexpr TCHAR StreetLamp[] = TEXT("/Game/AnimeCitySuburbs/Models/Props/SM_Lamp2.SM_Lamp2");
    constexpr TCHAR TrashCan[] = TEXT("/Game/AnimeCitySuburbs/Models/Shop03/SM_TrashCan.SM_TrashCan");
}

ABDRVisualBenchmark::ABDRVisualBenchmark()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    ApprovedFootprintCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ApprovedFootprintCollision"));
    ApprovedFootprintCollision->SetupAttachment(Root);
    ApprovedFootprintCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
    ApprovedFootprintCollision->SetBoxExtent(FVector(300.0f, 180.0f, 150.0f));
    ApprovedFootprintCollision->SetCollisionProfileName(TEXT("BlockAll"));
    ApprovedFootprintCollision->SetHiddenInGame(true);

    ResidenceFootprintCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ResidenceFootprintCollision"));
    ResidenceFootprintCollision->SetupAttachment(Root);
    ResidenceFootprintCollision->SetRelativeLocation(FVector(0.0f, 420.0f, 220.0f));
    ResidenceFootprintCollision->SetBoxExtent(FVector(300.0f, 180.0f, 220.0f));
    ResidenceFootprintCollision->SetCollisionProfileName(TEXT("BlockAll"));
    ResidenceFootprintCollision->SetHiddenInGame(true);

    Shop = CreateVisualMesh(TEXT("Shop"));
    Residence = CreateVisualMesh(TEXT("Residence"));
    NoodleSign = CreateVisualMesh(TEXT("NoodleSign"));
    FoodStand = CreateVisualMesh(TEXT("FoodStand"));
    SidewalkSouth = CreateVisualMesh(TEXT("SidewalkSouth"));
    SidewalkNorth = CreateVisualMesh(TEXT("SidewalkNorth"));
    StreetLamp = CreateVisualMesh(TEXT("StreetLamp"));
    TrashCan = CreateVisualMesh(TEXT("TrashCan"));

    UStaticMesh* ShopAsset = LoadObject<UStaticMesh>(nullptr, BDRVisualAssets::Shop);
    UStaticMesh* ResidenceAsset = LoadObject<UStaticMesh>(nullptr, BDRVisualAssets::Residence);
    UStaticMesh* NoodleSignAsset = LoadObject<UStaticMesh>(nullptr, BDRVisualAssets::NoodleSign);
    UStaticMesh* FoodStandAsset = LoadObject<UStaticMesh>(nullptr, BDRVisualAssets::FoodStand);
    UStaticMesh* SidewalkAsset = LoadObject<UStaticMesh>(nullptr, BDRVisualAssets::Sidewalk);
    UStaticMesh* StreetLampAsset = LoadObject<UStaticMesh>(nullptr, BDRVisualAssets::StreetLamp);
    UStaticMesh* TrashCanAsset = LoadObject<UStaticMesh>(nullptr, BDRVisualAssets::TrashCan);

    // Every transform is derived from the audited source bounds and the approved 600 x 360 cm greybox lot.
    PlaceMeshOnGround(Shop, ShopAsset, FVector::ZeroVector, 0.0f, FVector(0.78f));
    PlaceMeshOnGround(Residence, ResidenceAsset, FVector(0.0f, 420.0f, 0.0f), 0.0f, FVector(0.37f));
    PlaceMeshAtCenter(NoodleSign, NoodleSignAsset, FVector(-360.0f, 0.0f, 150.0f), FVector(0.46f), FRotator(0.0f, 90.0f, 0.0f));
    PlaceMeshOnGround(FoodStand, FoodStandAsset, FVector(-430.0f, 90.0f, 0.0f), 0.0f, FVector(0.72f), FRotator(0.0f, -90.0f, 0.0f));
    PlaceMeshOnGround(SidewalkSouth, SidewalkAsset, FVector(-500.0f, -150.0f, 0.0f), 0.0f, FVector(0.80f, 1.50f, 1.0f));
    PlaceMeshOnGround(SidewalkNorth, SidewalkAsset, FVector(-500.0f, 150.0f, 0.0f), 0.0f, FVector(0.80f, 1.50f, 1.0f));
    PlaceMeshOnGround(StreetLamp, StreetLampAsset, FVector(-500.0f, -150.0f, 0.0f), 24.0f, FVector(0.86f));
    PlaceMeshOnGround(TrashCan, TrashCanAsset, FVector(-500.0f, 155.0f, 0.0f), 24.0f, FVector(1.0f));
}

UStaticMeshComponent* ABDRVisualBenchmark::CreateVisualMesh(const FName& Name)
{
    UStaticMeshComponent* Component = CreateDefaultSubobject<UStaticMeshComponent>(Name);
    Component->SetupAttachment(Root);
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetGenerateOverlapEvents(false);
    Component->CastShadow = true;
    return Component;
}

void ABDRVisualBenchmark::PlaceMeshOnGround(UStaticMeshComponent* Component, UStaticMesh* Mesh, const FVector& BoundsCenter, float GroundZ, const FVector& Scale, const FRotator& Rotation)
{
    if (!Component || !Mesh)
    {
        return;
    }

    Component->SetStaticMesh(Mesh);
    Component->SetRelativeScale3D(Scale);
    Component->SetRelativeRotation(Rotation);

    const FTransform MeshTransform(Rotation, FVector::ZeroVector, Scale);
    const FBoxSphereBounds TransformedBounds = Mesh->GetBounds().TransformBy(MeshTransform);
    const float BoundsBottom = TransformedBounds.Origin.Z - TransformedBounds.BoxExtent.Z;
    Component->SetRelativeLocation(FVector(
        BoundsCenter.X - TransformedBounds.Origin.X,
        BoundsCenter.Y - TransformedBounds.Origin.Y,
        GroundZ - BoundsBottom));
}

void ABDRVisualBenchmark::PlaceMeshAtCenter(UStaticMeshComponent* Component, UStaticMesh* Mesh, const FVector& BoundsCenter, const FVector& Scale, const FRotator& Rotation)
{
    if (!Component || !Mesh)
    {
        return;
    }

    Component->SetStaticMesh(Mesh);
    Component->SetRelativeScale3D(Scale);
    Component->SetRelativeRotation(Rotation);

    const FTransform MeshTransform(Rotation, FVector::ZeroVector, Scale);
    const FBoxSphereBounds TransformedBounds = Mesh->GetBounds().TransformBy(MeshTransform);
    Component->SetRelativeLocation(BoundsCenter - TransformedBounds.Origin);
}
