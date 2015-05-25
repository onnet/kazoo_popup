#ifndef DEFAULTS_H
#define DEFAULTS_H

class QString;

static const char * const kLogin = "testuser1";
static const char * const kPassword = "temp1phone!";
static const char * const kRealm = "your_account_name.sip.onnet.su";
static const char * const kAuthUrl = "https://sip.onnet.su:8443";
static const char * const kCrossbarUrl = "https://sip.onnet.su:8443";
static const char * const kEventUrl = "https://sip.onnet.su:7777";
static const char * const kInfoUrl = "https://onnet.su/popup_login?login_name";
static const char * const kCrmUrl = "https://onnet.su/api/onnet/cidinfo?calling_number={{Caller-ID-Number}}";
static const char * const kCrmHash = "vhbxzj947yisfbl";
static const int kPopupTimeout = 50;
static const int kCallDirection = 2;
static const int kOpenUrl = 0;
static const bool kRunAtStartup = true;

static const char * const kRegistryKeyRun = "HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

QString dataDirPath();
QString logsDirPath();
QString detectPlatform();

#endif // DEFAULTS_H
