#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QHash>

class QNetworkAccessManager;
class QWebSocket;
class QSettings;

class Caller;

class WebSocketManager : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketManager(QObject *parent = 0);
    ~WebSocketManager();

    void start();
    void stop();

private:
    void retrieveAuthToken();
    void processWsData(const QString &data);

    QNetworkAccessManager *m_nam = nullptr;
    QWebSocket *m_webSocket = nullptr;

    QString m_authToken;
    QString m_accountId;

    QHash<QString, Caller> m_callersHash;

    QHash<QString, QString> m_callIdAndOtherLegHash;

    QSettings *m_settings = nullptr;

signals:
    void channelCreated(const QString &callId, const Caller &caller);
    void channelAnswered(const QString &callId);
    void channelAnsweredAnother(const QString &callId, const QString &calleeNumber, const QString &calleeName);
    void channelDestroyed(const QString &callId);

private slots:
    void retrieveAuthTokenFinished();
    void retrieveWsAddressFinished();

    void webSocketConnected();
    void webSocketDisconnected();
    void webSocketTextMessageReceived(const QString &message);
};

#endif // WEBSOCKETMANAGER_H
