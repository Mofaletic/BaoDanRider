#include "Core/BDRGameInstance.h"
#include "Core/BDRSaveGame.h"
#include "Kismet/GameplayStatics.h"

const FString UBDRGameInstance::SaveSlotName = TEXT("BaoDanRider_Profile");

void UBDRGameInstance::Init()
{
    Super::Init();
    LoadProfile();
}

void UBDRGameInstance::LoadProfile()
{
    if (!bProfilePersistenceEnabled)
    {
        Profile = FBDRPlayerProfile();
        return;
    }

    if (USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0))
    {
        if (UBDRSaveGame* BDRSave = Cast<UBDRSaveGame>(Loaded))
        {
            Profile = BDRSave->Profile;
            return;
        }
    }

    Profile = FBDRPlayerProfile();
    SaveProfile();
}

void UBDRGameInstance::SaveProfile() const
{
    if (!bProfilePersistenceEnabled)
    {
        return;
    }

    UBDRSaveGame* Save = Cast<UBDRSaveGame>(UGameplayStatics::CreateSaveGameObject(UBDRSaveGame::StaticClass()));
    if (!Save)
    {
        return;
    }

    Save->Profile = Profile;
    UGameplayStatics::SaveGameToSlot(Save, SaveSlotName, 0);
}

void UBDRGameInstance::ResetProfile()
{
    if (bProfilePersistenceEnabled)
    {
        UGameplayStatics::DeleteGameInSlot(SaveSlotName, 0);
    }
    Profile = FBDRPlayerProfile();
    bHasActiveOrder = false;
    bHasLastRunResult = false;
    Phase = EBDRGamePhase::Hub;
    SaveProfile();
}

void UBDRGameInstance::SetActiveOrder(const FBDROrderData& Order)
{
    ActiveOrder = Order;
    bHasActiveOrder = true;
    bHasLastRunResult = false;
    Phase = EBDRGamePhase::Pickup;
}

bool UBDRGameInstance::CanPickupActiveOrder() const
{
    return bHasActiveOrder && Phase == EBDRGamePhase::Pickup;
}

bool UBDRGameInstance::TryMarkActiveOrderPickedUp()
{
    if (!CanPickupActiveOrder())
    {
        return false;
    }

    Phase = EBDRGamePhase::Delivery;
    return true;
}

bool UBDRGameInstance::CanCompleteActiveOrder() const
{
    return bHasActiveOrder && Phase == EBDRGamePhase::Delivery;
}

void UBDRGameInstance::ApplyRunResult(const FBDRRunResult& Result)
{
    if (!CanCompleteActiveOrder())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BDRDelivery] Ignored settlement outside the delivery phase."));
        return;
    }

    LastRunResult = Result;
    bHasLastRunResult = true;
    bHasActiveOrder = false;

    Profile.Money += Result.Reward;
    Profile.Experience += Result.Experience;
    Profile.Stamina = FMath::Clamp(Profile.Stamina - 18.0f, 0.0f, 100.0f);

    while (Profile.Experience >= Profile.GetNextLevelExperience())
    {
        Profile.Experience -= Profile.GetNextLevelExperience();
        Profile.Level += 1;
        Profile.Stamina = FMath::Min(100.0f, Profile.Stamina + 15.0f);
    }

    Phase = EBDRGamePhase::Settlement;
    SaveProfile();
}

void UBDRGameInstance::EndDeliveryDay()
{
    bHasActiveOrder = false;
    Phase = EBDRGamePhase::Settlement;
    SaveProfile();
}

bool UBDRGameInstance::HasActiveOrder() const
{
    return bHasActiveOrder;
}

#if WITH_DEV_AUTOMATION_TESTS
void UBDRGameInstance::SetProfilePersistenceEnabledForTests(bool bEnabled)
{
    bProfilePersistenceEnabled = bEnabled;
}
#endif
