#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BDRVisualBenchmark.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS()
class BAODANRIDER_API ABDRVisualBenchmark : public AActor
{
    GENERATED_BODY()

public:
    ABDRVisualBenchmark();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UBoxComponent> ApprovedFootprintCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UBoxComponent> ResidenceFootprintCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> Shop;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> Residence;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> NoodleSign;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> FoodStand;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> SidewalkSouth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> SidewalkNorth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> StreetLamp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Visual Benchmark")
    TObjectPtr<UStaticMeshComponent> TrashCan;

private:
    UStaticMeshComponent* CreateVisualMesh(const FName& Name);
    static void PlaceMeshOnGround(UStaticMeshComponent* Component, UStaticMesh* Mesh, const FVector& BoundsCenter, float GroundZ, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);
    static void PlaceMeshAtCenter(UStaticMeshComponent* Component, UStaticMesh* Mesh, const FVector& BoundsCenter, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);
};
