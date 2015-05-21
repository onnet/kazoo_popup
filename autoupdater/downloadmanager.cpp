#include "downloadmanager.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QStandardPaths>

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
    m_targetDirPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
}

DownloadManager::~DownloadManager()
{
    abort();
}

void DownloadManager::setTargetDirPath(const QString &targetDirPath)
{
    m_targetDirPath = targetDirPath;
}

void DownloadManager::download(const QString &url)
{
    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    QNetworkReply *reply = m_nam->get(request);
    reply->setProperty("file_path", buildFilePath(requestUrl.fileName()));

    connect(reply, &QNetworkReply::downloadProgress,
            this, &DownloadManager::processDownloadProgress);
    connect(reply, &QNetworkReply::finished,
            this, &DownloadManager::processDownloadFinished);
    connect(reply, &QNetworkReply::readyRead,
            this, &DownloadManager::processReadyRead);

    m_replies.append(reply);
}

void DownloadManager::abort()
{
    while (!m_replies.isEmpty())
    {
        QNetworkReply *reply = m_replies.takeFirst();
        reply->abort();
        QFile::remove(reply->property("file_path").toString());
        reply->deleteLater();
    }
}

QString DownloadManager::buildFilePath(const QString &fileName)
{
    QString basePath;
    if (fileName.isEmpty())
        basePath = m_targetDirPath + "/autoupdates";
    else
        basePath = m_targetDirPath + "/" + fileName;

    if (!QFile::exists(basePath))
        return basePath;

    // already exists, don't overwrite
    int i = 1;
    while (QFile::exists(basePath + "_" + QString::number(i)))
        ++i;

    return basePath + "_" + QString::number(i);
}

bool DownloadManager::saveToDisk(QNetworkReply *reply)
{
    QFile file(reply->property("file_path").toString());
    if (!file.open(QIODevice::Append))
    {
        qWarning() << "Could not open file for writing: " << file.errorString();
        return false;
    }

    file.write(reply->readAll());
    file.close();

    return true;
}

void DownloadManager::processDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply == nullptr)
        return;

    if (reply->error() == QNetworkReply::NoError)
    {
        emit finished(reply->property("file_path").toString());
    }
    else
    {
        qDebug() << "download error: " << reply->errorString();
        emit error(reply->errorString());
        QFile::remove(reply->property("file_path").toString());
    }

    m_replies.removeOne(reply);
    reply->deleteLater();
}

void DownloadManager::processReadyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply == nullptr)
        return;

    if (!saveToDisk(reply))
    {
        emit error("Could not open file for writing");
        reply->abort();
        QFile::remove(reply->property("file_path").toString());
        m_replies.removeOne(reply);
        reply->deleteLater();
    }
}

void DownloadManager::processDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}
