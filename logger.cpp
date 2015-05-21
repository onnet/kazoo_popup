#include "logger.h"

#include "defaults.h"

#include <QMutexLocker>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QDate>

#include <iostream>

static const char * const kLogFileNameTemplate = "'%1_'yyMMdd'.log'";

Logger *Logger::m_instance = nullptr;

void msgHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    Q_UNUSED(ctx);
    Logger::instance()->handleMessage(type, msg);
}

Logger::Logger(QObject *parent) :
    QObject(parent)
{
}

Logger::~Logger()
{

}

Logger *Logger::instance()
{
    if (m_instance == nullptr)
    {
        static QMutex mutex;
        mutex.lock();
        if (m_instance == nullptr)
            m_instance = new Logger;
        mutex.unlock();
    }
    return m_instance;
}

void Logger::start()
{
    qInstallMessageHandler(msgHandler);
}

void Logger::stop()
{
    qInstallMessageHandler(0);
}

void Logger::handleMessage(QtMsgType type, const QString &msg)
{
    QMutexLocker locker(&m_mutex);

    //Create file with date addition
    QString fileName = logsDirPath() + QDate::currentDate().toString(logFileNameTemplate());
    QFile file(fileName);
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream stream(&file);
    QString datetime = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString log;
    switch (type)
    {
    case QtDebugMsg:
        log = QString("[DEBUG] %1: %2\n").arg(datetime).arg(msg);
        break;
    case QtWarningMsg:
        log = QString("[WARNING] %1: %2\n").arg(datetime).arg(msg);
        break;
    case QtCriticalMsg:
        log = QString("[CRITICAL] %1: %2\n").arg(datetime).arg(msg);
        break;
    case QtFatalMsg:
        log = QString("[FATAL] %1: %2\n").arg(datetime).arg(msg);
        abort();
    }

    std::cout << msg.toLatin1().data() << std::endl;
    stream << log;
    emit newLog(log);

    file.close();
}

QString Logger::logFileNameTemplate()
{
    return QString(kLogFileNameTemplate).arg(qApp->applicationName());
}

QString Logger::logs() const
{
    QString fileName = logsDirPath() + QDate::currentDate().toString(logFileNameTemplate());
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return QString();

    QTextStream stream(&file);
    QString data = stream.readAll();
    data = data.trimmed();
    file.close();

    return data;
}
