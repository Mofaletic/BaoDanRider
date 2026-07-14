#include "UI/BDRUserWidget.h"
#include "Core/BDRGameInstance.h"

FBDRPlayerProfile UBDRUserWidget::GetPlayerProfile() const
{
    if (const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        return BDRGI->Profile;
    }
    return FBDRPlayerProfile();
}

FBDROrderData UBDRUserWidget::GetActiveOrder() const
{
    if (const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        return BDRGI->ActiveOrder;
    }
    return FBDROrderData();
}

FBDRRunResult UBDRUserWidget::GetLastRunResult() const
{
    if (const UBDRGameInstance* BDRGI = Cast<UBDRGameInstance>(GetGameInstance()))
    {
        return BDRGI->LastRunResult;
    }
    return FBDRRunResult();
}
