#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BDRDeliveryHUD.generated.h"

UCLASS()
class BAODANRIDER_API ABDRDeliveryHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    void DrawPrototypePanel(float Scale);
    void DrawSettlementPanel(float Scale);
};
