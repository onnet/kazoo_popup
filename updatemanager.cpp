#include "updatemanager.h"

#include "defaults.h"

#include <QTimer>
#include <QMutex>
#include <QApplication>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QMessageBox>
#include <QIcon>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#endif

UpdateManager *UpdateManager::m_instance = nullptr;

static int kUpdateTimerInterval = 3600; // timer interval in seconds
static const char * const kCheckUpdateUrl = "http://localhost/checkupdate.php";

#ifdef Q_OS_WIN
static QString kUpdaterFileName = "autoupdater.exe";
static int kSuccessShellExecute = 32; // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762153(v=vs.85).aspx
#elif defined Q_OS_MAC
static QString kUpdaterFileName = "autoupdater.app";
#endif


UpdateManager::UpdateManager(QObject *parent) :
    QObject(parent)
{
    m_timer = new QTimer();
    m_timer->setInterval(kUpdateTimerInterval * 1000);
    connect(m_timer, &QTimer::timeout,
            this, &UpdateManager::checkUpdate);
}

UpdateManager *UpdateManager::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        mutex.lock();
        if (!m_instance)
            m_instance = new UpdateManager;
        mutex.unlock();
    }
    return m_instance;
}

void UpdateManager::start()
{
    checkUpdate();
    m_timer->start();
}

void UpdateManager::checkUpdate()
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

    connect(nam, &QNetworkAccessManager::finished,
            this, &UpdateManager::onReplyFinished);

    QUrl url(kCheckUpdateUrl);
    QString query("app=kazoo&platform=%1&ver=%2");
    url.setQuery(query.arg(detectPlatform()).arg(APP_VERSION));

    nam->get(QNetworkRequest(url));
}

void UpdateManager::onReplyFinished(QNetworkReply *reply)
{
    QString data = reply->readAll();
    if (data.isEmpty() || !data.startsWith("http"))
    {
        reply->manager()->deleteLater();
        reply->deleteLater();
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(qApp->applicationName());
    msgBox.setText(tr("New version is available. "
                      "Would you like to update?"));
    msgBox.setWindowIcon(QIcon(":/res/kazoo_32.png"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int answer = msgBox.exec();
    if (answer != QMessageBox::Yes)
    {
        reply->manager()->deleteLater();
        reply->deleteLater();
        return;
    }

    QString updaterPath("%1/%2");
    updaterPath = updaterPath.arg(qApp->applicationDirPath(), kUpdaterFileName);
    QStringList arguments("update");
    arguments.append(data);

    bool ok;
#ifdef Q_OS_WIN
    QString parameters("\"update\" \"");
    parameters.append(data);
    parameters.append("\"");
    int result = (int)ShellExecute(0,
                                   L"open",
                                   (const WCHAR*)updaterPath.utf16(),
                                   (const WCHAR*)parameters.utf16(),
                                   0,
                                   SW_SHOWNORMAL);
    if (result == SE_ERR_ACCESSDENIED)
    {
        // Requesting elevation
        result = (int)ShellExecute(0,
                                   L"runas",
                                   (const WCHAR*)updaterPath.utf16(),
                                   (const WCHAR*)parameters.utf16(),
                                   0,
                                   SW_SHOWNORMAL);
    }
    ok = (result > kSuccessShellExecute);
#elif defined Q_OS_MAC
    ok = QProcess::startDetached(updaterPath, arguments);
#endif
    if (!ok)
        qWarning() << "Unable to start updater from path: " << updaterPath;
    else
        QTimer::singleShot(0, qApp, SLOT(quit()));

    reply->manager()->deleteLater();
    reply->deleteLater();
}

