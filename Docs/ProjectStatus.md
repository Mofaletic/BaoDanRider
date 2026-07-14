# Project Status

Last updated: 2026-07-14

## Current Direction

`BaoDanRider` is a stylized third-person delivery-rider action game built with Unreal Engine 5.5. Core systems use C++; Blueprint and imported assets handle presentation and tuning.

The approved design direction is documented in `Docs/GameDesign.md`: a finite workday spent earning as much as possible, GTA/Forza-inspired vehicle controls, an Overcooked-like visual tone, visible backpack capacity, on-foot restaurant pickup, soft failure, S/A/B/C ratings, and compact handcrafted districts.

The city is back in the greybox phase. The visual benchmark is frozen while the corrected workday loop is player-tested. Route scale, collision, building footprints, floor stacking, restaurant access, and pickup/delivery readability must be approved before art-dressing expands.

## Implemented Foundation

- Rider pawn with a visible prototype rider, bike, rear cargo box, bright prototype delivery backpack, and vehicle-following chase camera.
- Forward movement, braking-to-zero before reversing, steering, and Space drift.
- Vehicle-following chase camera with position/rotation lag and exposed tuning values; boom collision retraction is disabled to prevent sudden camera pushes.
- Smoothed steering, reduced high-speed steering, correct reverse steering, drift presentation, and wall depenetration/bounce handling.
- Order, save, settlement, upgrade, cargo-grid, pickup, delivery, and hazard C++ foundations.
- DataTable row structures and prototype CSV data for cargo items, hazards, and upgrades.
- `DeliveryCity` map configured as the default map.
- Deterministic greybox city actor and basic delivery flow actors.
- Greybox buildings use centimetre dimensions, equal lot spacing, exact 300 cm floor stacking, and street-facing entrances.
- Pickup and delivery zones are aligned to the sidewalk; hazard triggers are disabled by default during route testing.
- The greybox route is now a rectangular driving loop with widened north-south and east-west shortcuts, a barrier-free shared intersection, sparse edge barriers, and a start road aligned to the placed rider.
- `/Game/Materials/M_BDR_Greybox` provides exposure-resistant color coding for roads, lots, lane marks, barriers, and buildings.
- The corrected playable loop is one finite five-minute workday. Orders repeat without a full-screen interruption while time remains, and the goal is to earn as much as possible.
- The rider must stop, press `E` to dismount, walk into the restaurant pickup marker, receive the meal in the backpack, walk back, and press `E` near the parked bike to mount.
- The single-order timer is prepared when the restaurant job becomes available but does not start draining until the player enters the restaurant on foot and accepts the food.
- Workday expiration stops new orders. An unaccepted order ends immediately; food already in the backpack may still be delivered before the day summary.
- Pickup and delivery triggers report overlaps to the delivery game mode; only the objective valid for the current phase can advance the order, and restaurant pickup is rejected while mounted.
- The blue pickup marker now sits at the restaurant entrance rather than in the road. After pickup, the target first points to the parked bike, then switches to the orange delivery marker after mounting.
- The prototype backpack uses a 3 x 3 cargo grid. The starter meal occupies five of nine cells, and the visible backpack follows the rider both on and off the bike.
- The prototype HUD shows objective, workday time, order time, direction, distance, daily earnings, completed orders, on-foot/on-bike state, backpack occupancy, food condition, status text, and the day-end summary.
- Pressing `R` respawns the rider during a workday and starts a new workday only from the completed-day summary.
- Guarded delivery-state APIs reject repeated pickup and repeated settlement, preventing duplicate rewards.
- Five editor automation tests cover backpack capacity, order timer preparation/start, delivery state flow, settlement rules, and an isolated temporary save round trip.
- A first visual compatibility benchmark replaces two east-side greybox lots with one restaurant, one residence, fitted sidewalk pieces, and selected street props.
- Imported benchmark meshes are visual-only; simple collision proxies retain the approved 600 x 360 cm greybox footprints.

## Imported Assets

- `/Game/AnimeCitySuburbs`
- `/Game/Cartoon_City_Free`

Both packs are preserved for the later art-dressing phase. Imported assets must be fitted to approved greybox footprints instead of being placed with guessed uniform scales.

The first isolated visual benchmark uses selected `/Game/AnimeCitySuburbs` assets through `BDRVisualBenchmark`; the remaining greybox is unchanged. An audit of 372 static meshes found that `Cartoon_City_Free` is dominated by high-rise buildings that are too large for this compact district. Full findings and benchmark rules are in `Docs/VisualBenchmark.md`. Expansion is frozen until the corrected workday loop is approved.

## Production Pipeline

1. Greybox: approve road width, route length, rider scale, collision, sight lines, and reachable gameplay zones.
2. Gameplay lock: approve the complete workday loop, including dismounting, on-foot restaurant pickup, backpack loading, remounting, repeated orders, and day-end settlement.
3. Visual benchmark: finish one representative street corner with a restaurant, residence, road kit, props, lighting, UI motion, and the core pickup/delivery/collision VFX. Verify style, scale, collision, readability, and performance before mass production.
4. Art production: replace remaining greybox pieces gradually without changing the approved gameplay footprint, reusing the approved benchmark kit.
5. Polish: complete secondary VFX, audio, animation, camera feedback, optimization, accessibility, and final collision pass.

Do not skip directly from greybox to full asset scattering.

## Important Paths

- Project: `E:\BaoDanRider\BaoDanRider\BaoDanRider.uproject`
- Source: `E:\BaoDanRider\BaoDanRider\Source\BaoDanRider`
- Map: `/Game/Maps/DeliveryCity`
- Engine: `D:\UE_5.5`

## Immediate Next Work

1. Player-test the corrected loop: drive to the restaurant, stop, dismount, walk into the blue pickup area, verify the backpack and order timer, remount, and deliver.
2. Confirm that delivery increases today's earnings and order count, then begins the next restaurant job without a settlement interruption.
3. Confirm the five-minute workday end rule: no new order after time expires, but food already in the backpack can still be delivered before the summary.
4. Tune only the values exposed by that test: day duration, order time, reward, walking speed, mount distance, and restaurant/customer spacing.
5. After approval, add the first day-end upgrade purchase and save/continue. Resume the visual benchmark and core pickup/delivery/collision/drift effects only after this loop is locked.

## Latest Verification

- Full `BaoDanRiderEditor Win64 Development` build passed on 2026-07-13.
- Headless `/Game/Maps/DeliveryCity` load and play smoke test passed with no errors.
- Removed an invalid UE 5.5 prototype rider mesh reference found by the smoke test.
- Automated 1280x720 gameplay screenshot confirmed the restored chase-camera composition and the rider start position.
- Player screenshot identified an over-constrained centre intersection; shortcut width and obstacle placement were corrected on 2026-07-13.
- Player re-test confirmed the centre no longer traps the rider and chase-camera collision behavior is stable.
- Steering pass 3 increased base turn rate to 126 degrees/second, high-speed steering scale to 0.68, input response to 10.0, and full-steering threshold to 110 cm/second; full build and map smoke test passed.
- Player re-test confirmed steering pass 3 feels normal; the driving and camera baseline is now locked for the first vertical slice.
- Full editor build passed after the fixed-order state flow, objective markers, HUD, settlement, and restart path were added.
- Headless `DeliveryCity` play smoke confirmed the active game mode found both gameplay zones and started the fixed order without errors.
- A 1280x720 gameplay screenshot confirmed the HUD is readable, the blue pickup marker is visible, and the first target is directly ahead at about 10 metres.
- Player completed the blue pickup, orange delivery, settlement, and `R` restart flow successfully; the one-order gameplay vertical slice was approved on 2026-07-13.
- All four `BaoDanRider.Core` automation tests passed in UE 5.5: `OrderTimer`, `DeliveryStateFlow`, `SettlementRules`, and `SaveRoundTrip`.
- The post-test `DeliveryCity` headless play smoke passed and still found both gameplay zones.
- The visual benchmark full build and map smoke passed without missing-asset or constructor errors.
- Automated 1280x720 start and close-up screenshots confirmed the sample restaurant and residence face the road and remain outside the approved driving surface.
- The fixed-order prototype was replaced by a five-minute workday loop with repeated orders, daily earnings/order tracking, average daily grade, and a day-end summary.
- A visible 9-cell backpack, manual `E` dismount/remount, parked-bike return target, and on-foot movement were added in C++.
- The pickup marker was moved from the road to the restaurant entrance and reduced so riding past it cannot accept food.
- Automated screenshots confirmed the restaurant marker, readable backpack HUD, on-foot state, rider separation from the parked bike, and chase-camera transition.
- All five `BaoDanRider.Core` automation tests passed in UE 5.5: `BackpackCapacity`, `OrderTimer`, `DeliveryStateFlow`, `SettlementRules`, and `SaveRoundTrip`.
- The latest full editor build and `DeliveryCity` play smoke passed after the workday, backpack, dismount, restaurant pickup, and HUD changes.
- A recruitment-oriented GitHub `README.md` now documents the playable loop, verified systems, production-stage decisions, and the human-directed AI prototype iteration method.
- Three development screenshots were copied into `Docs/Media` so the public README does not depend on ignored `Saved` output.
- A repository `.gitignore` now excludes generated UE/IDE files, per-user content, and locally retained third-party/sample asset packs from public source commits.
- The public GitHub README now documents the project from initial direction-setting through whitebox rollback, driving feedback, core-loop reconstruction, and art-stage decisions, including a reusable prompt-optimization template and concrete acceptance criteria.

## Recovery Procedure

When starting a new Codex task, ask it to read `AGENTS.md` and `Docs/ProjectStatus.md`, inspect the current Git or filesystem changes, and compile before making assumptions.
