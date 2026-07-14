# How To Run

## 第一次打开

1. 安装 Unreal Engine 5.3 或更高版本。
2. 双击 `BaoDanRider.uproject`。
3. 如果 Unreal 提示版本不同，选择用已安装版本打开。
4. 如果提示缺少模块，选择 `Yes` 重新生成。
5. 进入编辑器后，如果没有地图，先创建 `Content/Maps/Hub` 和 `Content/Maps/DeliveryCity`。

## 生成蓝图

在编辑器中创建这些 Blueprint 子类：

- `BP_RiderPawn`，父类选 `BDRRiderPawn`。
- `BP_PickupZone`，父类选 `BDRPickupZone`。
- `BP_DeliveryZone`，父类选 `BDRDeliveryZone`。
- `BP_Hazard_Roadblock`，父类选 `BDRHazardBase`。
- `WBP_Hub` 和 `WBP_DeliveryHUD`，父类可选 `BDRUserWidget`。

## 最小测试地图

1. 打开 `DeliveryCity`。
2. 拖入 `BDRPrototypeBlockout`，它会自动生成道路、餐厅、顾客点和危险区。
3. 拖入 `BP_RiderPawn`，放在路起点。
4. Play，测试 W/S 加速刹车、A/D 转向、空格刹车、R 复位。

如果还没有创建 Blueprint，也可以直接拖 C++ 类：

- `BDRRiderPawn`
- `BDRPrototypeBlockout`

## 报错时发给 Codex

请截图或复制：

- Unreal Output Log 的红色错误。
- Visual Studio / Rider 编译错误。
- 你做了哪一步。
- 当前想达到的效果。

## 导入 DataTable

`Content/Data/*.prototype.json` 是给人和 Codex 阅读的原型配置，不要导入。

需要导入 UE DataTable 时使用这些 CSV：

- `Content/Data/CargoItems.csv`，行类型选 `BDRCargoItemRow`
- `Content/Data/Hazards.csv`，行类型选 `BDRHazardRow`
- `Content/Data/Upgrades.csv`，行类型选 `BDRUpgradeRow`
