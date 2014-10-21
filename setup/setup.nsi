!define PRODUCT_NAME "KazooPopup"
!define PRODUCT_VERSION "0.1.0"
!define PRODUCT_WEB_SITE "http://2600hz.org/"
!define PRODUCT_PUBLISHER "2600hz"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKCU"

!include "MUI.nsh"
!include nsdialogs.nsh

!define MUI_ABORTWARNING
!define MUI_ICON "kazoo_32.ico"
!define MUI_UNICON "kazoo_32.ico"

;Set compressor
SetCompressor lzma

!insertmacro MUI_PAGE_WELCOME

!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCT_NAME}.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME}Setup-${PRODUCT_VERSION}.exe"

InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
ShowInstDetails show
ShowUnInstDetails show

Function .onInit
  nsExec::ExecToStack '"$SYSDIR\cmd.exe" /C tasklist /NH /FI "IMAGENAME eq ${PRODUCT_NAME}.exe" | find /C "${PRODUCT_NAME}.exe"'
  Pop $1
  Pop $2
  ${If} $2 = 1
    MessageBox MB_ICONINFORMATION|MB_OK "${PRODUCT_NAME} is already run. Please, close program and try again later."
    Abort
  ${EndIf}
FunctionEnd

Section Install 
  SetShellVarContext all
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "${PRODUCT_NAME}.exe"
  File "Qt5Core.dll"
  File "Qt5Gui.dll"
  File "Qt5Network.dll"
  File "Qt5WebSockets.dll"
  File "Qt5Widgets.dll"
  File "libeay32.dll"
  File "ssleay32.dll"
  File "libgcc_s_dw2-1.dll"
  File "libstdc++-6.dll"
  File "libwinpthread-1.dll"
  File "icudt52.dll"
  File "icuin52.dll"
  File "icuuc52.dll"
  File "readme.txt"
  File /oname=$LOCALAPPDATA\${PRODUCT_NAME}\settings.ini "settings.ini"
  
  CreateDirectory "$INSTDIR\platforms"
  File /oname=platforms\qminimal.dll "platforms\qminimal.dll"
  File /oname=platforms\qwindows.dll "platforms\qwindows.dll"
  
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe"
  
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  nsExec::ExecToStack '"$SYSDIR\cmd.exe" /C tasklist /NH /FI "IMAGENAME eq ${PRODUCT_NAME}.exe" | find /C "${PRODUCT_NAME}.exe"'
  Pop $1
  Pop $2
  ${If} $2 = 1
    MessageBox MB_ICONINFORMATION|MB_OK "${PRODUCT_NAME} is already run. Please, close program and uninstall later."
    Abort
  ${EndIf}
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd


Section Uninstall
  SetShellVarContext all
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\${PRODUCT_NAME}.exe"
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5Gui.dll"
  Delete "$INSTDIR\Qt5Network.dll"
  Delete "$INSTDIR\Qt5WebSockets.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\libeay32.dll"
  Delete "$INSTDIR\ssleay32.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\icudt52.dll"
  Delete "$INSTDIR\icuin52.dll"
  Delete "$INSTDIR\icuuc52.dll"
  Delete "$INSTDIR\readme.txt"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$LOCALAPPDATA\${PRODUCT_NAME}\settings.ini"
  Delete "$INSTDIR\platforms\qminimal.dll"
  Delete "$INSTDIR\platforms\qwindows.dll"
  RMDir "$INSTDIR\translations"
  RMDir /r "$INSTDIR"

  ;Delete Start Menu Shortcuts
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\*.*"
  RmDir  "$SMPROGRAMS\${PRODUCT_NAME}"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"

  SetAutoClose true
SectionEnd