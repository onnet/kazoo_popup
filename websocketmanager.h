#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QHash>

class QNetworkAccessManager;
class QWebSocket;
class QSettings;
class QTimer;

class Caller;

class WebSocketManager : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketManager(QObject *parent = 0);
    ~WebSocketManager();

public slots:
    void start();
    void stop();

private:
    void retrieveAuthToken();
    void processWsData(const QString &data);

    QNetworkAccessManager *m_nam = nullptr;
    QWebSocket *m_webSocket = nullptr;
    QTimer *m_timer;
    QSettings *m_settings = nullptr;
    qint64 m_lastPing = 0;

    QString m_authToken;
    QString m_accountId;

    QHash<QString, Caller> m_callersHash;
    QHash<QString, QString> m_callIdAndOtherLegHash;

signals:
    void channelCreated(const QString &callId, const Caller &caller);
    void channelAnswered(const QString &callId);
    void channelAnsweredAnother(const QString &callId, const QString &calleeNumber, const QString &calleeName);
    void channelDestroyed(const QString &callId);

    void connectionError();
    void connected();

private slots:
    void retrieveAuthTokenFinished();
    void retrieveWsAddressFinished();

    void webSocketConnected();
    void webSocketDisconnected();
    void webSocketTextMessageReceived(const QString &message);

    void checkPingTimeout();
    void handleConnectionError();
};

#endif // WEBSOCKETMANAGER_H
