#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QTime>
#include <QFile>

class QNetworkAccessManager;
class QNetworkReply;

class DownloadManager: public QObject
{
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = 0);
    ~DownloadManager();

    void setTargetDirPath(const QString &targetDirPath);
    void download(const QString &url);

public slots:
    void abort();

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void finished(const QString &filePath);
    void error(const QString &error);

private:
    QString buildFilePath(const QString &fileName);
    bool saveToDisk(QNetworkReply *reply);

    QNetworkAccessManager *m_nam = nullptr;
    QString m_targetDirPath;
    QList<QNetworkReply*> m_replies;

private slots:
    void processReadyRead();
    void processDownloadFinished();
    void processDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // DOWNLOADMANAGER_H
