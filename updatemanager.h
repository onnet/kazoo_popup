#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>

class QTimer;
class QNetworkReply;

class UpdateManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(UpdateManager)

public:
    static UpdateManager *instance();

    void start();

private:
    explicit UpdateManager(QObject *parent = 0);

    static UpdateManager *m_instance;
    QTimer *m_timer;

private slots:
    void checkUpdate();

    void onReplyFinished(QNetworkReply *reply);

};

#endif // UPDATEMANAGER_H
