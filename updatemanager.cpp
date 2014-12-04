#include "updatemanager.h"

#include "defaults.h"

#include <QTimer>
#include <QMutex>
#include <QApplication>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#elif defined Q_OS_MAC
#   include <QProcess>
#   include <QDir>
#endif

UpdateManager *UpdateManager::m_instance = nullptr;

static int kUpdateMinInterval = 1; // 1 hour min interval
static int kUpdateMaxInterval = 5; // 5 hours max interval
static const char * const kCheckUpdateUrl = "http://updates.2600hz.com/checkupdate.php";

#ifdef Q_OS_WIN
static QString kUpdaterFileName = "autoupdater.exe";
static int kSuccessShellExecute = 32; // http://msdn.microsoft.com/en-us/library/windows/desktop/bb762153(v=vs.85).aspx
#elif defined Q_OS_MAC
static QString kUpdaterFileName = "autoupdater";
#endif


UpdateManager::UpdateManager(QObject *parent) :
    QObject(parent)
{
    m_timer = new QTimer();
    qsrand(QDateTime::currentMSecsSinceEpoch());
    connect(m_timer, &QTimer::timeout,
            this, &UpdateManager::processTimeout);
}

UpdateManager *UpdateManager::instance()
{
    if (m_instance == nullptr)
    {
        static QMutex mutex;
        mutex.lock();
        if (m_instance == nullptr)
            m_instance = new UpdateManager;
        mutex.unlock();
    }
    return m_instance;
}

void UpdateManager::start()
{
    processTimeout();
}

void UpdateManager::stop()
{
    m_timer->stop();
}

void UpdateManager::processTimeout()
{
    checkUpdate();

    int randValue = (qrand() % (kUpdateMaxInterval - kUpdateMinInterval + 1)) + kUpdateMinInterval;
    m_timer->start(randValue * 60 * 60 * 1000);
}

void UpdateManager::checkUpdate(bool quiet)
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

    connect(nam, &QNetworkAccessManager::finished,
            this, &UpdateManager::onReplyFinished);

    QUrl url(kCheckUpdateUrl);
    QString query("app=kazoo&platform=%1&ver=%2");
    url.setQuery(query.arg(detectPlatform()).arg(APP_VERSION));

    QNetworkReply *reply = nam->get(QNetworkRequest(url));
    reply->setProperty("quiet", quiet);
}

void UpdateManager::onReplyFinished(QNetworkReply *reply)
{
    QString data = reply->readAll();
    bool quiet = reply->property("quiet").toBool();
    if (!data.isEmpty() && data.startsWith("http"))
    {
        qDebug("New version is available");
        m_updateUrl = data;
        stop();

        if (quiet)
            doUpdate();
        else
            emit updateAvailable();
    }
    else
    {
        if (quiet)
            emit noUpdate();
    }

    reply->manager()->deleteLater();
    reply->deleteLater();
}

bool UpdateManager::doUpdate() const
{
    QString updaterPath("%1/%2");
    updaterPath = updaterPath.arg(qApp->applicationDirPath(), kUpdaterFileName);

    bool ok;
#ifdef Q_OS_WIN
    QString parameters("\"update\" \"%1\"");
    parameters = parameters.arg(m_updateUrl);
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
    QStringList arguments("update");
    arguments.append(m_updateUrl);
    ok = QProcess::startDetached(updaterPath, arguments);
#endif
    if (!ok)
        qWarning() << "Unable to start updater from path: " << updaterPath;
    else
        QTimer::singleShot(0, qApp, SLOT(quit()));

    return ok;
}

void UpdateManager::quietUpdate()
{
    if (m_updateUrl.isEmpty())
        checkUpdate(true);
    else
        doUpdate();
}

