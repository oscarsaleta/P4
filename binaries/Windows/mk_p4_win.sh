#  This file is part of P4
# 
#  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
#                           F. Dumortier, J. Llibre, O. Saleta
# 
#  P4 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#!/bin/bash

VERSION="$(awk '/VERSION.*".*"/{print $NF}' ~/git/P4/QtCreator/version.h | awk -F'"' '$0=$2')"
DIR=p4_windows_$VERSION/p4
DIRWIN="p4_windows_$VERSION\p4"
rm -r $DIR

mkdir -p $DIR/bin $DIR/help $DIR/sum_tables $DIR/sumtables
chmod 777 $DIR/sumtables

cp ../../src-mpl/p4*m $DIR/bin/
cp ../../QtCreator/build-p4-Windows-Release/release/p4.exe $DIR/bin/
cp ../../QtCreator/build-lyapunov-Windows-Release/release/lyapunov.exe $DIR/bin/
cp ../../QtCreator/build-separatrice-Windows-Release/separatrice.exe $DIR/bin/
cp ../../help/*.* $DIR/help/
cp -r ../../help/screenshots/ $DIR/help/
cp ../../help/p4_flag.png $DIR/bin/portrait.png
#cp ../../help/p4smallicon.png $DIR/bin/p4smallicon.png
#cp ../../help/p4smallicon.ico $DIR/bin/p4smallicon.ico
#cp ../../help/newp4icon.png $DIR/bin/p4smallicon.png
cp ../../help/newp4icon.ico $DIR/bin/p4smallicon.ico

# solve dlls
"C:\Qt\5.7\msvc2015_64\bin\windeployqt.exe" $DIR/bin/p4.exe

# create setup script
SCRIPTNAME=inno_script.iss
if [ -f $SCRIPTNAME ]; then
    rm $SCRIPTNAME
fi
echo '#define MyAppName "P4"' >> $SCRIPTNAME
echo '#define MyAppVersion "'$VERSION'"' >> $SCRIPTNAME
echo '#define MyAppPublisher "UAB-UHasselt"' >> $SCRIPTNAME
echo '#define MyAppURL "http://github.com/oscarsaleta/P4"' >> $SCRIPTNAME
echo '#define MyAppExeName "bin\p4.exe"' >> $SCRIPTNAME

echo '[Setup]' >> $SCRIPTNAME
echo '; NOTE: The value of AppId uniquely identifies this application.' >> $SCRIPTNAME
echo '; Do not use the same AppId value in installers for other applications.' >> $SCRIPTNAME
echo '; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)' >> $SCRIPTNAME
echo 'AppId={{1567A622-CF41-4DB9-A4EF-998D3DE1E50A}' >> $SCRIPTNAME
echo 'AppName={#MyAppName}' >> $SCRIPTNAME
echo 'AppVersion={#MyAppVersion}' >> $SCRIPTNAME
echo ';AppVerName={#MyAppName} {#MyAppVersion}' >> $SCRIPTNAME
echo 'AppPublisher={#MyAppPublisher}' >> $SCRIPTNAME
echo 'AppPublisherURL={#MyAppURL}' >> $SCRIPTNAME
echo 'AppSupportURL={#MyAppURL}' >> $SCRIPTNAME
echo 'AppUpdatesURL={#MyAppURL}' >> $SCRIPTNAME
echo 'DefaultDirName={pf}\{#MyAppName}' >> $SCRIPTNAME
echo 'DisableProgramGroupPage=yes' >> $SCRIPTNAME
echo 'LicenseFile=C:\Users\Oscar\git\P4\LICENSE' >> $SCRIPTNAME
echo ';InfoBeforeFile=C:\Users\Oscar\git\P4\README.md' >> $SCRIPTNAME
echo 'OutputBaseFilename=P4setup_v'$VERSION >> $SCRIPTNAME
echo 'SetupIconFile='$DIRWIN'\bin\p4smallicon.ico' >> $SCRIPTNAME
echo 'UninstallDisplayIcon={app}\bin\p4smallicon.ico' >> $SCRIPTNAME
echo 'Compression=lzma' >> $SCRIPTNAME
echo 'SolidCompression=yes' >> $SCRIPTNAME
echo 'ArchitecturesInstallIn64BitMode=x64' >> $SCRIPTNAME
echo 'ArchitecturesAllowed=x64' >> $SCRIPTNAME
echo '' >> $SCRIPTNAME
echo '[Languages]' >> $SCRIPTNAME
echo 'Name: "english"; MessagesFile: "compiler:Default.isl"' >> $SCRIPTNAME
echo '' >> $SCRIPTNAME
echo '[Tasks]' >> $SCRIPTNAME
echo 'Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked' >> $SCRIPTNAME
echo '' >> $SCRIPTNAME
echo '[Dirs]' >> $SCRIPTNAME
echo 'Name: "{app}"; Permissions: users-readexec' >> $SCRIPTNAME
echo 'Name: "{app}\bin"; Permissions: users-readexec' >> $SCRIPTNAME
echo 'Name: "{app}\help"; Permissions: users-readexec' >> $SCRIPTNAME
echo 'Name: "{app}\sumtables"; Permissions: users-modify' >> $SCRIPTNAME
echo 'Name: "{userdocs}\P4"; Permissions: users-full' >> $SCRIPTNAME
echo '' >> $SCRIPTNAME
echo '[Files]' >> $SCRIPTNAME
echo '; Executables' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\p4.exe"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\separatrice.exe"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\lyapunov.exe"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\p4.m"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\p4gcf.m"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\help\*"; DestDir: "{app}\help"; Flags: ignoreversion' >> $SCRIPTNAME
echo '; Icons' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\*.png"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\p4smallicon.ico"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
#echo 'Source: "'$DIRWIN'\bin\p4bigicon.ico"; DestDir: "{app}\bin"; Flags: ignoreversion' >> $SCRIPTNAME
echo '; VC Redist' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\vcredist_x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall' >> $SCRIPTNAME
echo '; .dlls' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\*.dll"; DestDir: "{app}\bin"; Flags: onlyifdoesntexist' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\iconengines\*.dll"; DestDir: "{app}\bin\iconengines"; Flags: onlyifdoesntexist' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\imageformats\*.dll"; DestDir: "{app}\bin\imageformats"; Flags: onlyifdoesntexist' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\platforms\qwindows.dll"; DestDir: "{app}\bin\platforms"; Flags: onlyifdoesntexist' >> $SCRIPTNAME
echo 'Source: "'$DIRWIN'\bin\printsupport\windowsprintersupport.dll"; DestDir: "{app}\bin\printsupport"; Flags: onlyifdoesntexist' >> $SCRIPTNAME
echo '' >> $SCRIPTNAME
echo '[Icons]' >> $SCRIPTNAME
echo 'Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{userdocs}\P4"; IconFilename: "{app}\bin\p4smallicon.ico"' >> $SCRIPTNAME
echo 'Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; WorkingDir: "{userdocs}\P4"; IconFilename: "{app}\bin\p4smallicon.ico"' >> $SCRIPTNAME
echo '' >> $SCRIPTNAME
echo '[Run]' >> $SCRIPTNAME
echo 'Filename: "{tmp}\vcredist_x64.exe"; Parameters: "/quiet /install /norestart"; WorkingDir: "{tmp}"; StatusMsg: "Installing Microsoft Visual C++ 2015 Redistributable (x64)..."' >> $SCRIPTNAME
echo 'Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{userdocs}\P4"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '\''&'\'', '\''&&'\'')}}"; Flags: nowait postinstall skipifsilent' >> $SCRIPTNAME

iscc.exe $SCRIPTNAME
