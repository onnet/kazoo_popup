#ifndef DEFAULTS_H
#define DEFAULTS_H

class QString;

static const char * const kLogin = "testuser1";
static const char * const kPassword = "temp1phone!";
static const char * const kRealm = "a1722b.sip.sandbox.2600hz.com";
static const char * const kAuthUrl = "http://api.sandbox.2600hz.com:8000";
static const char * const kCrossbarUrl = "http://api.sandbox.2600hz.com:8000";
static const char * const kEventUrl = "http://api.sandbox.2600hz.com:5555";
static const char * const kInfoUrl = "http://localhost/kazoo_popup.php?caller_number={{Caller-ID-Number}}";
static const char * const kCrmUrl = "https://onnet.su/login?login_name={{Caller-ID-Number}}";
static const char * const kCrmHash = "vhbxzj947yisfbl";
static const int kPopupTimeout = 30;
static const int kCallDirection = 0;
static const int kOpenUrl = 0;
static const bool kRunAtStartup = true;

static const char * const kRegistryKeyRun = "HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

QString dataDirPath();
QString logsDirPath();
QString detectPlatform();

#endif // DEFAULTS_H
