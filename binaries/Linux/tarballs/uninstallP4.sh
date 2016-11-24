#!/bin/bash

echo "== P4 uninstaller =="

# remove main folder
echo "Removing "$HOME"/p4..."
rm -r $HOME/p4
echo "Done."

# remove desktop entry
echo "Removing $HOME/.local/share/applications/p4.desktop..."
rm $HOME/.local/share/applications/p4.desktop
echo "Done."

# end
echo "Uninstallation complete!"