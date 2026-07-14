#include "World/BDRPrototypeBlockout.h"

#include "Components/ChildActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Hazards/BDRHazardBase.h"
#include "Interaction/BDRDeliveryZone.h"
#include "Interaction/BDRPickupZone.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "World/BDRVisualBenchmark.h"

namespace BDRGreybox
{
    const FName GeneratedComponentTag(TEXT("BDR_GeneratedGreybox"));
    constexpr float GroundThickness = 100.0f;
    constexpr float RoadThickness = 4.0f;
    constexpr float SidewalkHeight = 18.0f;
    constexpr float LotDepth = 820.0f;
    constexpr float BuildingDepth = 600.0f;
    constexpr float BuildingSetback = 80.0f;
    constexpr float EndClearance = 120.0f;
    constexpr float FloorJointHeight = 16.0f;
    constexpr float LaneMarkLength = 220.0f;
    constexpr float LaneMarkGap = 180.0f;
    constexpr float MarkerHalfHeight = 120.0f;
    constexpr float ShortcutBarrierHeight = 90.0f;
    constexpr float ShortcutBarrierWidth = 60.0f;
    constexpr float ShortcutBarrierLength = 100.0f;
}

ABDRPrototypeBlockout::ABDRPrototypeBlockout()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent->SetMobility(EComponentMobility::Static);

    PickupZoneClass = ABDRPickupZone::StaticClass();
    DeliveryZoneClass = ABDRDeliveryZone::StaticClass();
    HazardClass = ABDRHazardBase::StaticClass();
    VisualBenchmarkClass = ABDRVisualBenchmark::StaticClass();
}

void ABDRPrototypeBlockout::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ClearPreview();

    if (!bBuildPreview)
    {
        return;
    }

    const float SafeRoadWidth = FMath::Max(RoadWidth, 500.0f);
    const float SafeLength = FMath::Max(BlockLength, FMath::Max(2400.0f, SafeRoadWidth * 3.35f));
    const float SafeBlockWidth = FMath::Max(BlockWidth, FMath::Max(2600.0f, SafeRoadWidth * 3.2f));
    const float SafeSidewalkWidth = FMath::Max(SidewalkWidth, 160.0f);
    const float SafeShortcutWidth = FMath::Clamp(ShortcutWidth, 320.0f, FMath::Min(560.0f, SafeRoadWidth * 0.78f));
    const int32 SafeBuildingPairs = FMath::Clamp(BuildingPairs, 2, 12);
    const float RoadHalfWidth = SafeRoadWidth * 0.5f;
    const float BlockHalfWidth = SafeBlockWidth * 0.5f;
    const float VerticalRoadCenterX = BlockHalfWidth - RoadHalfWidth;
    const float SouthRoadY = StartRoadCenterY;
    const float NorthRoadY = SouthRoadY + SafeLength - SafeRoadWidth;
    const float SouthRoadOuterY = SouthRoadY - RoadHalfWidth;
    const float NorthRoadOuterY = NorthRoadY + RoadHalfWidth;
    const float SouthRoadInnerY = SouthRoadY + RoadHalfWidth;
    const float NorthRoadInnerY = NorthRoadY - RoadHalfWidth;
    const float RouteCenterY = (SouthRoadY + NorthRoadY) * 0.5f;
    const float InnerHalfWidth = BlockHalfWidth - SafeRoadWidth;
    const float InnerHalfLength = RouteCenterY - SafeRoadWidth;
    const float DistrictHalfWidth = BlockHalfWidth + SafeSidewalkWidth + BDRGreybox::LotDepth;

    const FLinearColor GroundColor(0.10f, 0.16f, 0.10f);
    const FLinearColor RoadColor(0.055f, 0.06f, 0.07f);
    const FLinearColor ShortcutColor(0.09f, 0.095f, 0.105f);
    const FLinearColor SidewalkColor(0.42f, 0.44f, 0.46f);
    const FLinearColor LotColor(0.24f, 0.25f, 0.26f);
    const FLinearColor LaneColor(0.95f, 0.78f, 0.16f);
    const FLinearColor BarrierColor(0.95f, 0.24f, 0.08f);

    // All dimensions are real centimetres. Ground top is Z=0 and every driveable surface has the same top height.
    AddBox(TEXT("Ground"), FVector(0.0f, RouteCenterY, -BDRGreybox::GroundThickness * 0.5f), FVector(DistrictHalfWidth * 2.0f + 400.0f, SafeLength + (SafeSidewalkWidth + BDRGreybox::LotDepth) * 2.0f + 400.0f, BDRGreybox::GroundThickness), GroundColor);

    // Four overlapping road strips form a continuous rectangular loop with no seams at the corners.
    AddBox(TEXT("WestRoad"), FVector(-VerticalRoadCenterX, RouteCenterY, BDRGreybox::RoadThickness * 0.5f), FVector(SafeRoadWidth, SafeLength, BDRGreybox::RoadThickness), RoadColor);
    AddBox(TEXT("EastRoad"), FVector(VerticalRoadCenterX, RouteCenterY, BDRGreybox::RoadThickness * 0.5f), FVector(SafeRoadWidth, SafeLength, BDRGreybox::RoadThickness), RoadColor);
    AddBox(TEXT("SouthRoad"), FVector(0.0f, SouthRoadY, BDRGreybox::RoadThickness * 0.5f), FVector(SafeBlockWidth, SafeRoadWidth, BDRGreybox::RoadThickness), RoadColor);
    AddBox(TEXT("NorthRoad"), FVector(0.0f, NorthRoadY, BDRGreybox::RoadThickness * 0.5f), FVector(SafeBlockWidth, SafeRoadWidth, BDRGreybox::RoadThickness), RoadColor);

    const float VerticalStreetLength = SafeLength - SafeRoadWidth * 2.0f;
    const float OuterSidewalkCenterX = BlockHalfWidth + SafeSidewalkWidth * 0.5f;
    const float OuterLotCenterX = BlockHalfWidth + SafeSidewalkWidth + BDRGreybox::LotDepth * 0.5f;
    AddBox(TEXT("WestSidewalk"), FVector(-OuterSidewalkCenterX, RouteCenterY, BDRGreybox::SidewalkHeight * 0.5f), FVector(SafeSidewalkWidth, VerticalStreetLength, BDRGreybox::SidewalkHeight), SidewalkColor);
    AddBox(TEXT("EastSidewalk"), FVector(OuterSidewalkCenterX, RouteCenterY, BDRGreybox::SidewalkHeight * 0.5f), FVector(SafeSidewalkWidth, VerticalStreetLength, BDRGreybox::SidewalkHeight), SidewalkColor);
    AddBox(TEXT("WestLots"), FVector(-OuterLotCenterX, RouteCenterY, BDRGreybox::SidewalkHeight * 0.5f), FVector(BDRGreybox::LotDepth, VerticalStreetLength, BDRGreybox::SidewalkHeight), LotColor);
    AddBox(TEXT("EastLots"), FVector(OuterLotCenterX, RouteCenterY, BDRGreybox::SidewalkHeight * 0.5f), FVector(BDRGreybox::LotDepth, VerticalStreetLength, BDRGreybox::SidewalkHeight), LotColor);
    AddBox(TEXT("SouthSidewalk"), FVector(0.0f, SouthRoadOuterY - SafeSidewalkWidth * 0.5f, BDRGreybox::SidewalkHeight * 0.5f), FVector(SafeBlockWidth, SafeSidewalkWidth, BDRGreybox::SidewalkHeight), SidewalkColor);
    AddBox(TEXT("NorthSidewalk"), FVector(0.0f, NorthRoadOuterY + SafeSidewalkWidth * 0.5f, BDRGreybox::SidewalkHeight * 0.5f), FVector(SafeBlockWidth, SafeSidewalkWidth, BDRGreybox::SidewalkHeight), SidewalkColor);

    // Split the centre block into four pads. The gaps are real road cuts, so the pawn never has to climb a hidden curb.
    const float HorizontalShortcutWidth = SafeShortcutWidth * 1.10f;
    const float ShortcutHalfWidth = SafeShortcutWidth * 0.5f;
    const float HorizontalShortcutHalfWidth = HorizontalShortcutWidth * 0.5f;
    const float CentralPadWidth = InnerHalfWidth - ShortcutHalfWidth;
    const float CentralPadLength = InnerHalfLength - HorizontalShortcutHalfWidth;
    const float CentralPadCenterX = (InnerHalfWidth + ShortcutHalfWidth) * 0.5f;
    const float CentralPadCenterY = (InnerHalfLength + HorizontalShortcutHalfWidth) * 0.5f;

    for (int32 XSide = -1; XSide <= 1; XSide += 2)
    {
        for (int32 YSide = -1; YSide <= 1; YSide += 2)
        {
            const FString PadName = FString::Printf(TEXT("CentralPad_%d_%d"), XSide, YSide);
            AddBox(PadName, FVector(CentralPadCenterX * XSide, RouteCenterY + CentralPadCenterY * YSide, BDRGreybox::SidewalkHeight * 0.5f), FVector(CentralPadWidth, CentralPadLength, BDRGreybox::SidewalkHeight), LotColor);
        }
    }

    AddBox(TEXT("Shortcut_NorthSouth"), FVector(0.0f, RouteCenterY, BDRGreybox::RoadThickness * 0.5f), FVector(SafeShortcutWidth, InnerHalfLength * 2.0f, BDRGreybox::RoadThickness), ShortcutColor);
    AddBox(TEXT("Shortcut_EastWest"), FVector(0.0f, RouteCenterY, BDRGreybox::RoadThickness * 0.5f), FVector(InnerHalfWidth * 2.0f, HorizontalShortcutWidth, BDRGreybox::RoadThickness), ShortcutColor);

    const float MarkStride = BDRGreybox::LaneMarkLength + BDRGreybox::LaneMarkGap;
    const int32 VerticalMarkCount = FMath::FloorToInt((SafeLength - 80.0f) / MarkStride);
    for (int32 MarkIndex = 0; MarkIndex < VerticalMarkCount; ++MarkIndex)
    {
        const float Y = SouthRoadOuterY + 40.0f + BDRGreybox::LaneMarkLength * 0.5f + MarkIndex * MarkStride;
        AddRoadMarking(FString::Printf(TEXT("WestLaneMark_%02d"), MarkIndex), FVector(-VerticalRoadCenterX, Y, BDRGreybox::RoadThickness + 0.75f), FVector(14.0f, BDRGreybox::LaneMarkLength, 1.5f), LaneColor);
        AddRoadMarking(FString::Printf(TEXT("EastLaneMark_%02d"), MarkIndex), FVector(VerticalRoadCenterX, Y, BDRGreybox::RoadThickness + 0.75f), FVector(14.0f, BDRGreybox::LaneMarkLength, 1.5f), LaneColor);
    }

    const int32 HorizontalMarkCount = FMath::FloorToInt((SafeBlockWidth - 80.0f) / MarkStride);
    for (int32 MarkIndex = 0; MarkIndex < HorizontalMarkCount; ++MarkIndex)
    {
        const float X = -BlockHalfWidth + 40.0f + BDRGreybox::LaneMarkLength * 0.5f + MarkIndex * MarkStride;
        AddRoadMarking(FString::Printf(TEXT("SouthLaneMark_%02d"), MarkIndex), FVector(X, SouthRoadY, BDRGreybox::RoadThickness + 0.75f), FVector(BDRGreybox::LaneMarkLength, 14.0f, 1.5f), LaneColor);
        AddRoadMarking(FString::Printf(TEXT("NorthLaneMark_%02d"), MarkIndex), FVector(X, NorthRoadY, BDRGreybox::RoadThickness + 0.75f), FVector(BDRGreybox::LaneMarkLength, 14.0f, 1.5f), LaneColor);
    }

    // Keep the shared centre completely clear. Each shortcut gets only two edge barriers with a car-width safety margin.
    const float VerticalBarrierX = ShortcutHalfWidth - BDRGreybox::ShortcutBarrierWidth * 0.5f - 16.0f;
    const float VerticalBarrierFractions[] = {-0.62f, 0.62f};
    for (int32 BarrierIndex = 0; BarrierIndex < UE_ARRAY_COUNT(VerticalBarrierFractions); ++BarrierIndex)
    {
        const float X = BarrierIndex % 2 == 0 ? -VerticalBarrierX : VerticalBarrierX;
        const float Y = RouteCenterY + InnerHalfLength * VerticalBarrierFractions[BarrierIndex];
        AddBox(FString::Printf(TEXT("NorthSouthBarrier_%02d"), BarrierIndex), FVector(X, Y, BDRGreybox::RoadThickness + BDRGreybox::ShortcutBarrierHeight * 0.5f), FVector(BDRGreybox::ShortcutBarrierWidth, BDRGreybox::ShortcutBarrierLength, BDRGreybox::ShortcutBarrierHeight), BarrierColor);
    }

    const float HorizontalBarrierY = HorizontalShortcutHalfWidth - BDRGreybox::ShortcutBarrierWidth * 0.5f - 16.0f;
    const float HorizontalBarrierFractions[] = {-0.62f, 0.62f};
    for (int32 BarrierIndex = 0; BarrierIndex < UE_ARRAY_COUNT(HorizontalBarrierFractions); ++BarrierIndex)
    {
        const float X = InnerHalfWidth * HorizontalBarrierFractions[BarrierIndex];
        const float Y = RouteCenterY + (BarrierIndex % 2 == 0 ? -HorizontalBarrierY : HorizontalBarrierY);
        AddBox(FString::Printf(TEXT("EastWestBarrier_%02d"), BarrierIndex), FVector(X, Y, BDRGreybox::RoadThickness + BDRGreybox::ShortcutBarrierHeight * 0.5f), FVector(BDRGreybox::ShortcutBarrierLength, BDRGreybox::ShortcutBarrierWidth, BDRGreybox::ShortcutBarrierHeight), BarrierColor);
    }

    // Outside buildings use equal lots. Their width is derived from the pitch, so they can never overlap.
    const float BuildableLength = VerticalStreetLength - BDRGreybox::EndClearance * 2.0f;
    const int32 LayoutBuildingPairs = FMath::Min(SafeBuildingPairs, FMath::Max(2, FMath::FloorToInt(BuildableLength / 210.0f)));
    const float LotPitch = BuildableLength / static_cast<float>(LayoutBuildingPairs);
    const float BuildingWidth = FMath::Clamp(LotPitch - 60.0f, 180.0f, 460.0f);
    const float BuildingCenterX = BlockHalfWidth + SafeSidewalkWidth + BDRGreybox::BuildingSetback + BDRGreybox::BuildingDepth * 0.5f;

    for (int32 Index = 0; Index < LayoutBuildingPairs; ++Index)
    {
        const float Y = SouthRoadInnerY + BDRGreybox::EndClearance + LotPitch * (static_cast<float>(Index) + 0.5f);
        const int32 LeftFloors = 2 + (Index * 3) % 4;
        const int32 RightFloors = 2 + (Index * 3 + 2) % 4;
        const FLinearColor LeftColor = (Index % 2 == 0) ? FLinearColor(0.27f, 0.38f, 0.50f) : FLinearColor(0.35f, 0.32f, 0.46f);
        const FLinearColor RightColor = (Index % 2 == 0) ? FLinearColor(0.48f, 0.34f, 0.27f) : FLinearColor(0.31f, 0.43f, 0.36f);
        AddGreyboxBuilding(FString::Printf(TEXT("WestBuilding_%02d"), Index), FVector(-BuildingCenterX, Y, BDRGreybox::SidewalkHeight), BuildingWidth, BDRGreybox::BuildingDepth, LeftFloors, true, LeftColor, FLinearColor(0.76f, 0.82f, 0.88f));
        if (!(bSpawnVisualBenchmark && Index < 2))
        {
            AddGreyboxBuilding(FString::Printf(TEXT("EastBuilding_%02d"), Index), FVector(BuildingCenterX, Y, BDRGreybox::SidewalkHeight), BuildingWidth, BDRGreybox::BuildingDepth, RightFloors, false, RightColor, FLinearColor(0.84f, 0.79f, 0.68f));
        }
    }

    if (bSpawnVisualBenchmark && VisualBenchmarkClass)
    {
        const float FirstLotY = SouthRoadInnerY + BDRGreybox::EndClearance + LotPitch * 0.5f;
        AddChildActor(TEXT("VisualBenchmark"), VisualBenchmarkClass, FVector(BuildingCenterX, FirstLotY, BDRGreybox::SidewalkHeight));
    }

    // Four low central buildings make the shortcut entrances readable without hiding the road from the camera.
    const float CentralBuildingDepth = FMath::Clamp(CentralPadWidth - 100.0f, 180.0f, 520.0f);
    const float CentralBuildingWidth = FMath::Clamp(CentralPadLength - 120.0f, 180.0f, 600.0f);
    AddGreyboxBuilding(TEXT("CentralSouthWest"), FVector(-CentralPadCenterX, RouteCenterY - CentralPadCenterY, BDRGreybox::SidewalkHeight), CentralBuildingWidth, CentralBuildingDepth, 2, true, FLinearColor(0.38f, 0.31f, 0.48f), FLinearColor(1.0f, 0.58f, 0.08f));
    AddGreyboxBuilding(TEXT("CentralSouthEast"), FVector(CentralPadCenterX, RouteCenterY - CentralPadCenterY, BDRGreybox::SidewalkHeight), CentralBuildingWidth, CentralBuildingDepth, 3, false, FLinearColor(0.42f, 0.36f, 0.28f), FLinearColor(0.84f, 0.79f, 0.68f));
    AddGreyboxBuilding(TEXT("CentralNorthWest"), FVector(-CentralPadCenterX, RouteCenterY + CentralPadCenterY, BDRGreybox::SidewalkHeight), CentralBuildingWidth, CentralBuildingDepth, 3, true, FLinearColor(0.25f, 0.39f, 0.46f), FLinearColor(0.76f, 0.82f, 0.88f));
    AddGreyboxBuilding(TEXT("CentralNorthEast"), FVector(CentralPadCenterX, RouteCenterY + CentralPadCenterY, BDRGreybox::SidewalkHeight), CentralBuildingWidth, CentralBuildingDepth, 2, false, FLinearColor(0.34f, 0.43f, 0.34f), FLinearColor(0.08f, 0.68f, 1.0f));

    if (bSpawnGameplayZones)
    {
        const float ZoneZ = BDRGreybox::RoadThickness + BDRGreybox::MarkerHalfHeight;
        const float RestaurantY = SouthRoadInnerY + BDRGreybox::EndClearance + LotPitch * 0.5f;
        const float RestaurantEntranceX = BlockHalfWidth + SafeSidewalkWidth - 60.0f;
        // Pickup is at the restaurant entrance, not on the road: park, dismount, then walk into the marker.
        AddChildActor(TEXT("PickupZone"), PickupZoneClass, FVector(RestaurantEntranceX, RestaurantY, ZoneZ));
        AddChildActor(TEXT("DeliveryZone"), DeliveryZoneClass, FVector(VerticalRoadCenterX, NorthRoadInnerY - 350.0f, ZoneZ));
    }

    if (bSpawnHazardTriggers)
    {
        const float HazardZ = BDRGreybox::RoadThickness + 80.0f;
        AddChildActor(TEXT("HazardTrigger_01"), HazardClass, FVector(-VerticalRoadCenterX, RouteCenterY, HazardZ));
        AddChildActor(TEXT("HazardTrigger_02"), HazardClass, FVector(VerticalRoadCenterX, RouteCenterY, HazardZ));
    }
}

UStaticMeshComponent* ABDRPrototypeBlockout::AddBox(const FString& Name, const FVector& Center, const FVector& Dimensions, const FLinearColor& Color, bool bEnableCollision)
{
    UStaticMeshComponent* Box = NewObject<UStaticMeshComponent>(this, *Name);
    Box->SetupAttachment(RootComponent);
    Box->SetMobility(EComponentMobility::Static);
    Box->SetRelativeLocation(Center);
    Box->SetRelativeScale3D(FVector(
        FMath::Max(Dimensions.X, 1.0f) / 100.0f,
        FMath::Max(Dimensions.Y, 1.0f) / 100.0f,
        FMath::Max(Dimensions.Z, 1.0f) / 100.0f));
    Box->SetCollisionProfileName(bEnableCollision ? TEXT("BlockAll") : TEXT("NoCollision"));
    Box->ComponentTags.Add(BDRGreybox::GeneratedComponentTag);

    if (UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
    {
        Box->SetStaticMesh(CubeMesh);
    }
    if (UMaterialInterface* GreyboxMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/M_BDR_Greybox.M_BDR_Greybox")))
    {
        Box->SetMaterial(0, GreyboxMaterial);
    }

    AddInstanceComponent(Box);
    Box->RegisterComponent();

    if (UMaterialInstanceDynamic* Material = Box->CreateAndSetMaterialInstanceDynamic(0))
    {
        Material->SetVectorParameterValue(TEXT("Color"), Color);
        Material->SetVectorParameterValue(TEXT("BaseColor"), Color);
    }
    return Box;
}

void ABDRPrototypeBlockout::AddRoadMarking(const FString& Name, const FVector& Center, const FVector& Dimensions, const FLinearColor& Color)
{
    AddBox(Name, Center, Dimensions, Color, false);
}

void ABDRPrototypeBlockout::AddGreyboxBuilding(const FString& Name, const FVector& GroundCenter, float Width, float Depth, int32 FloorCount, bool bFacesPositiveX, const FLinearColor& Color, const FLinearColor& AccentColor)
{
    const float SafeFloorHeight = FMath::Max(FloorHeight, 240.0f);
    const float BodyHeight = SafeFloorHeight - BDRGreybox::FloorJointHeight;
    const float FrontDirection = bFacesPositiveX ? 1.0f : -1.0f;

    for (int32 FloorIndex = 0; FloorIndex < FloorCount; ++FloorIndex)
    {
        const float FloorBaseZ = GroundCenter.Z + FloorIndex * SafeFloorHeight;
        const FLinearColor FloorColor = Color * (FloorIndex % 2 == 0 ? 1.0f : 0.88f);
        AddBox(
            FString::Printf(TEXT("%s_Floor_%02d"), *Name, FloorIndex),
            FVector(GroundCenter.X, GroundCenter.Y, FloorBaseZ + BodyHeight * 0.5f),
            FVector(Depth, Width, BodyHeight),
            FloorColor);
        AddBox(
            FString::Printf(TEXT("%s_FloorJoint_%02d"), *Name, FloorIndex),
            FVector(GroundCenter.X, GroundCenter.Y, FloorBaseZ + BodyHeight + BDRGreybox::FloorJointHeight * 0.5f),
            FVector(Depth + 12.0f, Width + 12.0f, BDRGreybox::FloorJointHeight),
            FLinearColor(0.08f, 0.09f, 0.10f));
    }

    // A simple sign and door make the street-facing side unambiguous during route testing.
    const float FrontX = GroundCenter.X + FrontDirection * (Depth * 0.5f + 3.0f);
    AddBox(Name + TEXT("_Entrance"), FVector(FrontX, GroundCenter.Y, GroundCenter.Z + 105.0f), FVector(6.0f, FMath::Min(110.0f, Width * 0.32f), 210.0f), AccentColor, false);
    AddBox(Name + TEXT("_Sign"), FVector(FrontX + FrontDirection * 2.0f, GroundCenter.Y, GroundCenter.Z + 245.0f), FVector(8.0f, FMath::Min(220.0f, Width * 0.60f), 48.0f), AccentColor, false);

    const float RoofZ = GroundCenter.Z + FloorCount * SafeFloorHeight;
    AddBox(Name + TEXT("_Roof"), FVector(GroundCenter.X, GroundCenter.Y, RoofZ + 10.0f), FVector(Depth + 24.0f, Width + 24.0f, 20.0f), FLinearColor(0.10f, 0.11f, 0.12f));
}

UChildActorComponent* ABDRPrototypeBlockout::AddChildActor(const FString& Name, TSubclassOf<AActor> ActorClass, const FVector& Location)
{
    if (!ActorClass)
    {
        return nullptr;
    }

    UChildActorComponent* Child = NewObject<UChildActorComponent>(this, *Name);
    Child->SetupAttachment(RootComponent);
    Child->SetChildActorClass(ActorClass);
    Child->SetRelativeLocation(Location);
    Child->ComponentTags.Add(BDRGreybox::GeneratedComponentTag);
    AddInstanceComponent(Child);
    Child->RegisterComponent();
    return Child;
}

void ABDRPrototypeBlockout::ClearPreview()
{
    TArray<UActorComponent*> Components;
    GetComponents(Components);
    for (UActorComponent* Component : Components)
    {
        const bool bGeneratedByCurrentVersion = Component && Component->ComponentHasTag(BDRGreybox::GeneratedComponentTag);
        const bool bGeneratedByLegacyVersion = Component && Component->CreationMethod == EComponentCreationMethod::Instance;
        const bool bLegacyNativePreview = Component
            && Component->CreationMethod == EComponentCreationMethod::Native
            && (Component->IsA<UStaticMeshComponent>() || Component->IsA<UChildActorComponent>());
        if (Component != RootComponent && (bGeneratedByCurrentVersion || bGeneratedByLegacyVersion || bLegacyNativePreview))
        {
            Component->DestroyComponent();
        }
    }
}
