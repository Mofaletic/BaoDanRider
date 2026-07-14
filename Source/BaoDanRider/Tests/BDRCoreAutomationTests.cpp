#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Cargo/BDRCargoGridComponent.h"
#include "Core/BDRGameInstance.h"
#include "Core/BDROrderSubsystem.h"
#include "Core/BDRSaveGame.h"
#include "Core/BDRSettlementService.h"
#include "Kismet/GameplayStatics.h"

namespace BDRTest
{
    constexpr EAutomationTestFlags Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBDROrderTimerTest, "BaoDanRider.Core.OrderTimer", BDRTest::Flags)

bool FBDROrderTimerTest::RunTest(const FString& Parameters)
{
    UBDRGameInstance* GameInstance = NewObject<UBDRGameInstance>();
    GameInstance->SetProfilePersistenceEnabledForTests(false);
    UBDROrderSubsystem* Orders = NewObject<UBDROrderSubsystem>(GameInstance);
    FBDROrderData Order;
    Order.TimeLimitSeconds = 120.0f;

    Orders->PrepareOrderTimer(Order);
    TestFalse(TEXT("Prepared order waits for restaurant acceptance"), Orders->IsOrderTimerRunning());
    TestEqual(TEXT("Prepared order still exposes its time limit"), Orders->GetRemainingSeconds(), 120.0f);
    Orders->TickOrder(30.0f);
    TestEqual(TEXT("Order time cannot drain before restaurant acceptance"), Orders->GetRemainingSeconds(), 120.0f);

    Orders->StartOrderTimer(Order);
    TestTrue(TEXT("Timer starts running"), Orders->IsOrderTimerRunning());
    TestEqual(TEXT("Timer starts from the order limit"), Orders->GetRemainingSeconds(), 120.0f);

    Orders->TickOrder(30.0f);
    TestEqual(TEXT("Timer counts down"), Orders->GetRemainingSeconds(), 90.0f);

    Orders->TickOrder(-5.0f);
    TestEqual(TEXT("Negative delta cannot add time"), Orders->GetRemainingSeconds(), 90.0f);

    Orders->SetRemainingSeconds(-10.0f);
    TestEqual(TEXT("Remaining time clamps at zero"), Orders->GetRemainingSeconds(), 0.0f);
    TestTrue(TEXT("Zero time is expired"), Orders->IsOrderExpired());

    Orders->SetRemainingSeconds(25.0f);
    Orders->StopOrderTimer();
    Orders->TickOrder(10.0f);
    TestFalse(TEXT("Stopped timer reports not running"), Orders->IsOrderTimerRunning());
    TestEqual(TEXT("Stopped timer does not change"), Orders->GetRemainingSeconds(), 25.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBDRDeliveryStateFlowTest, "BaoDanRider.Core.DeliveryStateFlow", BDRTest::Flags)

bool FBDRDeliveryStateFlowTest::RunTest(const FString& Parameters)
{
    UBDRGameInstance* GameInstance = NewObject<UBDRGameInstance>();
    GameInstance->SetProfilePersistenceEnabledForTests(false);
    GameInstance->Profile = FBDRPlayerProfile();
    GameInstance->Profile.Money = 100;
    GameInstance->Profile.Experience = 0;
    GameInstance->Profile.Stamina = 100.0f;

    TestFalse(TEXT("No order cannot be picked up"), GameInstance->CanPickupActiveOrder());
    TestFalse(TEXT("No order cannot be completed"), GameInstance->CanCompleteActiveOrder());

    FBDROrderData Order;
    Order.OrderId = TEXT("AutomationOrder");
    GameInstance->SetActiveOrder(Order);
    TestTrue(TEXT("Setting an order activates it"), GameInstance->HasActiveOrder());
    TestEqual(TEXT("New order enters pickup phase"), GameInstance->Phase, EBDRGamePhase::Pickup);
    TestTrue(TEXT("Pickup is valid exactly once"), GameInstance->TryMarkActiveOrderPickedUp());
    TestEqual(TEXT("Pickup advances to delivery phase"), GameInstance->Phase, EBDRGamePhase::Delivery);
    TestFalse(TEXT("Repeated pickup is rejected"), GameInstance->TryMarkActiveOrderPickedUp());
    TestTrue(TEXT("Delivery is valid after pickup"), GameInstance->CanCompleteActiveOrder());

    FBDRRunResult Result;
    Result.bSuccess = true;
    Result.Reward = 25;
    Result.Experience = 10;
    Result.Rating = 4;
    GameInstance->ApplyRunResult(Result);

    TestEqual(TEXT("Settlement enters settlement phase"), GameInstance->Phase, EBDRGamePhase::Settlement);
    TestFalse(TEXT("Settlement clears the active order"), GameInstance->HasActiveOrder());
    TestTrue(TEXT("Settlement stores the last result"), GameInstance->bHasLastRunResult);
    TestEqual(TEXT("Reward is applied exactly once"), GameInstance->Profile.Money, 125);
    TestEqual(TEXT("Experience is applied"), GameInstance->Profile.Experience, 10);
    TestEqual(TEXT("A completed run consumes stamina"), GameInstance->Profile.Stamina, 82.0f);

    GameInstance->ApplyRunResult(Result);
    TestEqual(TEXT("Repeated settlement cannot duplicate rewards"), GameInstance->Profile.Money, 125);

    GameInstance->SetActiveOrder(Order);
    GameInstance->EndDeliveryDay();
    TestEqual(TEXT("Ending a day enters settlement phase"), GameInstance->Phase, EBDRGamePhase::Settlement);
    TestFalse(TEXT("Ending a day clears an unaccepted order"), GameInstance->HasActiveOrder());
    TestEqual(TEXT("Ending a day does not duplicate money"), GameInstance->Profile.Money, 125);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBDRSettlementRulesTest, "BaoDanRider.Core.SettlementRules", BDRTest::Flags)

bool FBDRSettlementRulesTest::RunTest(const FString& Parameters)
{
    FBDROrderData Order;
    Order.TimeLimitSeconds = 150.0f;
    Order.BaseReward = 80;

    const FBDRRunResult Perfect = UBDRSettlementService::CalculateSettlement(
        Order, true, EBDRDeliveryMethod::FaceToFace, 150.0f, 1.0f, 1.0f, TEXT("Perfect"));
    TestTrue(TEXT("Successful delivery succeeds"), Perfect.bSuccess);
    TestEqual(TEXT("Perfect delivery receives top numeric rating"), Perfect.Rating, 5);
    TestEqual(TEXT("Perfect reward follows the approved formula"), Perfect.Reward, 116);
    TestEqual(TEXT("Perfect food condition remains full"), Perfect.FoodIntegrity, 1.0f);

    const FBDRRunResult Damaged = UBDRSettlementService::CalculateSettlement(
        Order, true, EBDRDeliveryMethod::DoorPhoto, 0.0f, 0.4f, 0.2f, TEXT("Damaged"));
    TestTrue(TEXT("Late delivery remains a soft success"), Damaged.bSuccess);
    TestTrue(TEXT("Late damaged delivery grades below perfect"), Damaged.Rating < Perfect.Rating);
    TestTrue(TEXT("Late damaged delivery pays less"), Damaged.Reward < Perfect.Reward);

    const FBDRRunResult Failed = UBDRSettlementService::CalculateSettlement(
        Order, false, EBDRDeliveryMethod::FaceToFace, 150.0f, 1.0f, 1.0f, TEXT("Failed"));
    TestFalse(TEXT("Failed delivery is marked failed"), Failed.bSuccess);
    TestEqual(TEXT("Failed delivery gives no money"), Failed.Reward, 0);
    TestEqual(TEXT("Failed delivery still gives participation experience"), Failed.Experience, 8);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBDRSaveRoundTripTest, "BaoDanRider.Core.SaveRoundTrip", BDRTest::Flags)

bool FBDRSaveRoundTripTest::RunTest(const FString& Parameters)
{
    const FString SlotName = FString::Printf(TEXT("BaoDanRider_Automation_%s"), *FGuid::NewGuid().ToString(EGuidFormats::Digits));
    constexpr int32 UserIndex = 0;
    UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);

    UBDRSaveGame* Save = Cast<UBDRSaveGame>(UGameplayStatics::CreateSaveGameObject(UBDRSaveGame::StaticClass()));
    TestNotNull(TEXT("Save object can be created"), Save);
    if (!Save)
    {
        return false;
    }

    Save->Profile.Level = 4;
    Save->Profile.Experience = 37;
    Save->Profile.Money = 456;
    Save->Profile.Stamina = 72.5f;
    Save->Profile.Upgrades.Speed = 2;
    Save->Profile.Upgrades.Insulation = 1;

    const bool bSaved = UGameplayStatics::SaveGameToSlot(Save, SlotName, UserIndex);
    TestTrue(TEXT("Temporary profile saves successfully"), bSaved);
    TestTrue(TEXT("Temporary save exists"), UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex));

    UBDRSaveGame* Loaded = Cast<UBDRSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
    TestNotNull(TEXT("Temporary profile loads successfully"), Loaded);
    if (Loaded)
    {
        TestEqual(TEXT("Level survives save round trip"), Loaded->Profile.Level, 4);
        TestEqual(TEXT("Experience survives save round trip"), Loaded->Profile.Experience, 37);
        TestEqual(TEXT("Money survives save round trip"), Loaded->Profile.Money, 456);
        TestEqual(TEXT("Stamina survives save round trip"), Loaded->Profile.Stamina, 72.5f);
        TestEqual(TEXT("Speed upgrade survives save round trip"), Loaded->Profile.Upgrades.Speed, 2);
        TestEqual(TEXT("Insulation upgrade survives save round trip"), Loaded->Profile.Upgrades.Insulation, 1);
    }

    const bool bDeleted = UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
    TestTrue(TEXT("Temporary save is cleaned up"), bDeleted);
    TestFalse(TEXT("Temporary save no longer exists"), UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBDRBackpackCapacityTest, "BaoDanRider.Core.BackpackCapacity", BDRTest::Flags)

bool FBDRBackpackCapacityTest::RunTest(const FString& Parameters)
{
    UBDRCargoGridComponent* Backpack = NewObject<UBDRCargoGridComponent>();
    TestEqual(TEXT("Prototype backpack starts with nine cells"), Backpack->GetCapacityCellCount(), 9);
    TestEqual(TEXT("Prototype backpack starts empty"), Backpack->GetOccupiedCellCount(), 0);

    Backpack->AutoFillStarterMeal();
    TestEqual(TEXT("Starter meal contains three items"), Backpack->Items.Num(), 3);
    TestEqual(TEXT("Starter meal occupies five backpack cells"), Backpack->GetOccupiedCellCount(), 5);
    TestTrue(TEXT("Fresh starter meal begins intact"), FMath::IsNearlyEqual(Backpack->GetIntegrity(), 1.0f));

    Backpack->ClearCargo();
    TestEqual(TEXT("Clearing delivery empties backpack cells"), Backpack->GetOccupiedCellCount(), 0);
    return true;
}

#endif
