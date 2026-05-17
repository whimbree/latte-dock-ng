# Advanced Config Cleanup Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remove obsolete/unreliable configuration options and redundant compositing guards from advanced-mode config pages.

**Architecture:** Five independent QML file edits — each task modifies one file. No shared state. Build verification after all tasks.

**Tech Stack:** QML, Kirigami, Plasma Components

---

### Task 1: Cleanup BehaviorConfig.qml

**Files:**
- Modify: `shell/package/contents/configuration/pages/BehaviorConfig.qml`

- [ ] **Step 1: Remove "Activate KWin edge after hiding" checkbox and its layout**

Remove lines ~811-828 (the "Environment" section column layout containing `enableKWinEdges`). If this leaves "Environment" section empty, remove the entire section header too.

Search for `enableKWinEdges` and remove the entire `LatteComponents.CheckBox` block that contains it, plus its wrapping `ColumnLayout` if all items are removed.

- [ ] **Step 2: Remove "Raise on activity change" checkbox**

Search for `raiseOnActivity` and remove the entire `LatteComponents.CheckBox` block.

- [ ] **Step 3: Remove "Cycle Through Activities" from scroll action combo**

In the `scrollActionsListModel` ListModel (around line 640-660), remove the list element with `actionId: LatteContainment.types.ScrollActivities`.

- [ ] **Step 4: If "Environment" section becomes empty, remove the section header and column**

Check if the `ColumnLayout` for "Environment" has any remaining visible children. If not, remove the entire section including its `LatteComponents.Header { text: i18n("Environment") }` and wrapping layout.

- [ ] **Step 5: Build and verify**

```bash
cd /data/projects/latte-dock-ng && bash install.sh --user Debug 2>&1 | tail -5
```

Expected: Build succeeds, no errors.

- [ ] **Step 6: Commit**

```bash
git add shell/package/contents/configuration/pages/BehaviorConfig.qml
git commit -m "cleanup: remove obsolete KWin edge, activity raise, cycle activities from BehaviorConfig"
```

---

### Task 2: Cleanup AppearanceConfig.qml

**Files:**
- Modify: `shell/package/contents/configuration/pages/AppearanceConfig.qml`

- [ ] **Step 1: Remove "Current Active Window" and "Any Touching Window" from From Window combo model**

In the `LatteComponents.ComboBox` for `windowColors` (around line 937-968), modify the `model` array to keep only the "Disabled" option:

```qml
model: [
    {
        name: i18n("Disabled"),
        icon: "",
        toolTip: i18n("Colors are not going to take into account any windows")
    }
]
```

- [ ] **Step 2: Remove `colorsScriptIsPresent` property and related warning icon logic**

Remove the `readonly property bool colorsScriptIsPresent: universalSettings.colorsScriptIsPresent` from the GridLayout (line 881). Remove the `icon` role values that reference `state-warning` and simplify `blankSpaceForEmptyIcons` since it's no longer needed.

- [ ] **Step 3: If From Window only has one option (Disabled), consider removing the label+combo entirely**

The "From Window" label (line 934) and combo can be removed since the only option is "Disabled" — equivalent to not having the setting at all. Remove both the `PlasmaComponents.Label { text: i18n("From Window") }` and the `LatteComponents.ComboBox` blocks.

- [ ] **Step 4: Remove compositingActive guards (5 places)**

Search for `LatteCore.WindowSystem.compositingActive` and replace each `enabled:` guard with just the remaining condition (or remove if only compositing was the guard):

- Line ~294: Zoom slider — change `enabled: LatteCore.WindowSystem.compositingActive && plasmoid.configuration.animationsEnabled` to `enabled: plasmoid.configuration.animationsEnabled`
- Line ~984: Background HeaderSwitch — change `enabled: LatteCore.WindowSystem.compositingActive` to remove the guard (enabled always)
- Line ~1003: Thickness slider — `enabled: showBackground.checked` (already has this, compositing is extra)
- Line ~1200: Blur button — simplify `enabled: showBackground.checked && LatteCore.WindowSystem.compositingActive` to `enabled: showBackground.checked`
- Line ~1219: Shadows button — simplify similarly

- [ ] **Step 5: Build and verify**

```bash
cd /data/projects/latte-dock-ng && bash install.sh --user Debug 2>&1 | tail -5
```

- [ ] **Step 6: Commit**

```bash
git add shell/package/contents/configuration/pages/AppearanceConfig.qml
git commit -m "cleanup: remove KWin color script options and compositing guards from AppearanceConfig"
```

---

### Task 3: Cleanup EffectsConfig.qml

**Files:**
- Modify: `shell/package/contents/configuration/pages/EffectsConfig.qml`

- [ ] **Step 1: Remove compositingActive guards**

Search for `LatteCore.WindowSystem.compositingActive` and simplify `enabled:` bindings. In EffectsConfig, the relevant guards are:

- `enabled: LatteCore.WindowSystem.compositingActive` — remove entirely (always enabled)
- `enabled: ... && LatteCore.WindowSystem.compositingActive` — keep only the other condition

Check lines near shadows/animation enable controls.

- [ ] **Step 2: Build and verify**

```bash
cd /data/projects/latte-dock-ng && bash install.sh --user Debug 2>&1 | tail -5
```

- [ ] **Step 3: Commit**

```bash
git add shell/package/contents/configuration/pages/EffectsConfig.qml
git commit -m "cleanup: remove compositing guards from EffectsConfig"
```

---

### Task 4: Cleanup TasksConfig.qml

**Files:**
- Modify: `shell/package/contents/configuration/pages/TasksConfig.qml`

- [ ] **Step 1: Delete commented-out "Recycling" section**

Search for `//! BEGIN: Recycling` or `Recycle` and remove the entire commented-out section (~lines 718-741) including the Header, Button, and surrounding comments.

- [ ] **Step 2: Add KActivities guard to "Show only tasks from the current activity"**

Find the CheckBox for `showOnlyCurrentActivity` (around line 220). Add a `visible` binding:

```qml
visible: latteBridge && latteBridge.activitiesAvailable
```

Check the existing codebase for how KActivities availability is exposed. If no direct API, use `visible: false` and add a comment noting this needs the activities check.

- [ ] **Step 3: Remove compositingActive guards**

Search for `LatteCore.WindowSystem.compositingActive` and simplify:
- Scrolling HeaderSwitch (~line 435): `enabled: LatteCore.WindowSystem.compositingActive` → remove guard

- [ ] **Step 4: Build and verify**

```bash
cd /data/projects/latte-dock-ng && bash install.sh --user Debug 2>&1 | tail -5
```

- [ ] **Step 5: Commit**

```bash
git add shell/package/contents/configuration/pages/TasksConfig.qml
git commit -m "cleanup: delete Recycling section, guard activity filter, simplify compositing in TasksConfig"
```

---

### Task 5: Cleanup LatteDockConfiguration.qml

**Files:**
- Modify: `shell/package/contents/configuration/LatteDockConfiguration.qml`

- [ ] **Step 1: Remove compositingActive from inConfigureAppletsMode**

Line ~47: `readonly property bool inConfigureAppletsMode: universalSettings.inConfigureAppletsMode || !LatteCore.WindowSystem.compositingActive`

Simplify to: `readonly property bool inConfigureAppletsMode: universalSettings.inConfigureAppletsMode`

- [ ] **Step 2: Remove compositingActive from kirigamiLibraryIsFound (if present)**

Lines ~49: check if `kirigamiLibraryIsFound` uses compositing. Should be `true` always.

- [ ] **Step 3: Build and verify**

```bash
cd /data/projects/latte-dock-ng && bash install.sh --user Debug 2>&1 | tail -5
```

- [ ] **Step 4: Commit**

```bash
git add shell/package/contents/configuration/LatteDockConfiguration.qml
git commit -m "cleanup: remove compositing guard from LatteDockConfiguration"
```

---

### Final Verification

- [ ] **Rebuild and start debug instance**

```bash
cd /data/projects/latte-dock-ng && bash install.sh --user Debug
rm -f /tmp/latte-ng.log
nohup bash -c 'source ~/.config/latte-dock-ng/dev-env.sh && ~/.local/bin/latte-dock-ng --replace --debug' &>/tmp/latte-ng.log &
sleep 4; pgrep -x latte-dock-ng && echo "OK" || echo "FAILED"
```

- [ ] **Check log for errors**

```bash
grep -cE "\[Error|\[Warning" /tmp/latte-ng.log
```

Expected: No new errors or warnings beyond pre-existing ones.

- [ ] **Push all commits**

```bash
git push
```
