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

#include "websocketmanager.h"

#include "defaults.h"
#include "contactinfo.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonObject>

#include <QApplication>
#include <QSettings>

static const char * const kRetrieveWsPath = "/socket.io/1/";
static const char * const kWsPath = "/socket.io/1/websocket/";
static const char * const kCallerInfoQuery = "?calling_number=%1&md5=%2";

WebSocketManager::WebSocketManager(QObject *parent) :
    QObject(parent)
{
    m_nam = new QNetworkAccessManager(this); 

    m_settings = new QSettings(qApp->applicationDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);
}

WebSocketManager::~WebSocketManager()
{
    if (m_webSocket)
        m_webSocket->deleteLater();
}

void WebSocketManager::start()
{
    retrieveAuthToken();
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

    if (m_webSocket)
        m_webSocket->deleteLater();

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
    if (!message.startsWith("5")) return;

    QString data = message.mid(4);

    processWsData(data);
}

void WebSocketManager::processWsData(const QString &data)
{
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data.toLatin1(), &error);

    if (error.error != QJsonParseError::NoError)
        return;

    QString eventName = jsonDocument.object().value("name").toString();
    if (eventName == "CHANNEL_CREATE")
    {
        QJsonObject args = jsonDocument.object().value("args").toObject();
        QString callDirection = args.value("Call-Direction").toString();

        if (callDirection != "inbound") return; 

        QString callerNumber = args.value("Caller-ID-Number").toString();
        QString callId = args.value("Call-ID").toString();

        m_contactCallIdHash.insert(callerNumber, callId);

        m_createChannelHash.insert(callId, callerNumber);
        //QString calleeNumber = args.value("Callee-ID-Number").toString();

        if (m_contactsHash.contains(callerNumber))
        {
            ContactInfo *contactInfo = m_contactsHash.value(callerNumber);
            if (!contactInfo->isEmpty())
                emit channelCreated(contactInfo);
            return;
        }

        QByteArray hashTemplate(callerNumber.toLatin1());
        hashTemplate.append(":");
        hashTemplate.append(m_settings->value("md5_hash", kMd5Hash).toString());

        QByteArray hash = QCryptographicHash::hash(hashTemplate, QCryptographicHash::Md5).toHex();

        QNetworkRequest req;
        QString url(m_settings->value("info_url", kInfoUrl).toString());
        url.append(kCallerInfoQuery);
        req.setUrl(QUrl(url.arg(callerNumber).arg(hash.data())));
        QNetworkReply *reply = m_nam->get(req);
        reply->setProperty("caller_number", callerNumber);

        m_contactsHash.insert(callerNumber, new ContactInfo());

        connect(reply, &QNetworkReply::finished,
                this, &WebSocketManager::retrieveCallerInfoFinished);
    }
    else if (eventName == "CHANNEL_ANSWER")
    {
        QJsonObject args = jsonDocument.object().value("args").toObject();
        QString callDirection = args.value("Call-Direction").toString();

        if (callDirection != "inbound") return;

        QString callId = args.value("Call-ID").toString();
        QString calleeNumber = args.value("Callee-ID-Number").toString();
        if (!calleeNumber.isEmpty())
        {
            bool ok;
            qlonglong number = calleeNumber.toLongLong(&ok);
            Q_UNUSED(number);
            if (!ok)
                return;
        }

        QString callerNumber = m_contactCallIdHash.key(callId);

        if (m_contactsHash.contains(callerNumber))
        {
            ContactInfo *contactInfo = m_contactsHash.value(callerNumber);
            if (!contactInfo->isEmpty())
            {
                emit channelAnswered(contactInfo);
                m_contactCallIdHash.remove(callerNumber);
            }
        }
    }
    else if (eventName == "CHANNEL_DESTROY")
    {
        QJsonObject args = jsonDocument.object().value("args").toObject();
        //QString callerNumber = args.value("Caller-ID-Number").toString();
        QString callId = args.value("Call-ID").toString();

        if (m_createChannelHash.contains(callId))
        {
            QString number = m_createChannelHash.value(callId);
            m_createChannelHash.remove(callId);

            if (!m_createChannelHash.values().contains(number))
            {
                ContactInfo *contactInfo = m_contactsHash.value(number);
                m_contactsHash.remove(number);
                emit channelDestroyed(contactInfo);
            }
        }
    }
}

void WebSocketManager::retrieveCallerInfoFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    QString callerNumber = reply->property("caller_number").toString();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError)
        return;

    QString contactPerson = document.object().value("contact_person").toString();
    QString loginName = document.object().value("login_name").toString();
    QString callingNumber = document.object().value("calling_number").toString();
    QString companyName = document.object().value("company_name").toString();
    double balance = document.object().value("cur_balance").toDouble();

    ContactInfo *ci = new ContactInfo();
    ci->setContactPerson(contactPerson);
    ci->setLogin(loginName);
    ci->setCallingNumber(callingNumber);
    ci->setCompanyName(companyName);
    ci->setBalance(balance);

    m_contactsHash.insert(callerNumber, ci);

    emit channelCreated(ci);
}
