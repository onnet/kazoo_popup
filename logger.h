#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMutex>

class Logger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Logger)
public:
    static Logger* instance();

    ~Logger();

    void start();
    void stop();

    QString logs() const;

    void handleMessage(QtMsgType type, const QString &msg);

signals:
    void newLog(const QString &log);

private:
    explicit Logger(QObject *parent = 0);

    static QString logFileNameTemplate();

    static Logger *m_instance;

    QMutex m_mutex;
};

#endif // LOGGER_H
