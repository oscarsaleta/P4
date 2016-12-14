#!/bin/bash

echo "== P4 uninstaller =="

# remove main folder
if [[ -d $HOME/p4 ]]; then
    echo "Removing "$HOME"/p4...";
    rm -rf $HOME/p4;
    echo "Done.";
fi

# remove desktop entry
if [[ -f $HOME/.local/share/applications/p4.desktop ]]; then
    echo "Removing $HOME/.local/share/applications/p4.desktop...";
    rm $HOME/.local/share/applications/p4.desktop;
    echo "Done.";
fi

# remove config file
if [[ -d $HOME/.config/P4 ]]; then
    echo "Removing $HOME/.config/P4...";
    rm -rf $HOME/.config/P4;
    echo "Done.";
fi


# end
echo "Uninstallation complete!"
