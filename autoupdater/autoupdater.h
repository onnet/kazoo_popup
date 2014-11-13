#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QObject>
#include <QStringList>

class QProgressDialog;

class AutoUpdater : public QObject
{
    Q_OBJECT

public:
    explicit AutoUpdater(QObject *parent = 0);
    ~AutoUpdater();

    void downloadUpdates(const QString &url);
    void installNewVersion();
    void clean();

private slots:
    void processDownloadFinished(const QString &archiveFilePath);
    void processDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void processDownloadError(const QString &error);

    void processExtractFinished();

private:
    void createProgressDialog();

    static void runUpdateAction(const QString &updateAction, const QString &updaterDirPath);
    static void runApp(const QString &app, const QStringList &args = QStringList());
    static void runPreUpdateScript(const QString &updatesDirPath);
    static void runPostUpdateScript(const QString &updatesDirPath);

private:
    QProgressDialog *m_progressDialog = nullptr;
    QString m_url;
};

#endif // AUTOUPDATER_H
