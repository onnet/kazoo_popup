#include "websocketmanager.h"

#include "defaults.h"
#include "caller.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonObject>

#include <QSettings>

static const char * const kRetrieveWsPath = "/socket.io/1/";
static const char * const kWsPath = "/socket.io/1/websocket/";

WebSocketManager::WebSocketManager(QObject *parent) :
    QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
}

WebSocketManager::~WebSocketManager()
{
    if (m_webSocket)
    {
        m_webSocket->close();
        m_webSocket->deleteLater();
    }
}

void WebSocketManager::start()
{
    m_callersHash.clear();

    if (m_settings)
        m_settings->deleteLater();

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);

    if (m_webSocket)
    {
        m_webSocket->close();
        m_webSocket->deleteLater();
    }

    retrieveAuthToken();
}

void WebSocketManager::stop()
{
    m_callersHash.clear();

    if (m_settings)
        m_settings->deleteLater();

    if (m_webSocket)
    {
        m_webSocket->close();
        m_webSocket->deleteLater();
    }
}

void WebSocketManager::retrieveAuthToken()
{
    QByteArray hashTemplate(m_settings->value("login", kLogin).toByteArray());
    hashTemplate.append(":");
    hashTemplate.append(m_settings->value("password", kPassword).toString());

    QByteArray hash = QCryptographicHash::hash(hashTemplate, QCryptographicHash::Md5).toHex();
    QJsonObject jsonObject;
    QJsonObject jsonData;
    jsonData["credentials"] = hash.data();
    jsonData["realm"] = m_settings->value("realm", kRealm).toString();
    jsonObject["data"] = jsonData;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray json = jsonDocument.toJson();

    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setUrl(QUrl(m_settings->value("auth_url", kAuthUrl).toString()));
    QNetworkReply *reply = m_nam->put(req, json);
    connect(reply, &QNetworkReply::finished,
            this, &WebSocketManager::retrieveAuthTokenFinished);
}

void WebSocketManager::retrieveAuthTokenFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
        return;

    m_authToken = jsonDocument.object().value("auth_token").toString();
    QJsonObject dataObject = jsonDocument.object().value("data").toObject();
    m_accountId = dataObject.value("account_id").toString();
    QNetworkRequest req;
    req.setRawHeader("X-Auth-Token", m_authToken.toLatin1());
    QString url(m_settings->value("event_url", kEventUrl).toString());
    url.append(kRetrieveWsPath);
    req.setUrl(QUrl(url));
    QNetworkReply *newReply = m_nam->get(req);
    connect(newReply, &QNetworkReply::finished,
            this, &WebSocketManager::retrieveWsAddressFinished);
}

void WebSocketManager::retrieveWsAddressFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QByteArray webSocketPath = data.mid(0, data.indexOf(":"));

    QUrl wsUrl(m_settings->value("event_url", kEventUrl).toUrl());
    wsUrl.setScheme("ws");
    QString path(kWsPath);
    path.append(webSocketPath);
    wsUrl.setPath(path);

    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected,
            this, &WebSocketManager::webSocketConnected);
    connect(m_webSocket, &QWebSocket::disconnected,
            this, &WebSocketManager::webSocketDisconnected);

    m_webSocket->open(wsUrl);
}

void WebSocketManager::webSocketConnected()
{
    qDebug("WebSocket connected");
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketManager::webSocketTextMessageReceived);

    QString subscribe("5:::{\"name\":\"subscribe\",\"args\":[{\"account_id\":\"%1\",\"auth_token\":\"%2\",\"binding\":\"call.CHANNEL_CREATE.*\"}]}");
    m_webSocket->sendTextMessage(subscribe.arg(m_accountId, m_authToken));

    subscribe = QString("5:::{\"name\":\"subscribe\",\"args\":[{\"account_id\":\"%1\",\"auth_token\":\"%2\",\"binding\":\"call.CHANNEL_ANSWER.*\"}]}");
    m_webSocket->sendTextMessage(subscribe.arg(m_accountId, m_authToken));

    subscribe = QString("5:::{\"name\":\"subscribe\",\"args\":[{\"account_id\":\"%1\",\"auth_token\":\"%2\",\"binding\":\"call.CHANNEL_DESTROY.*\"}]}");
    m_webSocket->sendTextMessage(subscribe.arg(m_accountId, m_authToken));
}

void WebSocketManager::webSocketDisconnected()
{
    qDebug("WebSocket disconnected");
}

void WebSocketManager::webSocketTextMessageReceived(const QString &message)
{
    if (!message.startsWith("5"))
        return;

    QString data = message.mid(4);

    processWsData(data);
}

void WebSocketManager::processWsData(const QString &data)
{
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data.toLatin1(), &error);

    if (error.error != QJsonParseError::NoError)
        return;

    qDebug() << "\n" << data << "\n";

    QString eventName = jsonDocument.object().value("name").toString();
    if (eventName == "CHANNEL_CREATE")
    {
        QJsonObject args = jsonDocument.object().value("args").toObject();
        QString callDirection = args.value("Call-Direction").toString();

        QString callId = args.value("Call-ID").toString();
        if (callDirection == "outbound")
        {
            QString otherLegCallId = args.value("Other-Leg-Call-ID").toString();
            m_callIdAndOtherLegHash.insert(callId, otherLegCallId);
            return;
        }

        if (m_callersHash.contains(callId))
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
        m_callersHash.insert(callId, caller);

        emit channelCreated(callId, caller);
    }
    else if (eventName == "CHANNEL_ANSWER")
    {
        QJsonObject args = jsonDocument.object().value("args").toObject();
        QString callDirection = args.value("Call-Direction").toString();

        if (callDirection != "outbound")
            return;

        QString callId = args.value("Call-ID").toString();
        QString otherLegCallId = args.value("Other-Leg-Call-ID").toString();
        if (!m_callIdAndOtherLegHash.contains(callId))
        {
            QString calleeNumber = args.value("Callee-ID-Number").toString();
            QString calleeName = args.value("Callee-ID-Name").toString();
            emit channelAnsweredAnother(otherLegCallId, calleeNumber, calleeName);
            return;
        }

        QString otherLegCallIdFromCreate = m_callIdAndOtherLegHash.value(callId);
        m_callIdAndOtherLegHash.remove(callId);
        if (otherLegCallIdFromCreate != otherLegCallId)
        {
            QString calleeNumber = args.value("Callee-ID-Number").toString();
            QString calleeName = args.value("Callee-ID-Name").toString();
            emit channelAnsweredAnother(otherLegCallId, calleeNumber, calleeName);
        }
        else
        {
            emit channelAnswered(otherLegCallId);
        }
    }
    else if (eventName == "CHANNEL_DESTROY")
    {
        QJsonObject args = jsonDocument.object().value("args").toObject();
        QString callId = args.value("Call-ID").toString();
        if (!m_callersHash.contains(callId))
            return;

        emit channelDestroyed(callId);
        m_callersHash.remove(callId);
    }
}
