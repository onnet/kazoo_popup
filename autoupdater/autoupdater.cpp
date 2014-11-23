#include "autoupdater.h"

#include "defaults.h"
#include "downloadmanager.h"
#include "updatemanager.h"
#include "zipextractor.h"

#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>
#include <QFileInfo>
#include <QIcon>
#include <QThread>

#include <QDebug>

#ifdef Q_OS_WIN
static int kProgressDialogWidth = 280;
static int kProgressDialogHeight = 100;
#elif defined Q_OS_MAC
static int kProgressDialogWidth = 400;
static int kProgressDialogHeight = 100;
#endif

AutoUpdater::AutoUpdater(QObject *parent) :
    QObject(parent)
{
}

AutoUpdater::~AutoUpdater()
{
    if (m_progressDialog != nullptr)
        delete m_progressDialog;
}

void AutoUpdater::downloadUpdates(const QString &url)
{
    m_url = url;
    qDebug() << "Download updates url: " << url;
    DownloadManager *downloadManager = new DownloadManager(this);
    downloadManager->setTargetDirPath(qApp->applicationDirPath());

    if (m_progressDialog == nullptr)
        createProgressDialog();

    connect(downloadManager, &DownloadManager::finished,
            this, &AutoUpdater::processDownloadFinished);
    connect(downloadManager, &DownloadManager::error,
            this, &AutoUpdater::processDownloadError);
    connect(downloadManager, &DownloadManager::downloadProgress,
            this, &AutoUpdater::processDownloadProgress);

    downloadManager->download(url);
    m_progressDialog->show();
}

void AutoUpdater::installNewVersion()
{
    qDebug() << "Install new version";

    QString updatesDirPath(qApp->applicationDirPath());
    QDir appDir(updatesDirPath);
    appDir.cdUp();
    QString mainAppDirPath(appDir.absolutePath());

    if (appDir.exists(kBackupDirName))
    {
        appDir.cd(kBackupDirName);
        if (!appDir.removeRecursively())
        {
            qWarning() << "Cannot remove backup folder";
            QString message = tr("Updating was failed: cannot make updates. "
                                 "Please contact us by email: ");
            QMessageBox::warning(0,
                                 qApp->applicationName(),
                                 message.append(kContactEmail));

            runUpdateAction("clean", mainAppDirPath);
            exit(0);
        }
    }

    QStringList ignoreFiles(kUpdatesDirName);
    ignoreFiles.append(kBackupDirName);
    ignoreFiles.append(kIgnoreListFileName);

    QString ignoreListPath = updatesDirPath + "/" + kIgnoreListFileName;
    QFile ignoreList(ignoreListPath);
    if (ignoreList.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!ignoreList.atEnd())
            ignoreFiles << ignoreList.readLine().simplified();
        ignoreList.close();
    }

    if (UpdateManager::makeBackup(mainAppDirPath, kBackupDirName, ignoreFiles))
    {
        runUpdateScript(updatesDirPath, kPreUpdateFileName);

        if (UpdateManager::installUpdates(updatesDirPath, mainAppDirPath, ignoreFiles))
        {
            runUpdateScript(updatesDirPath, kPostUpdateFileName);

            runUpdateAction("clean", mainAppDirPath, true);
        }
        else
        {
            qDebug() << "Cannot install updates, restore backup";
            if (!UpdateManager::restoreFromBackup(mainAppDirPath, kBackupDirName))
            {
                QString message = tr("Updating was failed: cannot make updates and rollback. "
                                     "Please contact us by email: ");
                QMessageBox::warning(0,
                                     qApp->applicationName(),
                                     message.append(kContactEmail));
            }
            else
            {
                QString message = tr("Updating was failed: cannot make updates. "
                                     "Please contact us by email: ");
                QMessageBox::warning(0,
                                     qApp->applicationName(),
                                     message.append(kContactEmail));
            }

            runUpdateAction("clean", mainAppDirPath);
        }
    }
    else
    {
        qDebug() << "Cannot make backup, restore backup";
        QString message = tr("Updating was failed: cannot make backup. "
                             "Please contact us by email: ");
        QMessageBox::warning(0,
                             qApp->applicationName(),
                             message.append(kContactEmail));
        runUpdateAction("clean", mainAppDirPath);
    }

    exit(0);
}

void AutoUpdater::runUpdateScript(const QString &updatesDirPath, const QString &scriptName)
{
    QDir updatesDir(updatesDirPath);
    if (!updatesDir.exists(scriptName))
        return;

    QProcess p;
    QString path(updatesDir.absoluteFilePath(scriptName));
#ifdef Q_OS_WIN
    p.start("cmd.exe", QStringList() << "/c" << QDir::toNativeSeparators(path));
#elif defined Q_OS_MAC
    p.start("sh", QStringList() << path);
#endif
    p.waitForFinished();
}

void AutoUpdater::clean()
{
    QDir appDir(qApp->applicationDirPath());

    if (appDir.cd(kUpdatesDirName))
    {
        appDir.removeRecursively();
        appDir.cdUp();
    }

    if (appDir.cd(kBackupDirName))
    {
        appDir.removeRecursively();
        appDir.cdUp();
    }

    runApp(appDir.absoluteFilePath(kUpdatedAppFileName));
    exit(0);
}

void AutoUpdater::processDownloadFinished(const QString &archiveFilePath)
{
    qDebug() << "Download finished: " << archiveFilePath;

    m_progressDialog->setMinimum(0);
    m_progressDialog->setMaximum(0);
    m_progressDialog->setWindowTitle(tr("Extracting updates"));
    m_progressDialog->setLabelText(tr("Extracting KazooPopup updates, please wait..."));
    m_progressDialog->show();

    QThread *thread = new QThread();
    ZipExtractor *zipExtractor = new ZipExtractor();
    zipExtractor->setArchiveFilePath(archiveFilePath);
    zipExtractor->moveToThread(thread);

    connect(thread, &QThread::started,
            zipExtractor, &ZipExtractor::start);
    connect(zipExtractor, &ZipExtractor::finished,
            thread, &QThread::quit);
    connect(zipExtractor, &ZipExtractor::finished,
            this, &AutoUpdater::processExtractFinished);
    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);

    thread->start();
}

void AutoUpdater::processDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qint64 value = bytesReceived * 100 / bytesTotal;

    m_progressDialog->setValue(value);
}

void AutoUpdater::processDownloadError(const QString &error)
{
    qWarning() << "download error: " << error;

    m_progressDialog->close();

    int result = QMessageBox::question(0,
                                       qApp->applicationName(),
                                       tr("Download updates failed. "
                                          "Do you want to retry download updates?"));
    if (result == QMessageBox::Yes)
    {
        qDebug() << "user wants to retry download updates";
        DownloadManager *dm = qobject_cast<DownloadManager*>(sender());
        dm->deleteLater();

        downloadUpdates(m_url);
    }
    else
    {
        qDebug() << "quit without retry";
        // quit and run updated application
        runUpdateAction("clean", qApp->applicationDirPath());
        qApp->quit();
    }
}

void AutoUpdater::runUpdateAction(const QString &updateAction, const QString &updaterDirPath, bool ok)
{
    if (ok)
    {
        QMessageBox::information(0,
                                 qApp->applicationName(),
                                 tr("Kazoo Popup application is successfully updated"));
    }
    QStringList arguments(updateAction);
    runApp(updaterDirPath + "/" + kAutoUpdaterFileName, arguments);
}

void AutoUpdater::runApp(const QString &app, const QStringList &args)
{
    if (QProcess::startDetached(app, args))
        return;

    qDebug() << "Cannot start process: " << app << " with arguments: " << args;

    QString message = tr("Updating was failed: cannot make updates. "
                         "Please contact us by email: ");
    QMessageBox::warning(0,
                         qApp->applicationName(),
                         message.append(kContactEmail));
}

void AutoUpdater::createProgressDialog()
{
    m_progressDialog = new QProgressDialog();
    m_progressDialog->setMaximum(100);
    m_progressDialog->setWindowTitle(tr("Downloading updates"));
    m_progressDialog->setWindowIcon(QIcon(":/res/kazoo_32.png"));
    m_progressDialog->setLabelText(tr("Downloading KazooPopup updates, please wait..."));
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_progressDialog->setFixedSize(kProgressDialogWidth, kProgressDialogHeight);

    connect(m_progressDialog, &QProgressDialog::canceled,
            qApp, &QApplication::quit);
}

void AutoUpdater::processExtractFinished()
{
    ZipExtractor *zipExtractor = qobject_cast<ZipExtractor *>(sender());

    m_progressDialog->close();

    if (zipExtractor != nullptr && zipExtractor->error().isEmpty())
    {
        QStringList arguments("install");
        QFileInfo fileInfo(zipExtractor->archiveFilePath());
        QDir archiveDir(fileInfo.absoluteDir());
        archiveDir.cd(kUpdatesDirName);
        QString path = archiveDir.absoluteFilePath(kAutoUpdaterFileName);
        runApp(path, arguments);
    }
    else
    {
        QString message = tr("Updating was failed: cannot extract updates. "
                             "Please contact us by email: ");
        QMessageBox::warning(0,
                             qApp->applicationName(),
                             message.append(kContactEmail));
    }

    delete zipExtractor;

    qApp->quit();
}
