#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Core/BDRTypes.h"
#include "BDRSettlementService.generated.h"

UCLASS(BlueprintType)
class BAODANRIDER_API UBDRSettlementService : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    static FBDRRunResult CalculateSettlement(const FBDROrderData& Order, bool bSuccess, EBDRDeliveryMethod Method, float RemainingSeconds, float CargoIntegrity, float CargoTemperature, const FString& Note);
};
