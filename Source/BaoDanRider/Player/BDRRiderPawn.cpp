#include "Player/BDRRiderPawn.h"

#include "Camera/CameraComponent.h"
#include "Cargo/BDRCargoGridComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/BDRDeliveryGameMode.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ABDRRiderPawn::ABDRRiderPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
    Collision->InitCapsuleSize(82.0f, 94.0f);
    Collision->SetCollisionProfileName(TEXT("Pawn"));
    RootComponent = Collision;

    BikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BikeMesh"));
    BikeMesh->SetupAttachment(Collision);
    BikeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BikeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));
    BikeMesh->SetRelativeScale3D(FVector(1.85f, 0.72f, 0.28f));

    RiderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RiderMesh"));
    RiderMesh->SetupAttachment(Collision);
    RiderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RiderMesh->SetRelativeLocation(FVector(-12.0f, 0.0f, 18.0f));
    RiderMesh->SetRelativeScale3D(FVector(0.65f, 0.65f, 1.05f));

    CargoBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CargoBoxMesh"));
    CargoBoxMesh->SetupAttachment(Collision);
    CargoBoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CargoBoxMesh->SetRelativeLocation(FVector(-105.0f, 0.0f, -8.0f));
    CargoBoxMesh->SetRelativeScale3D(FVector(0.65f, 0.85f, 0.55f));

    BackpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackpackMesh"));
    BackpackMesh->SetupAttachment(Collision);
    BackpackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BackpackMesh->SetRelativeLocation(FVector(-55.0f, 0.0f, 22.0f));
    BackpackMesh->SetRelativeScale3D(FVector(0.38f, 0.58f, 0.68f));

    BackpackFlapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackpackFlapMesh"));
    BackpackFlapMesh->SetupAttachment(Collision);
    BackpackFlapMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BackpackFlapMesh->SetRelativeLocation(FVector(-75.0f, 0.0f, 48.0f));
    BackpackFlapMesh->SetRelativeScale3D(FVector(0.10f, 0.62f, 0.18f));

    CargoGrid = CreateDefaultSubobject<UBDRCargoGridComponent>(TEXT("CargoGrid"));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(Collision);
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->SetUsingAbsoluteRotation(false);
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 8.0f;
    SpringArm->CameraLagMaxDistance = 120.0f;
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 7.0f;
    SpringArm->bDoCollisionTest = false;
    SpringArm->ProbeSize = 18.0f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    ApplyCameraSettings();

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> RiderPrototypeMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BackpackMaterial(TEXT("/Game/Materials/M_BDR_Greybox.M_BDR_Greybox"));
    if (CubeMesh.Succeeded())
    {
        BikeMesh->SetStaticMesh(CubeMesh.Object);
        CargoBoxMesh->SetStaticMesh(CubeMesh.Object);
        BackpackMesh->SetStaticMesh(CubeMesh.Object);
        BackpackFlapMesh->SetStaticMesh(CubeMesh.Object);
    }
    if (RiderPrototypeMesh.Succeeded())
    {
        RiderMesh->SetStaticMesh(RiderPrototypeMesh.Object);
    }
    if (BackpackMaterial.Succeeded())
    {
        BackpackMesh->SetMaterial(0, BackpackMaterial.Object);
        BackpackFlapMesh->SetMaterial(0, BackpackMaterial.Object);
    }
}

void ABDRRiderPawn::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ApplyCameraSettings();
    ApplyBackpackColor();
}

void ABDRRiderPawn::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();
    SpawnRotation = GetActorRotation();
    ParkedBikeTransform = GetActorTransform();
    ApplyBackpackColor();
}

void ABDRRiderPawn::ApplyCameraSettings()
{
    if (SpringArm)
    {
        SpringArm->SetUsingAbsoluteRotation(false);
        SpringArm->bDoCollisionTest = false;
        SpringArm->TargetArmLength = bOnFoot ? FMath::Max(OnFootCameraArmLength, 300.0f) : FMath::Max(CameraArmLength, 400.0f);
        const float ActivePitch = bOnFoot ? FMath::Max(CameraPitch, -16.0f) : CameraPitch;
        SpringArm->SetRelativeRotation(FRotator(FMath::Clamp(ActivePitch, -60.0f, -5.0f), CameraYaw, 0.0f));
        SpringArm->TargetOffset = CameraLookAheadOffset + FVector(0.0f, 0.0f, CameraTargetHeight);
    }
    if (Camera)
    {
        Camera->SetFieldOfView(FMath::Clamp(CameraFieldOfView, 35.0f, 90.0f));
    }
}

void ABDRRiderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABDRRiderPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABDRRiderPawn::Turn);
    PlayerInputComponent->BindAction(TEXT("Brake"), IE_Pressed, this, &ABDRRiderPawn::DriftPressed);
    PlayerInputComponent->BindAction(TEXT("Brake"), IE_Released, this, &ABDRRiderPawn::DriftReleased);
    PlayerInputComponent->BindAction(TEXT("Respawn"), IE_Pressed, this, &ABDRRiderPawn::RespawnPressed);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ABDRRiderPawn::InteractPressed);
}

void ABDRRiderPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bOnFoot)
    {
        TickOnFoot(DeltaSeconds);
        return;
    }

    if (bCrashed)
    {
        return;
    }

    BlockedTurnCooldown = FMath::Max(0.0f, BlockedTurnCooldown - DeltaSeconds);
    SmoothedTurnInput = FMath::FInterpTo(SmoothedTurnInput, TurnInput, DeltaSeconds, SteeringResponse);

    const FVector Forward = GetActorForwardVector();
    const float CurrentSpeed = FVector::DotProduct(Velocity, Forward);

    if (ThrottleInput > 0.0f && CurrentSpeed < MaxSpeed)
    {
        Velocity += Forward * Acceleration * ThrottleInput * DeltaSeconds;
    }
    else if (ThrottleInput < 0.0f)
    {
        if (CurrentSpeed > 35.0f)
        {
            const float NewSpeed = FMath::Max(0.0f, CurrentSpeed - BrakeForce * FMath::Abs(ThrottleInput) * DeltaSeconds);
            Velocity = Forward * NewSpeed;
        }
        else
        {
            const float ReverseSpeed = FMath::Clamp(CurrentSpeed - Acceleration * 0.42f * FMath::Abs(ThrottleInput) * DeltaSeconds, -ReverseMaxSpeed, 0.0f);
            Velocity = Forward * ReverseSpeed;
        }
    }

    Velocity *= FMath::Pow(0.985f, DeltaSeconds * 60.0f);
    Velocity = Velocity.GetClampedToMaxSize(MaxSpeed);

    const float UpdatedForwardSpeed = FVector::DotProduct(Velocity, GetActorForwardVector());
    const float Speed = Velocity.Size();
    const float SpeedAlpha = FMath::Clamp(Speed / MaxSpeed, 0.0f, 1.0f);
    const bool bWantsDrift = bDriftHeld && FMath::Abs(SmoothedTurnInput) > 0.25f && SpeedAlpha > 0.18f;
    DriftAmount = FMath::FInterpTo(DriftAmount, bWantsDrift ? 1.0f : 0.0f, DeltaSeconds, bWantsDrift ? 7.0f : 3.0f);

    const float TurnBlockMultiplier = BlockedTurnCooldown > 0.0f && FMath::Abs(UpdatedForwardSpeed) > 80.0f ? 0.25f : 1.0f;
    const float SteeringActivation = FMath::Clamp(FMath::Abs(UpdatedForwardSpeed) / FMath::Max(FullSteeringSpeed, 1.0f), 0.0f, 1.0f);
    const float SpeedSensitiveSteering = FMath::Lerp(1.0f, HighSpeedSteeringScale, SpeedAlpha);
    const float SteeringDirection = UpdatedForwardSpeed < -5.0f ? -1.0f : 1.0f;
    const float DriftTurnBoost = FMath::Lerp(1.0f, 1.45f, DriftAmount);
    const float YawDelta = SmoothedTurnInput * TurnRate * SteeringActivation * SpeedSensitiveSteering * SteeringDirection * TurnBlockMultiplier * DriftTurnBoost * DeltaSeconds;
    AddActorWorldRotation(FRotator(0.0f, YawDelta, 0.0f));

    if (!Velocity.IsNearlyZero())
    {
        const FVector DesiredVelocity = GetActorForwardVector() * FVector::DotProduct(Velocity, GetActorForwardVector());
        const float Alignment = FMath::Lerp(VelocityAlignment, DriftAlignment, DriftAmount);
        Velocity = FMath::VInterpTo(Velocity, DesiredVelocity, DeltaSeconds, Alignment);
        if (DriftAmount > 0.05f)
        {
            Velocity += GetActorRightVector() * SmoothedTurnInput * SpeedAlpha * 120.0f * DriftAmount * DeltaSeconds;
        }
    }

    const FVector StartLocation = GetActorLocation();
    const FVector MoveDelta = Velocity * DeltaSeconds;
    FHitResult MoveHit;
    AddActorWorldOffset(MoveDelta, true, &MoveHit);
    if (MoveHit.bBlockingHit)
    {
        const FVector Normal = MoveHit.ImpactNormal.GetSafeNormal();
        const FVector SafeLocation = StartLocation + MoveDelta * FMath::Clamp(MoveHit.Time - 0.08f, 0.0f, 1.0f);
        SetActorLocation(SafeLocation, false, nullptr, ETeleportType::TeleportPhysics);
        AddActorWorldOffset(Normal * CollisionDepenetrationDistance, false);

        const FVector Reflected = FMath::GetReflectionVector(Velocity, Normal);
        const float ImpactSpeed = FMath::Clamp(Velocity.Size(), 180.0f, MaxSpeed);
        Velocity = Reflected.GetSafeNormal() * FMath::Max(CrashBounceStrength, ImpactSpeed * 0.48f);
        BlockedTurnCooldown = 0.18f;
        Balance = FMath::Max(0.0f, Balance - FMath::Clamp(ImpactSpeed / MaxSpeed * 22.0f, 6.0f, 22.0f));
        if (CargoGrid && !CargoGrid->Items.IsEmpty())
        {
            const float CargoShake = FMath::Clamp(ImpactSpeed / MaxSpeed * 0.12f, 0.03f, 0.12f);
            CargoGrid->ApplyShake(CargoShake, 0);
        }

        FHitResult PushHit;
        AddActorWorldOffset(Normal * CollisionDepenetrationDistance, true, &PushHit);
        if (PushHit.bBlockingHit)
        {
            SetActorLocation(StartLocation + Normal * CollisionDepenetrationDistance, false, nullptr, ETeleportType::TeleportPhysics);
            Velocity = Normal * CrashBounceStrength;
        }
    }

    const float Stress = FMath::Abs(SmoothedTurnInput) * SpeedAlpha * 18.0f;
    Balance = FMath::Clamp(Balance - Stress * DeltaSeconds + 18.0f * DeltaSeconds, 0.0f, 100.0f);

    const float VisualRoll = FMath::Lerp(10.0f, DriftVisualRoll, DriftAmount);
    const float VisualYaw = FMath::Lerp(0.0f, -SmoothedTurnInput * 10.0f, DriftAmount);
    BikeMesh->SetRelativeRotation(FRotator(0.0f, VisualYaw, -SmoothedTurnInput * VisualRoll * TurnBlockMultiplier));
    RiderMesh->SetRelativeRotation(FRotator(0.0f, VisualYaw * 0.5f, -SmoothedTurnInput * VisualRoll * 0.45f));
    CargoBoxMesh->SetRelativeRotation(FRotator(0.0f, VisualYaw * 0.35f, -SmoothedTurnInput * VisualRoll * 0.25f));
    BackpackMesh->SetRelativeRotation(FRotator(0.0f, VisualYaw * 0.35f, -SmoothedTurnInput * VisualRoll * 0.28f));
    BackpackFlapMesh->SetRelativeRotation(FRotator(0.0f, VisualYaw * 0.35f, -SmoothedTurnInput * VisualRoll * 0.28f));

    const FVector PlanarVelocity(Velocity.X, Velocity.Y, 0.0f);
    const FVector DesiredLookAhead = PlanarVelocity.GetSafeNormal() * CameraLookAheadDistance * SpeedAlpha;
    CameraLookAheadOffset = FMath::VInterpTo(CameraLookAheadOffset, DesiredLookAhead, DeltaSeconds, CameraLookAheadInterpSpeed);
    SpringArm->TargetOffset = CameraLookAheadOffset + FVector(0.0f, 0.0f, CameraTargetHeight);

    if (Balance <= 1.0f)
    {
        Crash();
    }

    if (bDebugDrawCollision)
    {
        DrawDebugCapsule(GetWorld(), GetActorLocation(), Collision->GetScaledCapsuleHalfHeight(), Collision->GetScaledCapsuleRadius(), GetActorQuat(), FColor::Cyan, false, 0.0f, 0, 1.5f);
        DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Velocity.GetSafeNormal() * 180.0f, FColor::Green, false, 0.0f, 0, 2.0f);
    }
}

void ABDRRiderPawn::ApplyHazardHit(float BalanceDamage, float SpeedPenalty, float CargoShake)
{
    Balance = FMath::Max(0.0f, Balance - BalanceDamage);
    Velocity *= FMath::Clamp(1.0f - SpeedPenalty, 0.0f, 1.0f);
    BlockedTurnCooldown = 0.2f;
    if (CargoGrid)
    {
        CargoGrid->ApplyShake(CargoShake, 0);
    }

    if (Balance <= 1.0f)
    {
        Crash();
    }
}

void ABDRRiderPawn::Crash()
{
    bCrashed = true;
    Balance = 0.0f;
    Velocity = FVector::ZeroVector;
    BikeMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 80.0f));
}

void ABDRRiderPawn::Respawn()
{
    bCrashed = false;
    bOnFoot = false;
    Balance = 65.0f;
    Velocity = FVector::ZeroVector;
    CameraLookAheadOffset = FVector::ZeroVector;
    SmoothedTurnInput = 0.0f;
    Collision->SetCapsuleSize(82.0f, 94.0f);
    SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::TeleportPhysics);
    AttachBikeVisuals();
    ParkedBikeTransform = GetActorTransform();
    ApplyCameraSettings();
    BikeMesh->SetRelativeRotation(FRotator::ZeroRotator);
    RiderMesh->SetRelativeRotation(FRotator::ZeroRotator);
    CargoBoxMesh->SetRelativeRotation(FRotator::ZeroRotator);
}

float ABDRRiderPawn::GetBalance() const
{
    return Balance;
}

float ABDRRiderPawn::GetForwardSpeed() const
{
    if (bOnFoot)
    {
        return ThrottleInput * OnFootMoveSpeed;
    }
    return FVector::DotProduct(Velocity, GetActorForwardVector());
}

bool ABDRRiderPawn::IsOnFoot() const
{
    return bOnFoot;
}

bool ABDRRiderPawn::CanMountBike() const
{
    return bOnFoot && FVector::Dist2D(GetActorLocation(), ParkedBikeTransform.GetLocation()) <= MountDistance;
}

FVector ABDRRiderPawn::GetParkedBikeLocation() const
{
    return ParkedBikeTransform.GetLocation();
}

bool ABDRRiderPawn::DismountBike()
{
    if (bOnFoot || bCrashed || FMath::Abs(GetForwardSpeed()) > MaxDismountSpeed)
    {
        return false;
    }

    ParkedBikeTransform = GetActorTransform();
    Velocity = FVector::ZeroVector;
    SmoothedTurnInput = 0.0f;
    BikeMesh->SetRelativeRotation(FRotator::ZeroRotator);
    CargoBoxMesh->SetRelativeRotation(FRotator::ZeroRotator);
    BikeMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    CargoBoxMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    bOnFoot = true;
    Collision->SetCapsuleSize(42.0f, 90.0f);

    const FVector SideStep = GetActorRightVector() * 150.0f;
    FHitResult SideHit;
    AddActorWorldOffset(SideStep, true, &SideHit);
    if (SideHit.bBlockingHit)
    {
        AddActorWorldOffset(-SideStep * 2.0f, true);
    }

    ApplyCameraSettings();
    return true;
}

bool ABDRRiderPawn::MountBike()
{
    if (!CanMountBike())
    {
        return false;
    }

    bOnFoot = false;
    Collision->SetCapsuleSize(82.0f, 94.0f);
    SetActorTransform(ParkedBikeTransform, false, nullptr, ETeleportType::TeleportPhysics);
    AttachBikeVisuals();
    Velocity = FVector::ZeroVector;
    SmoothedTurnInput = 0.0f;
    ApplyCameraSettings();
    return true;
}

void ABDRRiderPawn::MoveForward(float Value)
{
    ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ABDRRiderPawn::Turn(float Value)
{
    TurnInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ABDRRiderPawn::DriftPressed()
{
    bDriftHeld = true;
}

void ABDRRiderPawn::DriftReleased()
{
    bDriftHeld = false;
}

void ABDRRiderPawn::RespawnPressed()
{
    if (ABDRDeliveryGameMode* DeliveryMode = GetWorld() ? Cast<ABDRDeliveryGameMode>(GetWorld()->GetAuthGameMode()) : nullptr)
    {
        if (DeliveryMode->IsAwaitingRestart())
        {
            DeliveryMode->RestartPrototypeOrder();
            return;
        }
    }
    Respawn();
}

void ABDRRiderPawn::InteractPressed()
{
    BDRToggleMount();
}

void ABDRRiderPawn::BDRToggleMount()
{
    if (bOnFoot)
    {
        MountBike();
    }
    else
    {
        DismountBike();
    }
}

void ABDRRiderPawn::TickOnFoot(float DeltaSeconds)
{
    SmoothedTurnInput = FMath::FInterpTo(SmoothedTurnInput, TurnInput, DeltaSeconds, SteeringResponse);
    const float YawDelta = SmoothedTurnInput * OnFootTurnRate * DeltaSeconds;
    AddActorWorldRotation(FRotator(0.0f, YawDelta, 0.0f));

    const FVector MoveDelta = GetActorForwardVector() * ThrottleInput * OnFootMoveSpeed * DeltaSeconds;
    AddActorWorldOffset(MoveDelta, true);

    RiderMesh->SetRelativeRotation(FRotator::ZeroRotator);
    BackpackMesh->SetRelativeRotation(FRotator::ZeroRotator);
    BackpackFlapMesh->SetRelativeRotation(FRotator::ZeroRotator);
    CameraLookAheadOffset = FMath::VInterpTo(CameraLookAheadOffset, FVector::ZeroVector, DeltaSeconds, CameraLookAheadInterpSpeed);
    SpringArm->TargetOffset = FVector(0.0f, 0.0f, CameraTargetHeight);
}

void ABDRRiderPawn::AttachBikeVisuals()
{
    BikeMesh->AttachToComponent(Collision, FAttachmentTransformRules::KeepWorldTransform);
    CargoBoxMesh->AttachToComponent(Collision, FAttachmentTransformRules::KeepWorldTransform);
    BikeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));
    BikeMesh->SetRelativeScale3D(FVector(1.85f, 0.72f, 0.28f));
    BikeMesh->SetRelativeRotation(FRotator::ZeroRotator);
    CargoBoxMesh->SetRelativeLocation(FVector(-105.0f, 0.0f, -8.0f));
    CargoBoxMesh->SetRelativeScale3D(FVector(0.65f, 0.85f, 0.55f));
    CargoBoxMesh->SetRelativeRotation(FRotator::ZeroRotator);
}

void ABDRRiderPawn::ApplyBackpackColor()
{
    if (BackpackMesh)
    {
        if (UMaterialInstanceDynamic* Material = BackpackMesh->CreateAndSetMaterialInstanceDynamic(0))
        {
            Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.24f, 0.03f));
            Material->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(1.0f, 0.24f, 0.03f));
        }
    }
    if (BackpackFlapMesh)
    {
        if (UMaterialInstanceDynamic* Material = BackpackFlapMesh->CreateAndSetMaterialInstanceDynamic(0))
        {
            Material->SetVectorParameterValue(TEXT("Color"), FLinearColor(1.0f, 0.68f, 0.04f));
            Material->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(1.0f, 0.68f, 0.04f));
        }
    }
}
