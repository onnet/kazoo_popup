# Screen Pops
## Overview
### Terminology
_2600hz:_ We are an open-source organization building large scale telecommunications software to disrupt the industry.

_Kazoo:_ This is our platform which provides telecommunications services.  Available to everyone on [github](https://github.com/2600hz/kazoo), 2600hz also offers a hosted Kazoo service.

_Kazoo API:_  The Kazoo platform exposes REST HTTP interfaces for configuration, call control, maintenance, and integration.

_Channel:_ Telephone calls are comprised of one or more channels, each channel composed of media streams and call control signaling.  E.g. If Alice is on the phone with Bob, Alice and Bob each have a channel which are linked together forming the call.

_Call-ID:_ Every channel has a unique identifier of arbitrary format. E.g:

* 1738908920_87274668@192.168.4.1
* 14670df0-f943-11e3-a3ac-0800200c9a66
* SDb9pk302-83f92fb39f0d8b41737e6ed11e4865d0-06a91d1

_Notification:_ In the scope of this document this is a notice from the Kazoo platform to the Screen-Pop Application.  The notifications will indicate when a channel is created, answered, and destroyed.

_Caller:_ The person who placed a telephone call.

_Callee:_ The person receiving a telephone call. I.e: A caller places a call to a callee.

_User:_ In the scope of this document this will refer to a Kazoo end-user who is the callee of one or more calls.

_Screen-Pop:_ This is a graphical element displayed on the user's computer screen.

### Purpose
The purpose of this project is to create a native application for the Windows and Mac operating systems which will inform a user of caller details when they receiving a call.

### Scenarios

####Download
As a user, I would want to be able to download an executable and run it without any configuration.  This should include all presets relating to my current user profile at time of download.

#### Notifications
As a user who works on my computer all day I would like to have an notice appear on my computer screen when I receive a call with all the details about who is calling. 

#### CRM Integration
As a user who works with my companies clients, when they call in I would like some way to automatically access their information in our Customer Relationship Management software.

### Research and Technical Development


## Functional Specification
### Channel Notifications
#### Create
When the application receives a notification of an incoming channel involving the user it should prominently display a screen-pop.  This screen-pop should contain:

* The reported caller's name
* The reported caller's number
* The number the caller dialed

Each screen-pop should be associated with a Call-ID for correlation with subsequent notifications.

#### Answered Notifications
In the event that the notification indicates the call was answered, the screen-pop should operate differently based on which channel answered the call.

1. If the answering channel belongs to the user, then the screen-pop should remove itself immediately.
2. If the answering channel does not belong to the user, the screen-pop should be updated with information regarding which channel answered the call.  This should include:
* The reported callee number
* The reported callee name

#### Destroy Notifications
If the application receives a notification that a known Call-ID has been destroyed the corresponding screen-pop should be removed.

### Screen-Pop Actions
Active screen-pops should allow the user to open a website via the system default browser when the application is configured with a URL template.  The website URL should be dynamically generated based on the configured template and may contain information from the channel create notification (predominately the caller's reported number).  

This should be an option that can be selected per user to either require manual activation or to always open the associated url for any call answered by the user without requiring action on the screen-pop.

### Closing Screen-Pops
A visible screen-pop should automatically remove itself after a short period of time.  User should be able to configure the timeout settings.

The user should also be able to easily take action to remove the screen-pop.   

Any time the screen-pop is updated, such as when the call is answered, the timeout should be reset.

### Multiple Calls
It is possible that a user may receive more than one call at a time.  When this happens the application should generate independent screen-pops.  

### Installation
The installation should be as simple as possible, targeted at computer illiterate users.  The minimum number of steps should be involved and ideally no external dependencies.

The installation should have the appropriate information to monitor the calls associated with this user, regardless of the devices being used. 

Installation procedure should force user to accept EULA.

Installation should allow a user to determine if the application will start automatically upon boot.

###Updates
The user should be informed when an update is available.  This should be a persistent visual reminder in the form of a modification to the standard icon in the system notification area. 

### Removal
The user should be able to easily remove the application via the standard means of the operating system.

### Running
Once installed the application should start automatically every time the system is booted.

When running a indication should remain resident in the operating system notification area.  When this indication is clicked the user can only stop the application.

The user should be able to easily access a debug log.  In the event that the user experiences an issue, the computer illiterate user should be able to easily email this log to a support representative.  This log should at minimum contain 1) attempts to connect to the server 2) any rejection messages 3) configuration at start-up 4) any log lines the program maintainers would need in order to debug an issue.

The system tray icons should change to delineate whether the program is able to make screen-pops appear and offer additional details when not able to function as desired.

Information should be transferred between the application and the backend in a secure manner. 

### Configuration 
By clicking on the tray icon, the user should be able to stop the program, access settings, get the debug log, and close all active pop-ups. 



## Technical Specification



## Wishlist

### Extensibility
