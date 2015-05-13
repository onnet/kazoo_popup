kazoo_popup
===========

Ordered by Kirill Sysoev kirill.sysoev@gmail.com
(OnNet communications Inc. http://onnet.su)

Developed by Alexey Lysenko lysenkoalexmail@gmail.com

Please report bugs and provide any possible patches directly to this repository.

If you would like to order additional development, contact Alexey Lysenko over email lysenkoalexmail@gmail.com directly.


This application:
 - connects to Kazoo whapp blackhole;
 - listens for incoming calls;
 - queries third party server whether it knows anything about caller's number;
 - Pop's Up window with provided info.

It is:
 - written in Qt which promises to be crossplatform application (tested on Linux and Windows);
 - is NOT production ready, but intended to be a simple example of using blachole whapp
   (please note, that blackhole whapp doesn't support secure connectoin over SSL yet; check KAZOO-2632).

Good luck!


Win:

![Alt text](/design/Mockups/kazoo_popup.png "Screenshot Windows")


Mac:

![Alt text](/design/Mockups/kazoo_popup_mac.png "Screenshot Mac")


Linux:

![Alt text](/design/Mockups/kazoo_popup_linux.png "Screenshot Linux")
