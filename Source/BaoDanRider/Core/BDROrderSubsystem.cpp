#include "Core/BDROrderSubsystem.h"

FBDROrderData UBDROrderSubsystem::GenerateOrder(int32 PlayerLevel) const
{
    static const TArray<FString> Restaurants =
    {
        TEXT("阿强炒饭"), TEXT("北门炸鸡"), TEXT("青椒小馆"), TEXT("夜班拉面"), TEXT("三号便利餐")
    };

    static const TArray<FString> Customers =
    {
        TEXT("急着开会的顾客"), TEXT("刚洗完澡的顾客"), TEXT("门禁很严的顾客"), TEXT("爱拍照的顾客"), TEXT("给差评很快的顾客")
    };

    static const TArray<FString> Addresses =
    {
        TEXT("星河公寓 3 栋"), TEXT("旧城写字楼 B 座"), TEXT("南街小区 8 单元"), TEXT("大学宿舍北门"), TEXT("河岸花园 12 号")
    };

    static const TArray<FString> RiskTags =
    {
        TEXT("红灯密集"), TEXT("餐品易洒"), TEXT("催单严重"), TEXT("路障施工"), TEXT("夜路复杂")
    };

    FBDROrderData Order;
    Order.OrderId = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    Order.RestaurantName = Restaurants[FMath::RandRange(0, Restaurants.Num() - 1)];
    Order.CustomerName = Customers[FMath::RandRange(0, Customers.Num() - 1)];
    Order.CustomerAddress = Addresses[FMath::RandRange(0, Addresses.Num() - 1)];
    Order.DistanceMeters = FMath::FRandRange(420.0f, 900.0f + PlayerLevel * 30.0f);
    Order.TimeLimitSeconds = FMath::Clamp(Order.DistanceMeters / 7.0f + FMath::FRandRange(80.0f, 130.0f), 120.0f, 280.0f);
    Order.BaseReward = FMath::RoundToInt(Order.DistanceMeters * 0.08f) + 28 + PlayerLevel * 4;
    Order.RiskTag = RiskTags[FMath::RandRange(0, RiskTags.Num() - 1)];
    Order.PickupEvent = static_cast<EBDRPickupEvent>(FMath::RandRange(0, 2));
    return Order;
}

void UBDROrderSubsystem::StartOrderTimer(const FBDROrderData& Order)
{
    RemainingSeconds = FMath::Max(0.0f, Order.TimeLimitSeconds);
    bTimerRunning = true;
}

void UBDROrderSubsystem::PrepareOrderTimer(const FBDROrderData& Order)
{
    RemainingSeconds = FMath::Max(0.0f, Order.TimeLimitSeconds);
    bTimerRunning = false;
}

void UBDROrderSubsystem::TickOrder(float DeltaSeconds)
{
    if (bTimerRunning)
    {
        RemainingSeconds = FMath::Max(0.0f, RemainingSeconds - FMath::Max(0.0f, DeltaSeconds));
    }
}

void UBDROrderSubsystem::SetRemainingSeconds(float NewRemainingSeconds)
{
    RemainingSeconds = FMath::Max(0.0f, NewRemainingSeconds);
}

void UBDROrderSubsystem::StopOrderTimer()
{
    bTimerRunning = false;
}

float UBDROrderSubsystem::GetRemainingSeconds() const
{
    return RemainingSeconds;
}

bool UBDROrderSubsystem::IsOrderExpired() const
{
    return RemainingSeconds <= 0.0f;
}

bool UBDROrderSubsystem::IsOrderTimerRunning() const
{
    return bTimerRunning;
}
