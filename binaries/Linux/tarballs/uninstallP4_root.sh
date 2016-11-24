#!/bin/bash

echo "== P4 uninstaller for root =="

# remove main folder
echo "Removing /opt/p4..."
[[ -f /opt/p4 ]] && sudo rm -r /opt/p4
echo "Done."

# remove symlink
echo "Removing /usr/bin/p4..."
[[ -f /usr/bin/p4 ]] && sudo rm /usr/bin/p4
echo "Done."

# remove desktop entry
echo "Removing /usr/share/applications/p4.desktop..."
[[ -f /usr/share/applications/p4.desktop ]] && sudo rm /usr/share/applications/p4.desktop
echo "Done."

# end
echo "Uninstallation complete!"