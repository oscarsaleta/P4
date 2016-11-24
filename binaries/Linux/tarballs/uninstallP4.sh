#!/bin/bash

echo "== P4 uninstaller =="

# remove main folder
echo "Removing "$HOME"/p4..."
[[ -f $HOME/p4 ]] && sudo rm -r $HOME/p4
echo "Done."

# remove desktop entry
echo "Removing $HOME/.local/share/applications/p4.desktop..."
[[ -f $HOME/.local/share/applications/p4.desktop ]] && sudo rm $HOME/.local/share/applications/p4.desktop
echo "Done."

# end
echo "Uninstallation complete!"