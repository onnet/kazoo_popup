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
 * ** -------------------------------------------------------------------------------------------------------------------------*/

#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QHash>
#include <QStringList>

class QNetworkAccessManager;
class QWebSocket;
class QSettings;

class ContactInfo;

class WebSocketManager : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketManager(QObject *parent = 0);
    ~WebSocketManager();

    void start();

private:
    void retrieveAuthToken();
    void processWsData(const QString &data);

    QNetworkAccessManager *m_nam = nullptr;
    QWebSocket *m_webSocket = nullptr;

    QString m_authToken;
    QString m_accountId;

    QHash<QString, QString> m_createChannelHash;
    QHash<QString, ContactInfo*> m_contactsHash;
    QMultiHash<QString, QString> m_contactCallIdHash;

    QSettings *m_settings;

signals:
    void channelCreated(ContactInfo *contactInfo);
    void channelAnswered(ContactInfo *contactInfo);
    void channelDestroyed(ContactInfo *contactInfo);

private slots:
    void retrieveAuthTokenFinished();
    void retrieveWsAddressFinished();
    void retrieveCallerInfoFinished();

    void webSocketConnected();
    void webSocketDisconnected();
    void webSocketTextMessageReceived(const QString &message);

};

#endif // WEBSOCKETMANAGER_H
