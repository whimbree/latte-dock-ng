# Transparent Edit Mode Design

**Date:** 2026-05-17
**Status:** Approved

## Motivation

Plasma 6's built-in containment edit mode draws a blue/grid overlay that blocks the dock preview. Users want to edit applet configuration while seeing the dock transparently.

## Design

### Entry/Exit
- Right-click dock context menu → "Edit Dock" sets `universalSettings.inConfigureAppletsMode = true`
- Does NOT set `plasmoid.userConfiguring` (avoids Plasma's blue overlay)
- Right-click → "Exit Edit Dock" sets it back to `false`

### Edit Mode Behavior
- Dock renders normally — transparent background, all visual effects as configured
- Parabolic zoom: **disabled** during edit mode (existing behavior, unchanged)
- Splitters visible, applet paddings shown (existing behavior)

### AppletHoverTooltip
A lightweight overlay that appears when hovering an applet in edit mode:

- **Content**: applet name label + [Configure] [Colorize] [Lock/Unlock] [Remove] buttons
- **Positioning**: anchored near the hovered applet, respecting dock edge (bottom/top/left/right)
- **Show/hide**: appears on hover, hides after a delay when mouse leaves
- **Button actions**:
  - Configure: opens applet's Plasma configuration dialog
  - Colorize: toggles `userBlocksColorizing` for the applet
  - Lock: toggles `lockZoom` for the applet
  - Remove: triggers applet removal

### Changes

#### main.qml
1. `inConfigureAppletsMode` — remove `root.editMode &&` dependency:
   ```
   readonly property bool inConfigureAppletsMode: universalSettings && universalSettings.inConfigureAppletsMode
   ```
2. Add `AppletHoverTooltip` component (replaces removed ConfigOverlay)

#### New file: editmode/AppletHoverTooltip.qml
Minimal overlay component:
- MouseArea over entire containment to detect hovered applets
- PlasmaCore.Dialog tooltip showing applet name + action buttons
- No drag-and-drop logic, no resize handles, no placeholders

#### Context menu (LatteBridge or ContextMenu)
Add "Edit Dock" / "Exit Edit Dock" menu items that toggle `universalSettings.inConfigureAppletsMode`

#### ParabolicEffect.qml
Unchanged — already disabled during `inConfigureAppletsMode`

### Items NOT Changed
- PaddingsInConfigureApplets (existing behavior, now active in transparent mode)
- Parabolic effect disabling (existing behavior)
- All other `inConfigureAppletsMode` consumers
