#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BDRRiderPawn.generated.h"

class UBDRCargoGridComponent;
class UCameraComponent;
class UCapsuleComponent;
class USpringArmComponent;

UCLASS()
class BAODANRIDER_API ABDRRiderPawn : public APawn
{
    GENERATED_BODY()

public:
    ABDRRiderPawn();

    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable)
    void ApplyHazardHit(float BalanceDamage, float SpeedPenalty, float CargoShake);

    UFUNCTION(BlueprintCallable)
    void Crash();

    UFUNCTION(BlueprintCallable)
    void Respawn();

    UFUNCTION(BlueprintPure)
    float GetBalance() const;

    UFUNCTION(BlueprintPure)
    float GetForwardSpeed() const;

    UFUNCTION(BlueprintPure)
    bool IsOnFoot() const;

    UFUNCTION(BlueprintPure)
    bool CanMountBike() const;

    UFUNCTION(BlueprintPure)
    FVector GetParkedBikeLocation() const;

    UFUNCTION(BlueprintCallable)
    bool DismountBike();

    UFUNCTION(BlueprintCallable)
    bool MountBike();

    UFUNCTION(Exec)
    void BDRToggleMount();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCapsuleComponent> Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> BikeMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> RiderMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> CargoBoxMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> BackpackMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> BackpackFlapMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBDRCargoGridComponent> CargoGrid;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Acceleration = 2600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BrakeForce = 3600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxSpeed = 1450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ReverseMaxSpeed = 420.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TurnRate = 126.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving", meta=(ClampMin="0.25", ClampMax="1.0"))
    float HighSpeedSteeringScale = 0.68f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving", meta=(ClampMin="1.0"))
    float SteeringResponse = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving", meta=(ClampMin="1.0", ForceUnits="cm/s"))
    float FullSteeringSpeed = 110.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float VelocityAlignment = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DriftAlignment = 2.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DriftVisualRoll = 22.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CrashBounceStrength = 520.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CollisionDepenetrationDistance = 36.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDebugDrawCollision = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Foot", meta=(ClampMin="100.0", ForceUnits="cm/s"))
    float OnFootMoveSpeed = 420.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Foot", meta=(ClampMin="45.0", ForceUnits="deg/s"))
    float OnFootTurnRate = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Foot", meta=(ClampMin="0.0", ForceUnits="cm/s"))
    float MaxDismountSpeed = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Foot", meta=(ClampMin="100.0", ForceUnits="cm"))
    float MountDistance = 240.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="On Foot", meta=(ClampMin="300.0", ForceUnits="cm"))
    float OnFootCameraArmLength = 560.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chase Camera", meta=(ClampMin="400.0", ForceUnits="cm"))
    float CameraArmLength = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chase Camera", meta=(ClampMin="-60.0", ClampMax="-5.0", ForceUnits="deg"))
    float CameraPitch = -18.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chase Camera", meta=(ForceUnits="deg"))
    float CameraYaw = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chase Camera", meta=(ClampMin="35.0", ClampMax="90.0"))
    float CameraFieldOfView = 72.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chase Camera", meta=(ClampMin="0.0", ForceUnits="cm"))
    float CameraLookAheadDistance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chase Camera", meta=(ClampMin="0.1"))
    float CameraLookAheadInterpSpeed = 4.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Chase Camera", meta=(ForceUnits="cm"))
    float CameraTargetHeight = 130.0f;

private:
    FVector Velocity = FVector::ZeroVector;
    FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;
    float Balance = 100.0f;
    float ThrottleInput = 0.0f;
    float TurnInput = 0.0f;
    float SmoothedTurnInput = 0.0f;
    float BlockedTurnCooldown = 0.0f;
    float DriftAmount = 0.0f;
    FVector CameraLookAheadOffset = FVector::ZeroVector;
    bool bDriftHeld = false;
    bool bCrashed = false;
    bool bOnFoot = false;
    FTransform ParkedBikeTransform = FTransform::Identity;

    void MoveForward(float Value);
    void Turn(float Value);
    void DriftPressed();
    void DriftReleased();
    void RespawnPressed();
    void InteractPressed();
    void ApplyCameraSettings();
    void TickOnFoot(float DeltaSeconds);
    void AttachBikeVisuals();
    void ApplyBackpackColor();
};
