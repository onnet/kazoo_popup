/* --------------------------------------------------------------------------------------------------------------------------
 * ** 
 * ** Ordered by Kirill Sysoev kirill.sysoev@gmail.com
 * ** (OnNet communications Inc. http://onnet.su)
 * ** 
 * ** Developed by Alexey Lysenko lysenkoalexmail@gmail.com
 * ** 
 * ** Please report bugs and provide any possible patches directly to this repository: https://github.com/onnet/kazoo_popup.git
 * ** 
 * ** If you would like to order additional development, contact Alexey Lysenko over email lysenkoalexmail@gmail.com directly.
 * ** 
 * ** 
 * ** This application:
 * **  - connects to Kazoo whapp blackhole;
 * **  - listens for incoming calls;
 * **  - queries third party server whether it knows anything about caller's number;
 * **  - Pop's Up window with provided info.
 * ** 
 * ** It is:
 * **  - written in Qt which promises to be crossplatform application (hopefully);
 * **  - is NOT production ready, but intended to be a simple example of using blachole whapp
 * **    (please note, that blackhole whapp doesn't support secure connectoin over SSL yet; check KAZOO-2632).
 * ** 
 * ** Good luck!
 * ** 
 * ** ------------------------------------------------------------------------------------------------------------------------*/

#ifndef DEFAULTS_H
#define DEFAULTS_H

static const char * const kLogin = "admin";
static const char * const kPassword = "Black7Hole$5";
static const char * const kRealm = "blackhole.sip3.onnet.su";
static const char * const kAuthUrl = "https://kz527.onnet.su:8443/v1/user_auth";
static const char * const kEventUrl = "http://kz527.onnet.su:5555";
static const char * const kInfoUrl = "https://onnet.su:8443/api/zonnet/cidinfo";
static const char * const kMd5Hash = "vhbxzj947yisfbl";

#endif // DEFAULTS_H
