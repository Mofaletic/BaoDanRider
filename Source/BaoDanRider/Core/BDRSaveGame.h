#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Core/BDRTypes.h"
#include "BDRSaveGame.generated.h"

UCLASS()
class BAODANRIDER_API UBDRSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBDRPlayerProfile Profile;
};
