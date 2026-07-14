#include "UI/BDRDeliveryHUD.h"

#include "Core/BDRDeliveryGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

void ABDRDeliveryHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas || !GetWorld())
    {
        return;
    }

    const float Scale = FMath::Clamp(Canvas->SizeX / 1600.0f, 0.72f, 1.25f);
    const ABDRDeliveryGameMode* DeliveryMode = Cast<ABDRDeliveryGameMode>(GetWorld()->GetAuthGameMode());
    if (!DeliveryMode)
    {
        return;
    }

    if (DeliveryMode->IsAwaitingRestart())
    {
        DrawSettlementPanel(Scale);
    }
    else
    {
        DrawPrototypePanel(Scale);
    }
}

void ABDRDeliveryHUD::DrawPrototypePanel(float Scale)
{
    const ABDRDeliveryGameMode* DeliveryMode = Cast<ABDRDeliveryGameMode>(GetWorld()->GetAuthGameMode());
    if (!DeliveryMode || !GEngine)
    {
        return;
    }

    const float X = 28.0f * Scale;
    const float Y = 28.0f * Scale;
    const float Width = 720.0f * Scale;
    const float Height = 252.0f * Scale;
    DrawRect(FLinearColor(0.015f, 0.02f, 0.03f, 0.88f), X, Y, Width, Height);
    DrawRect(FLinearColor(0.04f, 0.72f, 1.0f, 0.95f), X, Y, 8.0f * Scale, Height);

    const int32 DaySeconds = FMath::CeilToInt(DeliveryMode->GetDayRemainingSeconds());
    const int32 OrderSeconds = FMath::CeilToInt(DeliveryMode->GetRemainingSeconds());
    const FString DayText = FString::Printf(TEXT("DAY  %02d:%02d"), DaySeconds / 60, DaySeconds % 60);
    const FString OrderText = DeliveryMode->IsOrderTimerRunning()
        ? FString::Printf(TEXT("ORDER  %02d:%02d"), OrderSeconds / 60, OrderSeconds % 60)
        : TEXT("ORDER  STARTS AT RESTAURANT");
    const FString TargetText = FString::Printf(TEXT("TARGET  %s   %.0f m"), *DeliveryMode->GetTargetDirectionText(), DeliveryMode->GetTargetDistanceMeters());
    const FString EarningsText = FString::Printf(TEXT("TODAY  $%d   |   ORDERS  %d   |   %s"), DeliveryMode->GetDayEarnings(), DeliveryMode->GetOrdersCompleted(), DeliveryMode->IsRiderOnFoot() ? TEXT("ON FOOT") : TEXT("ON BIKE"));
    const float CargoIntegrity = DeliveryMode->GetCargoIntegrity();
    const FString BackpackText = CargoIntegrity < 0.0f
        ? FString::Printf(TEXT("BACKPACK  %d/%d   |   EMPTY"), DeliveryMode->GetBackpackOccupiedCells(), DeliveryMode->GetBackpackCapacityCells())
        : FString::Printf(TEXT("BACKPACK  %d/%d   |   FOOD  %d%%"), DeliveryMode->GetBackpackOccupiedCells(), DeliveryMode->GetBackpackCapacityCells(), FMath::RoundToInt(CargoIntegrity * 100.0f));

    DrawText(DeliveryMode->GetObjectiveText(), FLinearColor(1.0f, 0.83f, 0.18f), X + 26.0f * Scale, Y + 16.0f * Scale, GEngine->GetMediumFont(), 1.10f * Scale, false);
    DrawText(DayText, FLinearColor::White, X + 26.0f * Scale, Y + 58.0f * Scale, GEngine->GetSmallFont(), 1.05f * Scale, false);
    DrawText(OrderText, FLinearColor(1.0f, 0.72f, 0.20f), X + 190.0f * Scale, Y + 58.0f * Scale, GEngine->GetSmallFont(), 0.95f * Scale, false);
    DrawText(TargetText, FLinearColor(0.20f, 0.82f, 1.0f), X + 445.0f * Scale, Y + 58.0f * Scale, GEngine->GetSmallFont(), 0.95f * Scale, false);
    DrawText(EarningsText, FLinearColor(0.48f, 1.0f, 0.58f), X + 26.0f * Scale, Y + 96.0f * Scale, GEngine->GetSmallFont(), 0.98f * Scale, false);
    DrawText(BackpackText, FLinearColor(0.98f, 0.58f, 0.16f), X + 26.0f * Scale, Y + 132.0f * Scale, GEngine->GetSmallFont(), 0.98f * Scale, false);
    DrawText(DeliveryMode->GetStatusMessage(), FLinearColor(0.88f, 0.90f, 0.94f), X + 26.0f * Scale, Y + 168.0f * Scale, GEngine->GetSmallFont(), 0.78f * Scale, false);
    DrawText(TEXT("W/S move   A/D turn   E get off/on   SPACE drift   R reset"), FLinearColor(0.60f, 0.64f, 0.70f), X + 26.0f * Scale, Y + 214.0f * Scale, GEngine->GetSmallFont(), 0.76f * Scale, false);
}

void ABDRDeliveryHUD::DrawSettlementPanel(float Scale)
{
    const ABDRDeliveryGameMode* DeliveryMode = Cast<ABDRDeliveryGameMode>(GetWorld()->GetAuthGameMode());
    if (!DeliveryMode || !GEngine)
    {
        return;
    }

    const float Width = 570.0f * Scale;
    const float Height = 330.0f * Scale;
    const float X = (Canvas->SizeX - Width) * 0.5f;
    const float Y = (Canvas->SizeY - Height) * 0.5f;
    DrawRect(FLinearColor(0.015f, 0.02f, 0.03f, 0.93f), X, Y, Width, Height);
    DrawRect(FLinearColor(1.0f, 0.55f, 0.08f, 0.98f), X, Y, Width, 9.0f * Scale);

    const FString GradeText = FString::Printf(TEXT("DAY GRADE  %s"), *DeliveryMode->GetDayRatingLetter());
    const FString OrdersText = FString::Printf(TEXT("ORDERS COMPLETED  %d"), DeliveryMode->GetOrdersCompleted());
    const FString EarningsText = FString::Printf(TEXT("MONEY EARNED  $%d"), DeliveryMode->GetDayEarnings());

    DrawText(TEXT("WORKDAY COMPLETE"), FLinearColor(1.0f, 0.83f, 0.18f), X + 36.0f * Scale, Y + 34.0f * Scale, GEngine->GetMediumFont(), 1.35f * Scale, false);
    DrawText(GradeText, FLinearColor(0.30f, 0.90f, 1.0f), X + 36.0f * Scale, Y + 94.0f * Scale, GEngine->GetLargeFont(), 1.40f * Scale, false);
    DrawText(OrdersText, FLinearColor::White, X + 36.0f * Scale, Y + 174.0f * Scale, GEngine->GetSmallFont(), 1.05f * Scale, false);
    DrawText(EarningsText, FLinearColor(0.45f, 1.0f, 0.55f), X + 36.0f * Scale, Y + 214.0f * Scale, GEngine->GetSmallFont(), 1.05f * Scale, false);
    DrawText(TEXT("PRESS R TO START A NEW WORKDAY"), FLinearColor(1.0f, 0.55f, 0.08f), X + 36.0f * Scale, Y + 282.0f * Scale, GEngine->GetSmallFont(), 1.0f * Scale, false);
}
