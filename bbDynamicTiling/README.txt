bbDynamicTiling
A manual tiling window manager for Blackbox for Windows, inspired by the 'i3' window manager for X11

# ================
# Append below lines to bbKeys.rc
# ================

# bbDynamicTiling

# toggle plugin
KeyToGrab(T), WithModifier(Ctrl+Alt), WithAction(exec @BBCfg.plugin.load bbDynamicTiling)

# focus
KeyToGrab(H), WithModifier(Win), WithAction(exec @bbDynamicTiling.focus.left)
KeyToGrab(J), WithModifier(Win), WithAction(exec @bbDynamicTiling.focus.down)
KeyToGrab(K), WithModifier(Win), WithAction(exec @bbDynamicTiling.focus.up)
KeyToGrab(L), WithModifier(Win), WithAction(exec @bbDynamicTiling.focus.right)
KeyToGrab(H), WithModifier(Alt), WithAction(exec @bbDynamicTiling.focus.container.left)
KeyToGrab(J), WithModifier(Alt), WithAction(exec @bbDynamicTiling.focus.container.down)
KeyToGrab(K), WithModifier(Alt), WithAction(exec @bbDynamicTiling.focus.container.up)
KeyToGrab(L), WithModifier(Alt), WithAction(exec @bbDynamicTiling.focus.container.right)

# move
KeyToGrab(H), WithModifier(Win+Shift), WithAction(exec @bbDynamicTiling.move.left)
KeyToGrab(J), WithModifier(Win+Shift), WithAction(exec @bbDynamicTiling.move.down)
KeyToGrab(K), WithModifier(Win+Shift), WithAction(exec @bbDynamicTiling.move.up)
KeyToGrab(L), WithModifier(Win+Shift), WithAction(exec @bbDynamicTiling.move.right)
KeyToGrab(H), WithModifier(Alt+Shift), WithAction(exec @bbDynamicTiling.move.container.left)
KeyToGrab(J), WithModifier(Alt+Shift), WithAction(exec @bbDynamicTiling.move.container.down)
KeyToGrab(K), WithModifier(Alt+Shift), WithAction(exec @bbDynamicTiling.move.container.up)
KeyToGrab(L), WithModifier(Alt+Shift), WithAction(exec @bbDynamicTiling.move.container.right)

# expand
KeyToGrab(H), WithModifier(Win+Ctrl), WithAction(exec @bbDynamicTiling.expand.left)
KeyToGrab(J), WithModifier(Win+Ctrl), WithAction(exec @bbDynamicTiling.expand.down)
KeyToGrab(K), WithModifier(Win+Ctrl), WithAction(exec @bbDynamicTiling.expand.up)
KeyToGrab(L), WithModifier(Win+Ctrl), WithAction(exec @bbDynamicTiling.expand.right)

# resize
KeyToGrab(H), WithModifier(Win+Alt), WithAction(exec @bbDynamicTiling.resize.left)
KeyToGrab(J), WithModifier(Win+Alt), WithAction(exec @bbDynamicTiling.resize.down)
KeyToGrab(K), WithModifier(Win+Alt), WithAction(exec @bbDynamicTiling.resize.up)
KeyToGrab(L), WithModifier(Win+Alt), WithAction(exec @bbDynamicTiling.resize.right)

# toggle fullscreen
KeyToGrab(F), WithModifier(Win), WithAction(exec @bbDynamicTiling.fullscreen)

# toggle floating
KeyToGrab(F), WithModifier(Win+Shift), WithAction(exec @bbDynamicTiling.floating)
