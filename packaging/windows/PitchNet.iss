; Sonaria Windows installer.
; Build: iscc /DMyAppVersion=<version> packaging\windows\PitchNet.iss

#define MyAppName "Sonaria"
#ifndef MyAppVersion
  #define MyAppVersion "0.9.0"
#endif
#define MyAppPublisher "NXE"
#define MyAppURL "https://github.com/narusenia/sonaria"
#define MyAppExeName "Sonaria.exe"

[Setup]
; This script lives in packaging\windows. SourceDir points back at the repository
; root so every relative path below (build\..., Resources\..., Output\...) is
; resolved from the repo root.
SourceDir=..\..
OutputDir=Output
AppId={{723F4B65-11A4-4813-80A7-39BB7B9E095E}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputBaseFilename=Sonaria-{#MyAppVersion}-Windows-Setup
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
SetupIconFile=build\PitchNet_artefacts\JuceLibraryCode\icon.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
LicenseFile=LICENSE
InfoBeforeFile=THIRD_PARTY_NOTICES.txt

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "main"; Description: "Models, license and notices (required)"; Types: full custom; Flags: fixed
Name: "standalone"; Description: "Standalone application"; Types: full
Name: "vst3"; Description: "VST3 plug-in"; Types: full
#ifdef WithAAX
Name: "aax"; Description: "AAX plug-in"; Types: full
#endif

[Files]
Source: "LICENSE"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "THIRD_PARTY_NOTICES.txt"; DestDir: "{app}"; Components: main; Flags: ignoreversion
Source: "build\PitchNet_artefacts\Release\{#MyAppExeName}"; DestDir: "{app}"; Components: standalone; Flags: ignoreversion
; onnxruntime.dll and onnxruntime_providers_shared.dll, copied next to the exe by CMake.
Source: "build\PitchNet_artefacts\Release\*.dll"; DestDir: "{app}"; Components: standalone; Flags: ignoreversion
Source: "build\PitchNetPlugin_artefacts\Release\VST3\Sonaria.vst3\*"; DestDir: "{code:getVST3Dir}"; Components: vst3; Flags: ignoreversion recursesubdirs createallsubdirs
#ifdef WithAAX
Source: "build\PitchNetPlugin_artefacts\Release\AAX\Sonaria.aaxplugin\*"; DestDir: "{commoncf64}\Avid\Audio\Plug-Ins\{#MyAppName}.aaxplugin"; Components: aax; Flags: ignoreversion recursesubdirs createallsubdirs
#endif
; Release builds load models from here first (Source/Utils/PlatformPaths.h).
Source: "Resources\models\*"; DestDir: "{commonappdata}\{#MyAppPublisher}\{#MyAppName}\models"; Components: main; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "Resources\fonts\Montserrat-Medium.ttf"; DestDir: "{commonfonts}"; FontInstall: "Montserrat-Medium"; Components: main; Flags: onlyifdoesntexist uninsneveruninstall
Source: "Resources\fonts\Montserrat-Regular.ttf"; DestDir: "{commonfonts}"; FontInstall: "Montserrat-Regular"; Components: main; Flags: onlyifdoesntexist uninsneveruninstall

[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Components: standalone

[Code]
var
  VST3Page: TInputDirWizardPage;

procedure InitializeWizard;
begin
  VST3Page := CreateInputDirPage(wpSelectComponents,
    'Select VST3 Plug-in Folder',
    'Where should the VST3 plug-in be installed?',
    'The VST3 plug-in will be installed in the following folder.',
    False, '');
  VST3Page.Add('');
  VST3Page.Values[0] := ExpandConstant('{commoncf64}\VST3');
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := (PageID = VST3Page.ID) and not WizardIsComponentSelected('vst3');
end;

function getVST3Dir(Param: String): String;
begin
  Result := AddBackslash(VST3Page.Values[0]) + '{#MyAppName}.vst3';
end;
