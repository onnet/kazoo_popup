#ifndef DEFAULTS_H
#define DEFAULTS_H

static const char * const kUpdatesDirName = "updates";
static const char * const kBackupDirName = "backup";
static const char * const kIgnoreListFileName = "ignore-list";
static const char * const kContactEmail = "example@example.com";
static const char * const kUpdatedAppName = "Kazoo Popup";

#ifdef Q_OS_WIN
static const char * const kAutoUpdaterFileName = "autoupdater.exe";
static const char * const kUpdatedAppFileName = "KazooPopup.exe";
static const char * const kPreUpdateFileName = "pre-update.bat";
static const char * const kPostUpdateFileName = "post-update.bat";
#endif

#ifdef Q_OS_MAC
static const char * const kAutoUpdaterFileName = "autoupdater.app";
static const char * const kUpdatedAppFileName = "KazooPopup.app";
static const char * const kPreUpdateFileName = "pre-update.sh";
static const char * const kPostUpdateFileName = "post-update.sh";
#endif

#endif // DEFAULTS_H
