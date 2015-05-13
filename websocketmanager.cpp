#include "websocketmanager.h"

#include "defaults.h"
#include "caller.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QSettings>

#include <QTimer>

static const char * const kRetrieveWsPath = "/socket.io/1/";
static const char * const kWsPath = "/socket.io/1/websocket/";
static const char * const kAuthPath = "/v1/user_auth";
static const char * const kCrossbarPath = "/v1/user_auth/accounts/%1/devices?filter_owner_id=%2";
static const char * const kWsScheme = "wss";

static const int kCheckPingTimeout = 30000;
static const int kPermittedPingTimeout = 15000;

enum CallDirection
{
    kCallDirectionInbound,
    kCallDirectionOutbound
};

QString socketErrorToString(QAbstractSocket::SocketError error);

WebSocketManager::WebSocketManager(QObject *parent) :
    QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            this, &WebSocketManager::checkPingTimeout);
}

WebSocketManager::~WebSocketManager()
{
    if (m_webSocket)
        m_webSocket->deleteLater();

    if (m_settings)
        m_settings->deleteLater();
}

void WebSocketManager::start()
{
    m_timer->start(kCheckPingTimeout);
    m_callersHash.clear();

    if (m_settings)
        m_settings->deleteLater();

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);

    if (m_webSocket)
    {
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }

    retrieveAuthToken();
}

void WebSocketManager::stop()
{
    m_timer->stop();

    m_callersHash.clear();

    if (m_settings)
    {
        m_settings->deleteLater();
        m_settings = nullptr;
    }

    if (m_webSocket)
    {
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }
}

void WebSocketManager::retrieveAuthToken()
{
    QByteArray hashTemplate(m_settings->value("login", kLogin).toByteArray());
    qDebug("Login: %s", hashTemplate.data());
    hashTemplate.append(":");
    hashTemplate.append(m_settings->value("password", kPassword).toString());

    QByteArray hash = QCryptographicHash::hash(hashTemplate, QCryptographicHash::Md5).toHex();
    QJsonObject jsonObject;
    QJsonObject jsonData;
    jsonData["credentials"] = hash.data();
    jsonData["realm"] = m_settings->value("realm", kRealm).toString();
    qDebug("Realm: %s", jsonData["realm"].toString().toLatin1().data());
    jsonObject["data"] = jsonData;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray json = jsonDocument.toJson();

    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);

    QString authUrl(m_settings->value("auth_url", kAuthUrl).toString());
    authUrl.append(kAuthPath);
    qDebug("Auth url: %s", authUrl.toLatin1().data());
    req.setUrl(QUrl(authUrl));

    QNetworkReply *reply = m_nam->put(req, json);
    connect(reply, &QNetworkReply::finished,
            this, &WebSocketManager::retrieveAuthTokenFinished);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(handleConnectionError()));
}

void WebSocketManager::retrieveAuthTokenFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug("%s, NetworkReply error: %s, request: %s",
               Q_FUNC_INFO,
               reply->errorString().toLatin1().data(),
               reply->request().url().toString().toLatin1().data());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
    {
        qWarning("Cannot parse retrieve auth token data: %s",
                 error.errorString().toLatin1().data());
        qDebug("Retrieve auth token data: %s", data.data());
        handleConnectionError();
        return;
    }

    m_authToken = jsonDocument.object().value("auth_token").toString();
    QJsonObject dataObject = jsonDocument.object().value("data").toObject();
    m_accountId = dataObject.value("account_id").toString();

    QString ownerId = dataObject.value("owner_id").toString();

    QNetworkRequest crossbarReq;
    crossbarReq.setRawHeader("X-Auth-Token", m_authToken.toLatin1());
    QString crossbarUrl(m_settings->value("crossbar_url", kCrossbarUrl).toString());
    crossbarUrl.append(QString(kCrossbarPath).arg(m_accountId, ownerId));
    crossbarReq.setUrl(QUrl(crossbarUrl));
    qDebug("Crossbar url: %s", crossbarUrl.toLatin1().data());
    QNetworkReply *crossbarReply = m_nam->get(crossbarReq);
    connect(crossbarReply, &QNetworkReply::finished,
            this, &WebSocketManager::retrieveDevicesFinished);
    connect(crossbarReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(handleConnectionError()));
}

void WebSocketManager::retrieveDevicesFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug("%s, NetworkReply error: %s, request: %s",
               Q_FUNC_INFO,
               reply->errorString().toLatin1().data(),
               reply->request().url().toString().toLatin1().data());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
    {
        qWarning("Cannot parse retrieve device data: %s",
                 error.errorString().toLatin1().data());
        qDebug("Retrieve device data: %s", data.data());
        handleConnectionError();
        return;
    }

    QJsonArray devices = jsonDocument.object().value("data").toArray();
    for (int i = 0; i < devices.count(); ++i)
        m_devices.append(devices.at(i).toObject().value("id").toString());

    QNetworkRequest req;

    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);

    req.setRawHeader("X-Auth-Token", m_authToken.toLatin1());
    QString url(m_settings->value("event_url", kEventUrl).toString());
    url.append(kRetrieveWsPath);
    req.setUrl(QUrl(url));
    qDebug("event url: %s", url.toLatin1().data());
    QNetworkReply *newReply = m_nam->get(req);

    connect(newReply, &QNetworkReply::finished,
            this, &WebSocketManager::retrieveWsAddressFinished);
    connect(newReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(handleConnectionError()));
}


void WebSocketManager::retrieveWsAddressFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug("%s, NetworkReply error: %s, request: %s",
               Q_FUNC_INFO,
               reply->errorString().toLatin1().data(),
               reply->request().url().toString().toLatin1().data());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QByteArray webSocketPath = data.mid(0, data.indexOf(":"));

    QUrl wsUrl(m_settings->value("event_url", kEventUrl).toUrl());
    wsUrl.setScheme(kWsScheme);
    QString path(kWsPath);
    path.append(webSocketPath);
    wsUrl.setPath(path);

    qDebug("websocket url: %s", wsUrl.toString().toLatin1().data());

    m_webSocket = new QWebSocket();

    /* Setup SSL */
    QSslConfiguration config = m_webSocket->sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    m_webSocket->setSslConfiguration(config);

    connect(m_webSocket, &QWebSocket::connected,
            this, &WebSocketManager::webSocketConnected);
    connect(m_webSocket, &QWebSocket::disconnected,
            this, &WebSocketManager::webSocketDisconnected);
    connect(m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleConnectionError()));

    m_webSocket->open(wsUrl);
}

void WebSocketManager::webSocketConnected()
{
    qDebug("WebSocket connected");
    connect(m_webSocket, &QWebSocket::textFrameReceived,
            this, &WebSocketManager::webSocketTextFrameReceived);

    QString subscribe("5:::{\"name\":\"subscribe\",\"args\":"
                      "[{\"account_id\":\"%1\",\"auth_token\""
                      ":\"%2\",\"binding\":\"call.CHANNEL_CREATE.*\"}]}");
    m_webSocket->sendTextMessage(subscribe.arg(m_accountId, m_authToken));

    subscribe = QString("5:::{\"name\":\"subscribe\",\"args\":"
                        "[{\"account_id\":\"%1\",\"auth_token\""
                        ":\"%2\",\"binding\":\"call.CHANNEL_ANSWER.*\"}]}");
    m_webSocket->sendTextMessage(subscribe.arg(m_accountId, m_authToken));

    subscribe = QString("5:::{\"name\":\"subscribe\",\"args\":"
                        "[{\"account_id\":\"%1\",\"auth_token\""
                        ":\"%2\",\"binding\":\"call.CHANNEL_DESTROY.*\"}]}");
    m_webSocket->sendTextMessage(subscribe.arg(m_accountId, m_authToken));

    emit connected();
}

void WebSocketManager::webSocketDisconnected()
{
    qDebug("WebSocket disconnected");
}

void WebSocketManager::webSocketTextFrameReceived(const QString &frame)
{
    if (frame.startsWith("2"))
    {
        m_lastPing = QDateTime::currentMSecsSinceEpoch();
        return;
    }

    if (!frame.startsWith("5"))
        return;

    QString data = frame.mid(4);

    processWsData(data);
}

void WebSocketManager::processWsData(const QString &data)
{
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data.toLatin1(), &error);

    if (error.error != QJsonParseError::NoError)
    {
        qWarning("Cannot parse data from websocket: %s",
                 error.errorString().toLatin1().data());
        qDebug("WebSocket data: %s", data.toLatin1().data());
        return;
    }

    QString eventName = jsonDocument.object().value("name").toString();
    QJsonObject args = jsonDocument.object().value("args").toObject();
    if (eventName == "CHANNEL_CREATE")
    {
        processChannelCreate(args);
    }
    else if (eventName == "CHANNEL_ANSWER")
    {
        processChannelAnswer(args);
    }
    else if (eventName == "CHANNEL_DESTROY")
    {
        processChannelDestroy(args);
    }
}

bool WebSocketManager::isSupportCallDirection(const QString &callDirection)
{
    int direction = m_settings->value("call_direction", kCallDirection).toInt();

    if (direction == kCallDirectionInbound && callDirection != "inbound")
        return false;

    if (direction == kCallDirectionOutbound && callDirection != "outbound")
        return false;

    return true;
}

void WebSocketManager::processChannelCreate(const QJsonObject &args)
{
    QJsonObject customChannelVars = args.value("Custom-Channel-Vars").toObject();
    QString authorizingId = customChannelVars.value("Authorizing-ID").toString();

    if (!m_devices.contains(authorizingId))
        return;

    QString callDirection = args.value("Call-Direction").toString();

    if (!isSupportCallDirection(callDirection))
        return;

    QString otherLegCallId = args.value("Other-Leg-Call-ID").toString();

    if (m_callersHash.contains(otherLegCallId))
        return;

    QString callerIdName = args.value("Caller-ID-Name").toString();
    QString callerIdNumber = args.value("Caller-ID-Number").toString();
    QString callerDialed = args.value("Request").toString();
    int separatorIndex = callerDialed.indexOf("@");
    callerDialed = callerDialed.mid(0, separatorIndex);

    QString url(m_settings->value("info_url", kInfoUrl).toString());
    QRegExp regExp("[^\\{]*\\{\\{([^\\}]*)\\}\\}");
    if (regExp.indexIn(url) != -1)
    {
        QString key = regExp.cap(1);
        QString value = args.value(key).toString();
        url.replace("{{" + key + "}}", value);
    }

    Caller caller(callerIdName, callerIdNumber, callerDialed, url);
    m_callersHash.insert(otherLegCallId, caller);

    qDebug("Channel created");
    emit channelCreated(otherLegCallId, caller);
}

void WebSocketManager::processChannelAnswer(const QJsonObject &args)
{
    QString callDirection = args.value("Call-Direction").toString();

    if (callDirection != "outbound")
        return;

    QJsonObject customChannelVars = args.value("Custom-Channel-Vars").toObject();
    QString authorizingId = customChannelVars.value("Authorizing-ID").toString();

    QString otherLegCallId = args.value("Other-Leg-Call-ID").toString();
    if (m_devices.contains(authorizingId))
    {
        qDebug("Channel answered");
        emit channelAnswered(otherLegCallId);
    }
    else
    {
        QString calleeNumber = args.value("Callee-ID-Number").toString();
        QString calleeName = args.value("Callee-ID-Name").toString();
        qDebug("Channel answered by another");
        emit channelAnsweredAnother(otherLegCallId, calleeNumber, calleeName);
    }
}

void WebSocketManager::processChannelDestroy(const QJsonObject &args)
{
    QJsonObject customChannelVars = args.value("Custom-Channel-Vars").toObject();
    QString authorizingId = customChannelVars.value("Authorizing-ID").toString();

    if (!m_devices.contains(authorizingId))
        return;

    QString otherLegCallId = args.value("Other-Leg-Call-ID").toString();
    if (!m_callersHash.contains(otherLegCallId))
        return;

    qDebug("Channel destroyed");
    emit channelDestroyed(otherLegCallId);
    m_callersHash.remove(otherLegCallId);
}

void WebSocketManager::handleConnectionError()
{
    qWarning("WebSocket connection error");
    if (m_webSocket != nullptr)
    {
        qDebug("WebSocket error: %s",
               socketErrorToString(m_webSocket->error()).toLatin1().data());
    }
    stop();
    emit connectionError();
}

void WebSocketManager::checkPingTimeout()
{
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
    if (currentTimestamp - m_lastPing > kPermittedPingTimeout)
        handleConnectionError();
}

QString socketErrorToString(QAbstractSocket::SocketError error)
{
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        return "The connection was refused by the peer (or timed out)";
    case QAbstractSocket::RemoteHostClosedError:
        return "The remote host closed the connection";
    case QAbstractSocket::HostNotFoundError:
        return "The host address was not found";
    case QAbstractSocket::SocketAccessError:
        return "The socket operation failed because the application lacked the required privileges";
    case QAbstractSocket::SocketResourceError:
        return "The local system ran out of resources (e.g., too many sockets)";
    case QAbstractSocket::SocketTimeoutError:
        return "The socket operation timed out";
    case QAbstractSocket::DatagramTooLargeError:
        return "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)";
    case QAbstractSocket::NetworkError:
        return "An error occurred with the network (e.g., the network cable was accidentally plugged out)";
    case QAbstractSocket::AddressInUseError:
        return "The specified address is already in use and was set to be exclusive";
    case QAbstractSocket::SocketAddressNotAvailableError:
        return "The specified address does not belong to the host";
    case QAbstractSocket::UnsupportedSocketOperationError:
        return "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)";
    case QAbstractSocket::ProxyAuthenticationRequiredError:
    return "The socket is using a proxy, and the proxy requires authentication";
    case QAbstractSocket::SslHandshakeFailedError:
        return "The SSL/TLS handshake failed, so the connection was closed";
    case QAbstractSocket::UnfinishedSocketOperationError:
        return "The last operation attempted has not finished yet (still in progress in the background)";
    case QAbstractSocket::ProxyConnectionRefusedError:
        return "Could not contact the proxy server because the connection to that server was denied";
    case QAbstractSocket::ProxyConnectionClosedError:
        return "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";
    case QAbstractSocket::ProxyConnectionTimeoutError:
        return "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase";
    case QAbstractSocket::ProxyNotFoundError:
        return "The proxy address was not found";
    case QAbstractSocket::ProxyProtocolError:
        return "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood";
    case QAbstractSocket::OperationError:
        return "An operation was attempted while the socket was in a state that did not permit it";
    case QAbstractSocket::SslInternalError:
        return "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library";
    case QAbstractSocket::SslInvalidUserDataError:
        return "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library";
    case QAbstractSocket::TemporaryError:
        return "A temporary error occurred (e.g., operation would block and socket is non-blocking)";
    case QAbstractSocket::UnknownSocketError:
    default:
        return "An unidentified websocket error occurred";
    }
}
