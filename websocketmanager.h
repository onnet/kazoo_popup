#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QHash>
#include <QStringList>

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
    void processChannelCreate(const QJsonObject &args);
    void processChannelAnswer(const QJsonObject &args);
    void processChannelDestroy(const QJsonObject &args);

    bool isSupportCallDirection(const QString &callDirection);

    QNetworkAccessManager *m_nam = nullptr;
    QWebSocket *m_webSocket = nullptr;
    QTimer *m_timer = nullptr;
    QSettings *m_settings = nullptr;
    qint64 m_lastPing = 0;

    QString m_authToken;
    QString m_accountId;
    QStringList m_devices;

    QHash<QString, Caller> m_callersHash;

signals:
    void channelCreated(const QString &callId, const Caller &caller);
    void channelAnswered(const QString &callId);
    void channelAnsweredAnother(const QString &callId, const QString &calleeNumber, const QString &calleeName);
    void channelDestroyed(const QString &callId);

    void connectionError();
    void connected();

private slots:
    void retrieveAuthTokenFinished();
    void retrieveDevicesFinished();
    void retrieveWsAddressFinished();

    void retrieveCrmInfoFinished();

    void webSocketConnected();
    void webSocketDisconnected();
    void webSocketTextFrameReceived(const QString &frame);

    void checkPingTimeout();
    void handleConnectionError();
};

#endif // WEBSOCKETMANAGER_H
