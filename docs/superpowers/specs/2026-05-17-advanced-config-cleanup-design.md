# Advanced Mode Configuration Cleanup Design

**Date:** 2026-05-17
**Status:** Approved

## Motivation

Edit dock advanced mode exposes ~60 configuration options across 4 pages (Behavior, Appearance, Effects, Tasks). Many reference features that are obsolete, non-functional, or redundant under Plasma 6 / Wayland — the exclusive target of this fork.

## Scope

Remove or simplify confirmed-broken features, KActivities-dependent options, and redundant compositing guards. Preserve all functional settings.

## Changes Per File

### 1. BehaviorConfig.qml

| Setting | Action | Reason |
|---|---|---|
| "Activate KWin edge after hiding" (`enableKWinEdges`) | **Remove** | KWin edge API fundamentally changed in Plasma 6 / Wayland. Non-functional. |
| "Raise on activity change" (`raiseOnActivity`) | **Remove** | Depends on KActivities signals that may not fire reliably in Plasma 6. |
| "Cycle Through Activities" from scroll action combo | **Remove option** | KActivities API uncertainty. Keep other scroll actions. |

**Also remove** the "Environment" section header if all its items become empty.

### 2. AppearanceConfig.qml

| Setting | Action | Reason |
|---|---|---|
| "From Window" combo: "Current Active Window" option | **Remove** | Depends on Latte Colors KWin Script from KDE Store — incompatible with Plasma 6. |
| "From Window" combo: "Any Touching Window" option | **Remove** | Same KWin script dependency. |
| "From Window" combo: keep only "Disabled" | **Simplify** | Single remaining option, could be removed entirely or kept as explicit default. |
| `LatteCore.WindowSystem.compositingActive` guards (5 places) | **Remove** | Compositing is always active on Wayland. Guards are always `true`. |

**Remove** `colorsGridLayout.colorsScriptIsPresent` property and the warning icon logic — no longer needed.

### 3. EffectsConfig.qml

| Setting | Action | Reason |
|---|---|---|
| `LatteCore.WindowSystem.compositingActive` guards (4 places) | **Remove** | Always true on Wayland. |

### 4. TasksConfig.qml

| Setting | Action | Reason |
|---|---|---|
| Commented-out "Recycling" section (~24 lines) | **Delete** | Dead code. |
| "Show only tasks from the current activity" (`showOnlyCurrentActivity`) | **Guard visibility** | Add `visible: KActivities.available` check instead of always showing. |
| `LatteCore.WindowSystem.compositingActive` guards (~2 places) | **Remove** | Always true on Wayland. |

### 5. LatteDockConfiguration.qml

| Setting | Action | Reason |
|---|---|---|
| `LatteCore.WindowSystem.compositingActive` in `inConfigureAppletsMode` | **Remove guard** | Always true on Wayland. |

## Items Explicitly Preserved (No Change)

- All visibility modes (Always Visible, Dodge Active, Windows Go Below, etc.)
- Dock Style (Classic/Modern)
- Length, margins, colors palette
- Background settings (thickness, opacity, radius, shadow, blur, outline, corners)
- Dynamic visibility settings
- All Effects page items (shadows, animations, indicators)
- All Tasks badges, filters (except activities), animations, scrolling, actions
- All mouse/wheel action combos (except Cycle Activities)

## Migration Strategy

- **Radical cleanup**: removed settings are deleted from UI and their config keys ignored.
- `plasmoid.configuration` values for removed keys will remain in user config files but have no effect — no migration code needed.
- If a removed `windowColors` value (1 or 2) exists, it silently falls back to default (0 = Disabled).

## Self-Review

- [x] No placeholders or TBDs
- [x] Architecture matches feature descriptions
- [x] Scope is focused — single cleanup pass
- [x] No ambiguous requirements
