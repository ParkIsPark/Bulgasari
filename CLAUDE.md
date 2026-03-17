# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Bulgasari** is an Unreal Engine 5.4 top-down shooting game (탑뷰 시점의 슈팅 게임). It was bootstrapped from UE5's Third-Person Template and uses a single C++ module (`Bulgasari`, Runtime type).

## Build & Development Commands

Open the project in Unreal Engine 5.4 by double-clicking `Bulgasari.uproject`. There is no separate build script — compilation happens inside the Unreal Editor or via UnrealBuildTool (UBT).

**Rebuild C++ from command line (Windows):**
```
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\Build.bat" BulgasariEditor Win64 Development "C:\Users\kk409\Documents\Unreal Projects\Bulgasari\Bulgasari.uproject" -WaitMutex -FromMsBuild
```

**Regenerate project files:**
```
"C:\Program Files\Epic Games\UE_5.4\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "C:\Users\kk409\Documents\Unreal Projects\Bulgasari\Bulgasari.uproject" -Game
```

There are no automated tests configured at this time.

## Code Architecture

### Module Structure

All C++ lives in `Source/Bulgasari/`:

| File | Purpose |
|---|---|
| `Bulgasari.h/cpp` | Module entry point — uses `FDefaultGameModuleImpl`, nothing custom |
| `BulgasariGameMode.h/cpp` | `AGameModeBase` subclass; sets default pawn to `BP_ThirdPersonCharacter` |
| `BulgasariCharacter.h/cpp` | `ACharacter` subclass; owns the spring arm + camera, handles Enhanced Input |

The character mesh, animations, and Blueprint logic live in `BP_ThirdPersonCharacter` (Content folder) — C++ intentionally avoids hard references to content assets.

### Input System

Uses **Enhanced Input** (UE5 standard). Input actions and mappings are defined in Blueprint/Data assets, then bound in `ABulgasariCharacter::SetupPlayerInputComponent` via `UEnhancedInputComponent`. Three actions are wired at the C++ level: `Move`, `Look`, `Jump`.

### Camera

`CameraBoom` (USpringArmComponent) is attached 400 units behind the character. `FollowCamera` (UCameraComponent) is attached to the boom tip. This is currently a third-person follow camera; the game description says top-down, so the boom pitch will likely be changed (or a separate camera rig added).

### Default Map & Game Mode

- Default map: `/Game/ThirdPerson/Maps/ThirdPersonMap`
- Global game mode override in `DefaultEngine.ini`: `BulgasariGameMode`
- Renderer: DirectX 12, Nanite + Lumen + Ray Tracing enabled

### Module Dependencies (`Bulgasari.Build.cs`)

`Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`

## Key Conventions

- **파일 이름:** 새로 만드는 모든 C++ 파일(및 블루프린트 에셋)에는 `Bgr` 접두사를 붙인다. (예: `BgrTopDownCamera.h`, `BgrEnemyBase.cpp`)
- C++ provides base classes and component wiring; gameplay details (weapons, enemies, UI) are expected to live in Blueprints.
- The single runtime module pattern means all new C++ gameplay classes go under `Source/Bulgasari/`.
- New UE5 input should use the Enhanced Input System, not the legacy `BindAxis`/`BindAction` API.
