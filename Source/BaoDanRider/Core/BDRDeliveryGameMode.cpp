#include "Core/BDRDeliveryGameMode.h"

#include "Cargo/BDRCargoGridComponent.h"
#include "Core/BDRGameInstance.h"
#include "Core/BDROrderSubsystem.h"
#include "Core/BDRUpgradeService.h"
#include "EngineUtils.h"
#include "Interaction/BDRDeliveryZone.h"
#include "Interaction/BDRPickupZone.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BDRRiderPawn.h"
#include "UI/BDRDeliveryHUD.h"

ABDRDeliveryGameMode::ABDRDeliveryGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = ABDRRiderPawn::StaticClass();
    HUDClass = ABDRDeliveryHUD::StaticClass();
}

void ABDRDeliveryGameMode::BeginPlay()
{
    Super::BeginPlay();
    FindGameplayActors();

    if (bAutoStartPrototypeDay)
    {
        BeginPrototypeDay(false);
    }
}

void ABDRDeliveryGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>();
    if (Orders)
    {
        Orders->TickOrder(DeltaSeconds);
        if (BDRGI && BDRGI->CanCompleteActiveOrder() && Orders->IsOrderTimerRunning() && Orders->IsOrderExpired() && !bOverdueMessageShown)
        {
            bOverdueMessageShown = true;
            StatusMessage = TEXT("Order is late. Finish it anyway; the grade and pay will be lower.");
        }
    }

    if (bDayActive)
    {
        RemainingDaySeconds = FMath::Max(0.0f, RemainingDaySeconds - FMath::Max(0.0f, DeltaSeconds));
        if (RemainingDaySeconds <= 0.0f && !bDayExpiredMessageShown)
        {
            bDayExpiredMessageShown = true;
            if (BDRGI && BDRGI->CanCompleteActiveOrder())
            {
                bFinishActiveOrderAfterDay = true;
                StatusMessage = TEXT("The workday is over. Finish the food already in your backpack.");
            }
            else
            {
                EndPrototypeDay();
            }
        }
    }

    if (BDRGI && BDRGI->Phase == EBDRGamePhase::Delivery)
    {
        if (ABDRRiderPawn* Rider = ResolveRider(); Rider && Rider->CargoGrid)
        {
            Rider->CargoGrid->TickTemperature(DeltaSeconds, BDRGI->Profile.Upgrades.Insulation);
        }
    }
}

FBDROrderData ABDRDeliveryGameMode::GeneratePreviewOrder() const
{
    const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    const int32 Level = BDRGI ? BDRGI->Profile.Level : 1;
    if (const UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>())
    {
        return Orders->GenerateOrder(Level);
    }
    return FBDROrderData();
}

void ABDRDeliveryGameMode::AcceptOrder(const FBDROrderData& Order, FName DeliveryMapName)
{
    if (UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        BDRGI->SetActiveOrder(Order);
    }
    UGameplayStatics::OpenLevel(this, DeliveryMapName);
}

void ABDRDeliveryGameMode::ReturnToHub(FName HubMapName)
{
    UGameplayStatics::OpenLevel(this, HubMapName);
}

void ABDRDeliveryGameMode::ApplySettlement(const FBDRRunResult& Result)
{
    if (UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        BDRGI->ApplyRunResult(Result);
    }
}

bool ABDRDeliveryGameMode::BuyUpgrade(EBDRUpgradeType Type)
{
    if (UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        const bool bBought = UBDRUpgradeService::TryBuyUpgrade(BDRGI->Profile, Type);
        if (bBought)
        {
            BDRGI->SaveProfile();
        }
        return bBought;
    }
    return false;
}

void ABDRDeliveryGameMode::RestRider()
{
    if (UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        BDRGI->Profile.Stamina = 100.0f;
        BDRGI->SaveProfile();
    }
}

float ABDRDeliveryGameMode::GetRemainingSeconds() const
{
    if (const UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>())
    {
        return Orders->GetRemainingSeconds();
    }
    return 0.0f;
}

float ABDRDeliveryGameMode::GetDayRemainingSeconds() const
{
    return RemainingDaySeconds;
}

int32 ABDRDeliveryGameMode::GetDayEarnings() const
{
    return DayEarnings;
}

int32 ABDRDeliveryGameMode::GetOrdersCompleted() const
{
    return OrdersCompleted;
}

FString ABDRDeliveryGameMode::GetDayRatingLetter() const
{
    if (OrdersCompleted <= 0)
    {
        return TEXT("-");
    }
    return RatingToLetter(FMath::RoundToInt(static_cast<float>(DayRatingTotal) / OrdersCompleted));
}

bool ABDRDeliveryGameMode::IsOrderTimerRunning() const
{
    if (const UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>())
    {
        return Orders->IsOrderTimerRunning();
    }
    return false;
}

int32 ABDRDeliveryGameMode::GetBackpackOccupiedCells() const
{
    const ABDRRiderPawn* Rider = ResolveRider();
    return Rider && Rider->CargoGrid ? Rider->CargoGrid->GetOccupiedCellCount() : 0;
}

int32 ABDRDeliveryGameMode::GetBackpackCapacityCells() const
{
    const ABDRRiderPawn* Rider = ResolveRider();
    return Rider && Rider->CargoGrid ? Rider->CargoGrid->GetCapacityCellCount() : 0;
}

bool ABDRDeliveryGameMode::IsRiderOnFoot() const
{
    const ABDRRiderPawn* Rider = ResolveRider();
    return Rider && Rider->IsOnFoot();
}

void ABDRDeliveryGameMode::RestartPrototypeOrder()
{
    BeginPrototypeDay(true);
}

void ABDRDeliveryGameMode::HandlePickupReached(ABDRPickupZone* Zone, ABDRRiderPawn* Rider)
{
    UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>();
    if (!BDRGI || !Orders || !Zone || !Rider || !bDayActive || !BDRGI->CanPickupActiveOrder())
    {
        return;
    }

    if (!Rider->IsOnFoot())
    {
        StatusMessage = TEXT("Park nearby, stop the bike, press E to get off, then walk into the restaurant.");
        return;
    }

    Orders->StartOrderTimer(BDRGI->ActiveOrder);
    float RemainingSeconds = Orders->GetRemainingSeconds();
    FString PickupMessage;
    Zone->ResolvePickup(Rider, BDRGI->ActiveOrder, RemainingSeconds, PickupMessage);
    Orders->SetRemainingSeconds(RemainingSeconds);
    if (!BDRGI->TryMarkActiveOrderPickedUp())
    {
        Orders->StopOrderTimer();
        return;
    }

    ActiveRider = Rider;
    StatusMessage = PickupMessage + TEXT(" Walk back to the bike, press E to mount, then deliver to ORANGE.");
    UpdateZoneMarkers();
    UE_LOG(LogTemp, Display, TEXT("[BDRDelivery] Order accepted on foot. Remaining=%.1f Backpack=%d/%d"), RemainingSeconds, GetBackpackOccupiedCells(), GetBackpackCapacityCells());
}

void ABDRDeliveryGameMode::HandleDeliveryReached(ABDRDeliveryZone* Zone, ABDRRiderPawn* Rider)
{
    UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>();
    if (!BDRGI || !Orders || !Zone || !Rider || !BDRGI->CanCompleteActiveOrder())
    {
        return;
    }

    const FBDRRunResult Result = Zone->CompleteDelivery(Rider, BDRGI->ActiveOrder, EBDRDeliveryMethod::FaceToFace, Orders->GetRemainingSeconds());
    Orders->StopOrderTimer();
    ApplySettlement(Result);
    ActiveRider = Rider;
    DayEarnings += Result.Reward;
    DayRatingTotal += Result.Rating;
    ++OrdersCompleted;

    UE_LOG(LogTemp, Display, TEXT("[BDRDelivery] Delivery complete. Grade=%s Reward=%d DayOrders=%d DayEarnings=%d"), *RatingToLetter(Result.Rating), Result.Reward, OrdersCompleted, DayEarnings);

    if (bFinishActiveOrderAfterDay || RemainingDaySeconds <= 0.0f)
    {
        EndPrototypeDay();
        return;
    }

    BeginNextPrototypeOrder();
    StatusMessage = FString::Printf(TEXT("Delivered: +$%d. Return to the BLUE restaurant and get off the bike for the next order."), Result.Reward);
}

FString ABDRDeliveryGameMode::GetObjectiveText() const
{
    const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    if (bDayComplete)
    {
        return TEXT("WORKDAY COMPLETE");
    }
    if (!BDRGI)
    {
        return TEXT("WAITING FOR GAME STATE");
    }

    switch (BDRGI->Phase)
    {
    case EBDRGamePhase::Pickup:
        return IsRiderOnFoot() ? TEXT("WALK INTO THE BLUE RESTAURANT") : TEXT("PARK AND ENTER THE BLUE RESTAURANT");
    case EBDRGamePhase::Delivery:
        return IsRiderOnFoot() ? TEXT("RETURN TO THE BIKE") : TEXT("DELIVER THE FOOD");
    default:
        return TEXT("DELIVERY WORKDAY");
    }
}

FString ABDRDeliveryGameMode::GetStatusMessage() const
{
    return StatusMessage;
}

FString ABDRDeliveryGameMode::GetTargetDirectionText() const
{
    const ABDRRiderPawn* Rider = ResolveRider();
    const FVector ToTarget = GetCurrentTargetLocation() - (Rider ? Rider->GetActorLocation() : FVector::ZeroVector);
    const FVector Direction = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();
    if (!Rider || Direction.IsNearlyZero())
    {
        return TEXT("--");
    }

    const float ForwardDot = FVector::DotProduct(Rider->GetActorForwardVector(), Direction);
    const float RightDot = FVector::DotProduct(Rider->GetActorRightVector(), Direction);
    const float AngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));
    if (FMath::Abs(AngleDegrees) > 150.0f)
    {
        return TEXT("TURN AROUND");
    }
    if (AngleDegrees > 12.0f)
    {
        return TEXT(">>>");
    }
    if (AngleDegrees < -12.0f)
    {
        return TEXT("<<<");
    }
    return TEXT("^^^");
}

float ABDRDeliveryGameMode::GetTargetDistanceMeters() const
{
    const ABDRRiderPawn* Rider = ResolveRider();
    if (!Rider || IsAwaitingRestart())
    {
        return 0.0f;
    }
    return FVector::Dist2D(Rider->GetActorLocation(), GetCurrentTargetLocation()) / 100.0f;
}

float ABDRDeliveryGameMode::GetCargoIntegrity() const
{
    const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    const ABDRRiderPawn* Rider = ResolveRider();
    if (!BDRGI || BDRGI->Phase == EBDRGamePhase::Pickup || !Rider || !Rider->CargoGrid)
    {
        return -1.0f;
    }
    return Rider->CargoGrid->GetIntegrity();
}

FString ABDRDeliveryGameMode::GetRatingLetter() const
{
    const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    return BDRGI && BDRGI->bHasLastRunResult ? RatingToLetter(BDRGI->LastRunResult.Rating) : TEXT("-");
}

bool ABDRDeliveryGameMode::IsAwaitingRestart() const
{
    return bDayComplete;
}

void ABDRDeliveryGameMode::FindGameplayActors()
{
    PickupZone.Reset();
    DeliveryZone.Reset();
    ActiveRider.Reset();

    for (TActorIterator<ABDRPickupZone> It(GetWorld()); It; ++It)
    {
        PickupZone = *It;
        break;
    }
    for (TActorIterator<ABDRDeliveryZone> It(GetWorld()); It; ++It)
    {
        DeliveryZone = *It;
        break;
    }
    for (TActorIterator<ABDRRiderPawn> It(GetWorld()); It; ++It)
    {
        ActiveRider = *It;
        if (It->IsPlayerControlled())
        {
            break;
        }
    }
}

void ABDRDeliveryGameMode::BeginPrototypeDay(bool bResetRider)
{
    FindGameplayActors();

    DayEarnings = 0;
    OrdersCompleted = 0;
    DayRatingTotal = 0;
    RemainingDaySeconds = FMath::Max(60.0f, DayDurationSeconds);
    bDayActive = true;
    bDayComplete = false;
    bFinishActiveOrderAfterDay = false;
    bDayExpiredMessageShown = false;

    if (ABDRRiderPawn* Rider = ResolveRider())
    {
        if (bResetRider)
        {
            Rider->Respawn();
        }
        if (Rider->CargoGrid)
        {
            Rider->CargoGrid->ClearCargo();
        }
    }

    BeginNextPrototypeOrder();
    StatusMessage = PickupZone.IsValid() && DeliveryZone.IsValid()
        ? TEXT("Workday started. Drive to the BLUE restaurant, park, press E, and walk inside.")
        : TEXT("Workday started, but a gameplay marker is missing.");

    const ABDRRiderPawn* Rider = ResolveRider();
    UE_LOG(LogTemp, Display, TEXT("[BDRDelivery] Workday started. Day=%.0fs Rider=%s Pickup=%s Delivery=%s"),
        RemainingDaySeconds,
        Rider ? *Rider->GetActorLocation().ToCompactString() : TEXT("missing"),
        PickupZone.IsValid() ? *PickupZone->GetActorLocation().ToCompactString() : TEXT("missing"),
        DeliveryZone.IsValid() ? *DeliveryZone->GetActorLocation().ToCompactString() : TEXT("missing"));
}

void ABDRDeliveryGameMode::BeginNextPrototypeOrder()
{
    UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>();
    if (!BDRGI || !Orders)
    {
        StatusMessage = TEXT("Delivery state could not start.");
        EndPrototypeDay();
        return;
    }

    FBDROrderData Order;
    Order.OrderId = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    Order.RestaurantName = TEXT("Happy Wok");
    Order.CustomerName = TEXT("Mr. Hurry");
    Order.CustomerAddress = TEXT("Orange Apartment");
    Order.TimeLimitSeconds = FixedOrderTimeLimit;
    Order.BaseReward = FixedOrderBaseReward;
    Order.DistanceMeters = PickupZone.IsValid() && DeliveryZone.IsValid()
        ? FVector::Dist2D(PickupZone->GetActorLocation(), DeliveryZone->GetActorLocation()) / 100.0f
        : 0.0f;
    Order.RiskTag = TEXT("Workday route test");
    Order.PickupEvent = EBDRPickupEvent::Normal;

    BDRGI->SetActiveOrder(Order);
    Orders->PrepareOrderTimer(Order);
    bOverdueMessageShown = false;
    if (ABDRRiderPawn* Rider = ResolveRider(); Rider && Rider->CargoGrid)
    {
        Rider->CargoGrid->ClearCargo();
    }
    UpdateZoneMarkers();
}

void ABDRDeliveryGameMode::EndPrototypeDay()
{
    if (bDayComplete)
    {
        return;
    }

    bDayActive = false;
    bDayComplete = true;
    bFinishActiveOrderAfterDay = false;
    if (UBDROrderSubsystem* Orders = GetGameInstance()->GetSubsystem<UBDROrderSubsystem>())
    {
        Orders->StopOrderTimer();
    }
    if (UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        BDRGI->EndDeliveryDay();
    }
    StatusMessage = TEXT("Workday complete. Press R to start a new day.");
    UpdateZoneMarkers();
    UE_LOG(LogTemp, Display, TEXT("[BDRDelivery] Workday complete. Orders=%d Earnings=%d Grade=%s"), OrdersCompleted, DayEarnings, *GetDayRatingLetter());
}

FVector ABDRDeliveryGameMode::GetCurrentTargetLocation() const
{
    const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    const ABDRRiderPawn* Rider = ResolveRider();
    if (BDRGI && BDRGI->Phase == EBDRGamePhase::Pickup && PickupZone.IsValid())
    {
        return PickupZone->GetActorLocation();
    }
    if (BDRGI && BDRGI->Phase == EBDRGamePhase::Delivery)
    {
        if (Rider && Rider->IsOnFoot())
        {
            return Rider->GetParkedBikeLocation();
        }
        if (DeliveryZone.IsValid())
        {
            return DeliveryZone->GetActorLocation();
        }
    }
    return FVector::ZeroVector;
}

ABDRRiderPawn* ABDRDeliveryGameMode::ResolveRider() const
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ABDRRiderPawn* ControlledRider = Cast<ABDRRiderPawn>(PC->GetPawn()))
        {
            const_cast<ABDRDeliveryGameMode*>(this)->ActiveRider = ControlledRider;
            return ControlledRider;
        }
    }
    return ActiveRider.Get();
}

void ABDRDeliveryGameMode::UpdateZoneMarkers() const
{
    const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance());
    if (PickupZone.IsValid())
    {
        PickupZone->SetObjectiveActive(bDayActive && BDRGI && BDRGI->Phase == EBDRGamePhase::Pickup);
    }
    if (DeliveryZone.IsValid())
    {
        DeliveryZone->SetObjectiveActive(bDayActive && BDRGI && BDRGI->Phase == EBDRGamePhase::Delivery);
    }
}

FString ABDRDeliveryGameMode::RatingToLetter(int32 Rating)
{
    if (Rating >= 5)
    {
        return TEXT("S");
    }
    if (Rating == 4)
    {
        return TEXT("A");
    }
    if (Rating == 3)
    {
        return TEXT("B");
    }
    return TEXT("C");
}
