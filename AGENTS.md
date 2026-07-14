# BaoDanRider Codex Guide

## Project

- Unreal Engine 5.5 C++ project.
- Project file: `BaoDanRider.uproject`.
- Engine installation: `D:\UE_5.5`.
- Core gameplay belongs in C++; Blueprint is for assets, presentation, and tuning.
- The player relies on Codex for implementation and needs concise, step-by-step UE Editor guidance in Chinese.

## Build

Close Unreal Editor before a full build, then run:

```powershell
& 'D:\UE_5.5\Engine\Build\BatchFiles\Build.bat' BaoDanRiderEditor Win64 Development -Project='E:\BaoDanRider\BaoDanRider\BaoDanRider.uproject' -WaitMutex -NoHotReloadFromIDE
```

## Working Rules

- Preserve existing imported assets and user changes.
- Verify C++ changes with a full editor build.
- Keep systems data-driven where practical.
- Update `Docs/ProjectStatus.md` after meaningful milestones.
- Give editor instructions using the Chinese UE interface labels when known.

