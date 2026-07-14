#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BDRTypes.h"
#include "BDRUserWidget.generated.h"

UCLASS()
class BAODANRIDER_API UBDRUserWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    FBDRPlayerProfile GetPlayerProfile() const;

    UFUNCTION(BlueprintPure)
    FBDROrderData GetActiveOrder() const;

    UFUNCTION(BlueprintPure)
    FBDRRunResult GetLastRunResult() const;
};
