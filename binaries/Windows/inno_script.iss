#define MyAppName "P4"
#define MyAppVersion "3.3.1"
#define MyAppPublisher "UAB-UHasselt"
#define MyAppURL "http://github.com/oscarsaleta/P4"
#define MyAppExeName "bin\p4.exe"
[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{1567A622-CF41-4DB9-A4EF-998D3DE1E50A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=C:\Users\Oscar\git\P4\LICENSE
;InfoBeforeFile=C:\Users\Oscar\git\P4\README.md
OutputBaseFilename=P4setup_v3.3.1
SetupIconFile=p4_windows_3.3.1\p4\bin\p4smallicon.ico
UninstallDisplayIcon={app}\bin\p4smallicon.ico
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Dirs]
Name: "{app}"; Permissions: users-readexec
Name: "{app}\bin"; Permissions: users-readexec
Name: "{app}\help"; Permissions: users-readexec
Name: "{app}\sumtables"; Permissions: users-modify
Name: "{userdocs}\P4"; Permissions: users-full

[Files]
; Executables
Source: "p4_windows_3.3.1\p4\bin\p4.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "p4_windows_3.3.1\p4\bin\separatrice.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "p4_windows_3.3.1\p4\bin\lyapunov.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "p4_windows_3.3.1\p4\bin\p4.m"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "p4_windows_3.3.1\p4\bin\p4gcf.m"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "p4_windows_3.3.1\p4\help\*"; DestDir: "{app}\help"; Flags: ignoreversion
; Icons
Source: "p4_windows_3.3.1\p4\bin\*.png"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "p4_windows_3.3.1\p4\bin\p4smallicon.ico"; DestDir: "{app}\bin"; Flags: ignoreversion
; VC Redist
Source: "p4_windows_3.3.1\p4\bin\vcredist_x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
; .dlls
Source: "p4_windows_3.3.1\p4\bin\*.dll"; DestDir: "{app}\bin"; Flags: onlyifdoesntexist
Source: "p4_windows_3.3.1\p4\bin\iconengines\*.dll"; DestDir: "{app}\bin\iconengines"; Flags: onlyifdoesntexist
Source: "p4_windows_3.3.1\p4\bin\imageformats\*.dll"; DestDir: "{app}\bin\imageformats"; Flags: onlyifdoesntexist
Source: "p4_windows_3.3.1\p4\bin\platforms\qwindows.dll"; DestDir: "{app}\bin\platforms"; Flags: onlyifdoesntexist
Source: "p4_windows_3.3.1\p4\bin\printsupport\windowsprintersupport.dll"; DestDir: "{app}\bin\printsupport"; Flags: onlyifdoesntexist

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{userdocs}\P4"; IconFilename: "{app}\bin\p4smallicon.ico"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; WorkingDir: "{userdocs}\P4"; IconFilename: "{app}\bin\p4smallicon.ico"

[Run]
Filename: "{tmp}\vcredist_x64.exe"; Parameters: "/quiet /install /norestart"; WorkingDir: "{tmp}"; StatusMsg: "Installing Microsoft Visual C++ 2015 Redistributable (x64)..."
Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{userdocs}\P4"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
