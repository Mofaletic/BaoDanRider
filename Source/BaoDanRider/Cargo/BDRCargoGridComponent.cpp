#include "Cargo/BDRCargoGridComponent.h"

UBDRCargoGridComponent::UBDRCargoGridComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    EnsureGrid();
}

bool UBDRCargoGridComponent::TryPlaceItem(const FBDRCargoItem& Item, int32 X, int32 Y)
{
    EnsureGrid();
    if (!CanPlaceItem(Item, X, Y))
    {
        return false;
    }

    const int32 NewItemIndex = Items.Add(Item);
    for (int32 IX = X; IX < X + Item.Size.X; ++IX)
    {
        for (int32 IY = Y; IY < Y + Item.Size.Y; ++IY)
        {
            Cells[CellIndex(IX, IY)] = NewItemIndex;
        }
    }
    return true;
}

void UBDRCargoGridComponent::ClearCargo()
{
    Items.Reset();
    Cells.Init(INDEX_NONE, Width * Height);
}

void UBDRCargoGridComponent::AutoFillStarterMeal()
{
    ClearCargo();

    FBDRCargoItem Rice;
    Rice.Name = TEXT("炒饭");
    Rice.Size = FIntPoint(1, 1);
    TryPlaceItem(Rice, 0, 0);

    FBDRCargoItem Tea;
    Tea.Name = TEXT("奶茶");
    Tea.Size = FIntPoint(1, 2);
    Tea.bSpillable = true;
    Tea.bFragile = true;
    TryPlaceItem(Tea, 1, 0);

    FBDRCargoItem Noodles;
    Noodles.Name = TEXT("汤面");
    Noodles.Size = FIntPoint(2, 1);
    Noodles.bSpillable = true;
    Noodles.bFragile = true;
    TryPlaceItem(Noodles, 0, 2);
}

void UBDRCargoGridComponent::ApplyShake(float Strength, int32 AntiSpillLevel)
{
    const float Mitigated = FMath::Max(0.0f, Strength - AntiSpillLevel * 0.04f);
    for (FBDRCargoItem& Item : Items)
    {
        const float Sensitivity = Item.bSpillable ? 1.25f : 0.55f;
        Item.Stability = FMath::Clamp(Item.Stability - Mitigated * Sensitivity, 0.0f, 1.0f);
    }
}

void UBDRCargoGridComponent::TickTemperature(float DeltaSeconds, int32 InsulationLevel)
{
    const float Cooling = FMath::Max(0.0006f, 0.0025f - InsulationLevel * 0.00035f);
    for (FBDRCargoItem& Item : Items)
    {
        Item.Temperature = FMath::Clamp(Item.Temperature - DeltaSeconds * Cooling, 0.0f, 1.0f);
    }
}

bool UBDRCargoGridComponent::CanPlaceItem(const FBDRCargoItem& Item, int32 X, int32 Y) const
{
    if (X < 0 || Y < 0 || X + Item.Size.X > Width || Y + Item.Size.Y > Height)
    {
        return false;
    }

    for (int32 IX = X; IX < X + Item.Size.X; ++IX)
    {
        for (int32 IY = Y; IY < Y + Item.Size.Y; ++IY)
        {
            const int32 Index = CellIndex(IX, IY);
            if (!Cells.IsValidIndex(Index) || Cells[Index] != INDEX_NONE)
            {
                return false;
            }
        }
    }

    return true;
}

float UBDRCargoGridComponent::GetIntegrity() const
{
    if (Items.IsEmpty())
    {
        return 1.0f;
    }

    float Total = 0.0f;
    for (const FBDRCargoItem& Item : Items)
    {
        Total += Item.Stability;
    }
    return Total / Items.Num();
}

float UBDRCargoGridComponent::GetTemperatureAverage() const
{
    if (Items.IsEmpty())
    {
        return 1.0f;
    }

    float Total = 0.0f;
    for (const FBDRCargoItem& Item : Items)
    {
        Total += Item.Temperature;
    }
    return Total / Items.Num();
}

int32 UBDRCargoGridComponent::GetOccupiedCellCount() const
{
    int32 OccupiedCount = 0;
    for (const int32 Cell : Cells)
    {
        if (Cell != INDEX_NONE)
        {
            ++OccupiedCount;
        }
    }
    return OccupiedCount;
}

int32 UBDRCargoGridComponent::GetCapacityCellCount() const
{
    return FMath::Max(0, Width * Height);
}

FString UBDRCargoGridComponent::GetCellLabel(int32 X, int32 Y) const
{
    const int32 Index = CellIndex(X, Y);
    if (!Cells.IsValidIndex(Index) || Cells[Index] == INDEX_NONE || !Items.IsValidIndex(Cells[Index]))
    {
        return TEXT(".");
    }

    return Items[Cells[Index]].Name.Left(1);
}

int32 UBDRCargoGridComponent::CellIndex(int32 X, int32 Y) const
{
    return Y * Width + X;
}

void UBDRCargoGridComponent::EnsureGrid()
{
    const int32 Expected = Width * Height;
    if (Cells.Num() != Expected)
    {
        Cells.Init(INDEX_NONE, Expected);
    }
}
