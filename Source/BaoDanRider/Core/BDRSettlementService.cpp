#include "Core/BDRSettlementService.h"

FBDRRunResult UBDRSettlementService::CalculateSettlement(const FBDROrderData& Order, bool bSuccess, EBDRDeliveryMethod Method, float RemainingSeconds, float CargoIntegrity, float CargoTemperature, const FString& Note)
{
    const float TimeScore = FMath::Clamp(RemainingSeconds / FMath::Max(1.0f, Order.TimeLimitSeconds), 0.0f, 1.0f);
    const float FoodScore = FMath::Clamp(CargoIntegrity * 0.75f + CargoTemperature * 0.25f, 0.0f, 1.0f);
    const float MethodBonus = Method == EBDRDeliveryMethod::FaceToFace ? 0.06f : -0.02f;
    const float TotalScore = bSuccess ? FMath::Clamp(TimeScore * 0.35f + FoodScore * 0.65f + MethodBonus, 0.0f, 1.0f) : 0.0f;

    FBDRRunResult Result;
    Result.bSuccess = bSuccess;
    Result.Rating = FMath::Clamp(FMath::RoundToInt(TotalScore * 5.0f), 1, 5);
    Result.Reward = bSuccess ? FMath::RoundToInt(Order.BaseReward * (0.45f + TotalScore)) : 0;
    Result.Experience = bSuccess ? 35 + Result.Rating * 8 : 8;
    Result.RemainingSeconds = FMath::Max(0.0f, RemainingSeconds);
    Result.FoodIntegrity = FoodScore;
    Result.Note = Note;
    return Result;
}
